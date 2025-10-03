#include <Light.h>

namespace X3
{
    Light::Light(const std::string& name, const LightType& t, const glm::vec3& c, const glm::vec3& p, const glm::vec3& dir, float intensity, LightRender renderType, bool hasShadows)
    {
        ViewDir(dir);
        mHasShadows = hasShadows;
        mRenderType = renderType;
        mIntensity = intensity;
        mName = name;
        mColor = c;
        mType = t;

        // Additional data
        mAttenuation = glm::vec3(1.f, 0.09f, 0.032f);
        mConeAngle = glm::vec2(12.5f, 17.5f);
        mViewDir = dir;
        mPosition = p;


        // Prepares render data
        switch (mRenderType)
        {
        case None:
            break;
        case Cube:
        {
            mPrimitive = GL_TRIANGLES;

            // Vertex pos + texture coordinates
            std::vector<float> vertices = {
            -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
             0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
             0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
             0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
             0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
             0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
             0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
            -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

            -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
            -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
            -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

             0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
             0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
             0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
             0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
             0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
             0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
             0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
             0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
             0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
             0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
             0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
             0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
            -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
            };

            // Upload to GPU
            mVertices = std::make_shared<Vbo<float>>(vertices);

            mVao = std::make_shared<Vao>(std::vector<VertexAttrib>{
                VertexAttrib(mVertices, 0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0),
                VertexAttrib(mVertices, 1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)))
            });

            // Camera
            mCam = std::make_shared<CameraFPS>();
            break;
        }
        case Sphere:
            break;
        default:
            break;
        }
    }

    void Light::Render()
    {

        switch (mRenderType)
        {
        case None:
            break;
        case Cube:
        {
            mCam->InputHandling();
            mCam->Update();

            Shaders& shaders = Shaders::Inst();
            Parameters& params = Parameters::Inst();
            std::shared_ptr<Shader> shade = shaders.Load("light", params.Main.RootDir + "/shaders/test/lamp");

            shade->Use();
            shade->Unif("view") = mCam->MatV();
            shade->Unif("proj") = mCam->MatP(params.WindowRatio());
            shade->Unif("lightColor") = mColor;
            shade->Unif("intensity") = mIntensity;
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, mPosition);
            model = glm::scale(model, glm::vec3(0.2));
            shade->Unif("model") = model;
            mVao->Bind();
            glDrawArrays(GL_TRIANGLES, 0, 36);
            mVao->Unbind();
            shade->Unuse();
            break;
        }
        case Sphere:
            break;
        default:
            break;
        }
    }

    void Light::RenderUi()
    {
        ImGui::Text("Name: %s", mName.c_str());
        ImGui::SliderFloat3("Position", &mPosition.x, -2.0f, 2.0f, "%.5f");
        ImGui::SliderFloat3("Direction", &mViewDir.x, -1.0f, 1.0f, "%.5f");
        ImGui::ColorEdit3("Color", &mColor.x);
        ImGui::SliderFloat("Cone inner", &mConeAngle.x, 0.f, 90.f, "%.5f");
        ImGui::SliderFloat("Cone out", &mConeAngle.y, mConeAngle.x, 90.f, "%.5f");
        ImGui::SliderFloat("Intensity", &mIntensity, 0.0f, 5.0f, "%.5f");
    }

    void Light::Update()
    {
    }

    UboLight Light::UboParams()
    {
        UboLight params;

        params.Attenuation = mAttenuation;
        params.Position = mPosition;
        params.ViewDir = mViewDir;
        params.Intensity = mIntensity;
        params.ConeAngleCos = ConeAngleCos();
        params.Ambient = glm::vec3(0.f);
        params.Diffuse = mColor;
        params.Specular = mColor;

        return params;
    }

}