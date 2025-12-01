#include "DemosaicFbo.h"
#include <cstdio>
#include <cstring>

// Auto contained shader programs (for completion of code)
static const char* kVS = R"(#version 330 core
layout(location=0) in vec2 aPos;
layout(location=1) in vec2 aUV;
out vec2 vTex;
void main(){ vTex=aUV; gl_Position=vec4(aPos,0,1); })";

static const char* kFS = R"(#version 330 core
in vec2 vTex;
out vec4 FragColor;
uniform sampler2D uRaw;
uniform ivec2     uSize;
uniform int       uPattern; // 0=RGGB,1=BGGR,2=GRBG,3=GBRG, else: grayscale copy
uniform float     uBlack;
uniform vec3      uGain;
uniform mat3      uCCM;
uniform float     uGamma;

float fetchRaw(ivec2 p){ vec2 uv=(vec2(p)+0.5)/vec2(uSize); return texture(uRaw,uv).r; }

vec3 demosaic(ivec2 p){
    int x=p.x, y=p.y; bool xe=(x&1)==0, ye=(y&1)==0;
    float c=fetchRaw(p), l=fetchRaw(p+ivec2(-1,0)), r=fetchRaw(p+ivec2(1,0));
    float u=fetchRaw(p+ivec2(0,-1)), d=fetchRaw(p+ivec2(0,1));
    float ul=fetchRaw(p+ivec2(-1,-1)), ur=fetchRaw(p+ivec2(1,-1));
    float dl=fetchRaw(p+ivec2(-1,1)),  dr=fetchRaw(p+ivec2(1,1));
    bool isR=false,isG=false,isB=false;
    if(uPattern==0){ isR= ye&&xe; isB=!ye&&!xe; isG=!isR&&!isB; }        // RGGB
    else if(uPattern==1){ isB= ye&&xe; isR=!ye&&!xe; isG=!isR&&!isB; }   // BGGR
    else if(uPattern==2){ isG= ye&&xe; isR= ye&&!xe; isB=!ye&&xe; }      // GRBG
    else { isG= ye&&xe; isB= ye&&!xe; isR=!ye&&xe; }                     // GBRG
    vec3 rgb;
    if(isG){
        float rV=0.5*(l+r), bV=0.5*(u+d);
        bool swapRB =
            (uPattern==0 && ((ye&&!xe)||(!ye&&xe))) ||
            (uPattern==1 && ((ye&&!xe)||(!ye&&xe))) ||
            (uPattern==2 && (ye || (!ye&&!xe))) ||
            (uPattern==3 && (ye || (!ye&&!xe)));
        if(swapRB){ float t=rV; rV=bV; bV=t; }
        rgb=vec3(rV,c,bV);
    } else if(isR){
        float gV=0.25*(l+r+u+d);
        float bV=0.25*(ul+ur+dl+dr);
        rgb=vec3(c,gV,bV);
    } else {
        float gV=0.25*(l+r+u+d);
        float rV=0.25*(ul+ur+dl+dr);
        rgb=vec3(rV,gV,c);
    }
    return clamp(rgb,0.0,1.0);
}

void main(){
    ivec2 p=ivec2(vTex*vec2(uSize));
    vec3 rgb;
    if(uPattern>=0 && uPattern<=3) rgb=demosaic(p);
    else{ float g=fetchRaw(p); rgb=vec3(g); } // grayscale copy

    rgb = max(rgb - vec3(uBlack), 0.0);
    rgb *= uGain;
    rgb = uCCM * rgb;
    rgb = clamp(rgb, 0.0, 1.0);
    rgb = pow(rgb, vec3(1.0 / max(uGamma, 1e-6)));
    FragColor=vec4(rgb,1.0);
})";

static const float kQuad[] = {
    // pos   // uv
    -1.f,-1.f, 0.f,0.f,
     1.f,-1.f, 1.f,0.f,
    -1.f, 1.f, 0.f,1.f,
     1.f, 1.f, 1.f,1.f,
};

