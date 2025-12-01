#pragma once
#include <glad/glad.h>
#include <cstdint>
#include <iostream>


// SDK config file states GBRG but it seems it is actually RGGB!
enum class BayerPattern : int { RGGB=0, BGGR=1, GRBG=2, GBRG=3, None=4 };

class DemosaicFbo {
public:
    void Init(); // compile/link, setup quad
    void Render(GLuint srcTex, int w, int h,
                BayerPattern pattern,
                float blackLevel /*0..1*/,
                float gainR, float gainG, float gainB,
                float gamma);

    GLuint ColorTexture() const { return mColorTex; }

private:
    GLuint mProg = 0;
    GLuint mVao = 0, mVbo = 0;
    GLuint mFbo = 0, mColorTex = 0;
    int mTexW = 0, mTexH = 0;

    // uniforms
    GLint mURaw=-1, mUSize=-1, mUPattern=-1, mUBlack=-1, mUGain=-1, mUCCM=-1, mUGamma=-1;

    void EnsureTarget(int w, int h);
    GLuint MakeProgram(const char* vsSrc, const char* fsSrc);
};
