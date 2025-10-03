#pragma once

// STL
#include <vector>
#include <string>

// GL
#include <glm/glm.hpp>
#include <glad/glad.h>

namespace X3
{
    // Forward class declaration of ShaderProgram (because of cross-inclusion)
    class Shader;

    /**
     * Wraps OpenGL shader uniform variable.
     */
    class Uniform
    {
    public:
        Uniform() = default; // Required to work with ShaderProgram [] operator
        Uniform(const std::string& name, Shader* shaderProgram);

        // Family of functions setting vec2 uniforms
        Uniform& operator=(const glm::vec2& vector2D);
        void Set(const glm::vec2& vector2D) const;
        void Set(const glm::vec2* vectors2D, GLsizei count = 1) const;

        // Family of functions setting vec3 uniforms
        Uniform& operator=(const glm::vec3& vector3D);
        void Set(const glm::vec3& vector3D) const;
        void Set(const glm::vec3* vectors3D, GLsizei count = 1) const;

        // Family of functions setting vec4 uniforms
        Uniform& operator=(const glm::vec4& vector4D);
        void Set(const glm::vec4& vector4D) const;
        void Set(const glm::vec4* vectors4D, GLsizei count = 1) const;

        // Family of functions setting float uniforms
        Uniform& operator=(float floatValue);
        Uniform& operator=(const std::vector<float>& floatValues);
        void Set(float floatValue) const;
        void Set(const float* floatValues, GLsizei count = 1) const;

        // Family of functions setting integer uniforms
        Uniform& operator=(int integerValue);
        void Set(int integerValue) const;
        void Set(const int* integerValues, GLsizei count = 1) const;

        // Family of functions setting 3x3 matrices uniforms
        Uniform& operator=(const glm::mat3& matrix);
        void Set(const glm::mat3& matrix) const;
        void Set(const glm::mat3* matrices, GLsizei count = 1) const;

        // Family of functions setting 4x4 matrices uniforms
        Uniform& operator=(const glm::mat4& matrix);
        void Set(const glm::mat4& matrix) const;
        void Set(const glm::mat4* matrices, GLsizei count = 1) const;

    private:
        std::string mName; // Name of the uniform variable
        Shader* mOwner{ nullptr }; // Pointer to shader program this uniform belongs to
        GLint mLocation{ -1 }; // OpenGL assigned uniform location (cached in this variable)
    };
} // namespace X3