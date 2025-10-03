#pragma once

// STL
#include <map>
#include <string>
#include <fstream>
#include <set>
#include <memory>

// GL
#include <glad/glad.h>
#include <glm/glm.hpp>

// X3
#include <StringUtils.h>
#include <Uniform.h>

namespace X3
{

    // Different render passes
    enum class RenderPass { Forward, Transparent };

    // Cross-definition
    class Material;
    class Camera;
    class Light;

    // Shader wrapper
    class Shader : public std::enable_shared_from_this<Shader>
    {
    public:
        ~Shader();
        Shader() {};

        // Consturctor takes all shader file names
        Shader(const std::vector<std::string>& shadersFn);

        // Main-loop methods
        void Render();
        void Update();

        // Common methods
        void Unuse() const;
        void Use() const;
        void Delete();
        bool Link();

        // Get/Set
        void AddMaterial(const std::shared_ptr<Material>& material) { mMaterials.push_back(material); }
        int GetAttribLocation(std::string name) const;
        bool HasAttrib(const std::string name) const;
        GLuint GetID() const;

        // Gets uniform variable by name. If it's first time, it creates a new one, otherwise it's retrieved from cache.
        Uniform& operator[](const std::string& varName);
        Uniform& Unif(const std::string& varName);

        // Sets model and normal matrix
        void SetModelMat(const glm::mat4& modelMatrix);

        // Gets index of given uniform block in this shader program.
        GLuint GetUniformBlockIndex(const std::string& uniformBlockName) const;

        // Binds uniform block of this program to a uniform binding point.
        void BindUniformBlockToBindingPoint(const std::string& uniformBlockName, GLuint bindingPoint) const;

        // Tells OpenGL, which output variables should be recorded during transform feedback.
        void SetTransformFeedbackRecordedVariables(const std::vector<std::string>& recordedVariablesNames, GLenum bufferMode = GL_INTERLEAVED_ATTRIBS) const;

    private:

        // General info
        GLuint mProgramID{ 0 }; // OpenGL-assigned shader program ID
        bool mLinked{ false }; // Flag teling, whether shader program has been linked successfully

        // Shader data
        std::map<std::string, Uniform> mUniforms; // Cache of uniform locations (reduces OpenGL calls)
        std::map<GLenum, GLuint> mShaderPrograms; // Programs of this shader (vertex, fragment, etc.)
        std::map<std::string, int> mAttribs; // Cache of shader program attributes
        std::vector<std::string> mShadersFn; // Files containing shaders

        // Associated materials
        std::vector<std::weak_ptr<Material>> mMaterials;

        // Helpers
        bool GetLinesFromFile(const std::string& fileName, std::vector<std::string>& result, std::set<std::string>& filesIncludedAlready, bool isReadingIncludedFile = false) const;
        void CompileShaderProgram(const std::string& fn);
        GLenum ShaderTypeFromFn(const std::string& fn);
        void FindActiveAttribs();
        void AttachShaders();
        void DetachShaders();
        void DeleteShaders();
        void BuildShader();
    };
} // namespace X3