static GLuint compile(GLenum type, const char* src){
    GLuint s=glCreateShader(type);
    glShaderSource(s,1,&src,nullptr);
    glCompileShader(s);
    GLint ok=0; glGetShaderiv(s,GL_COMPILE_STATUS,&ok);
    if(!ok){ char log[2048]; glGetShaderInfoLog(s,2048,nullptr,log); std::fprintf(stderr,"[shader] compile err:\n%s\n",log); }
    return s;
}
GLuint DemosaicFbo::MakeProgram(const char* vsSrc, const char* fsSrc){
    GLuint v=compile(GL_VERTEX_SHADER,vsSrc);
    GLuint f=compile(GL_FRAGMENT_SHADER,fsSrc);
    GLuint p=glCreateProgram();
    glAttachShader(p,v); glAttachShader(p,f);
    glBindAttribLocation(p,0,"aPos");
    glBindAttribLocation(p,1,"aUV");
    glLinkProgram(p);
    glDeleteShader(v); glDeleteShader(f);
    GLint ok=0; glGetProgramiv(p,GL_LINK_STATUS,&ok);
    if(!ok){ char log[2048]; glGetProgramInfoLog(p,2048,nullptr,log); std::fprintf(stderr,"[shader] link err:\n%s\n",log); }
    return p;
}

void DemosaicFbo::EnsureTarget(int w,int h){
    if (w<=0 || h<=0) return;
    if (w==mTexW && h==mTexH && mColorTex && mFbo) return;
    if (!mFbo) glGenFramebuffers(1,&mFbo);
    if (!mColorTex) glGenTextures(1,&mColorTex);
    glBindTexture(GL_TEXTURE_2D, mColorTex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glBindFramebuffer(GL_FRAMEBUFFER, mFbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mColorTex, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    mTexW=w; mTexH=h;
}

void DemosaicFbo::Init(){
    // require current GL context
    if (!glGetString(GL_VERSION)) { std::fprintf(stderr,"[DemosaicFbo] No GL context\n"); return; }

    // quad
    glGenVertexArrays(1,&mVao);
    glGenBuffers(1,&mVbo);
    glBindVertexArray(mVao);
    glBindBuffer(GL_ARRAY_BUFFER, mVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(kQuad), kQuad, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,4*sizeof(float),(void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,4*sizeof(float),(void*)(2*sizeof(float)));
    glBindVertexArray(0);

    // program
    mProg = MakeProgram(kVS, kFS);
    mURaw     = glGetUniformLocation(mProg, "uRaw");
    mUSize    = glGetUniformLocation(mProg, "uSize");
    mUPattern = glGetUniformLocation(mProg, "uPattern");
    mUBlack   = glGetUniformLocation(mProg, "uBlack");
    mUGain    = glGetUniformLocation(mProg, "uGain");
    mUCCM     = glGetUniformLocation(mProg, "uCCM");
    mUGamma   = glGetUniformLocation(mProg, "uGamma");
}

void DemosaicFbo::Render(GLuint srcTex, int w, int h,
                         BayerPattern pattern,
                         float blackLevel, float gainR, float gainG, float gainB,
                         float gamma)
{
    EnsureTarget(w,h);
    glBindFramebuffer(GL_FRAMEBUFFER, mFbo);
    glViewport(0,0,mTexW,mTexH);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, srcTex);

    glUseProgram(mProg);
    glUniform1i(mURaw, 0);
    glUniform2i(mUSize, w, h);
    glUniform1i(mUPattern, (pattern==BayerPattern::None)? 999 : (int)pattern);
    glUniform1f(mUBlack, blackLevel);
    glUniform3f(mUGain, gainR, gainG, gainB);
    // identity CCM
    const float I3[9] = {1,0,0, 0,1,0, 0,0,1};
    glUniformMatrix3fv(mUCCM, 1, GL_FALSE, I3);
    glUniform1f(mUGamma, gamma);

    glBindVertexArray(mVao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);

    glUseProgram(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

}
