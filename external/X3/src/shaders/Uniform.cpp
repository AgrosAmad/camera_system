#include <iostream>

#include <Uniform.h>
#include <Shader.h>

namespace X3
{
    Uniform::Uniform(const std::string& name, Shader* shaderProgram)
        : mName(name)
        , mOwner(shaderProgram)
    {
        mLocation = glGetUniformLocation(mOwner->GetID(), mName.c_str());
        if (mLocation == -1)
        {
            std::cout << "WARNING: uniform with name " << name << " does not exist, setting it will fail!" << std::endl;
        }
    }

    Uniform& Uniform::operator=(const glm::vec2& vector2D)
    {
        Set(vector2D);
        return *this;
    }

    // Family of functions setting vec2 uniforms

    void Uniform::Set(const glm::vec2& vector2D) const
    {
        glUniform2fv(mLocation, 1, reinterpret_cast<const GLfloat*>(&vector2D));
    }

    void Uniform::Set(const glm::vec2* vectors2D, GLsizei count) const
    {
        glUniform2fv(mLocation, count, reinterpret_cast<const GLfloat*>(vectors2D));
    }

    // Family of functions setting vec3 uniforms

    Uniform& Uniform::operator=(const glm::vec3& vector3D)
    {
        Set(vector3D);
        return *this;
    }

    void Uniform::Set(const glm::vec3& vector3D) const
    {
        glUniform3fv(mLocation, 1, reinterpret_cast<const GLfloat*>(&vector3D));
    }

    void Uniform::Set(const glm::vec3* vectors3D, GLsizei count) const
    {
        glUniform3fv(mLocation, count, reinterpret_cast<const GLfloat*>(vectors3D));
    }

    // Family of functions setting vec4 uniforms

    Uniform& Uniform::operator=(const glm::vec4& vector4D)
    {
        Set(vector4D);
        return *this;
    }

    void Uniform::Set(const glm::vec4& vector4D) const
    {
        glUniform4fv(mLocation, 1, reinterpret_cast<const GLfloat*>(&vector4D));
    }

    void Uniform::Set(const glm::vec4* vectors4D, GLsizei count) const
    {
        glUniform4fv(mLocation, count, reinterpret_cast<const GLfloat*>(vectors4D));
    }

    // Family of functions setting float uniforms

    Uniform& Uniform::operator=(const float floatValue)
    {
        Set(floatValue);
        return *this;
    }

    Uniform& Uniform::operator=(const std::vector<float>& floatValues)
    {
        Set(floatValues.data(), static_cast<GLsizei>(floatValues.size()));
        return *this;
    }

    void Uniform::Set(float floatValue) const
    {
        glUniform1fv(mLocation, 1, static_cast<const float*>(&floatValue));
    }

    void Uniform::Set(const float* floatValues, GLsizei count) const
    {
        glUniform1fv(mLocation, count, floatValues);
    }

    // Family of functions setting integer uniforms

    Uniform& Uniform::operator=(const int integerValue)
    {
        Set(integerValue);
        return *this;
    }

    void Uniform::Set(int integerValue) const
    {
        glUniform1iv(mLocation, 1, static_cast<const int*>(&integerValue));
    }

    void Uniform::Set(const int* integerValues, GLsizei count) const
    {
        glUniform1iv(mLocation, count, integerValues);
    }

    // Family of functions setting 3x3 matrices uniforms

    Uniform& Uniform::operator=(const glm::mat3& matrix)
    {
        Set(matrix);
        return *this;
    }

    void Uniform::Set(const glm::mat3& matrix) const
    {
        glUniformMatrix3fv(mLocation, 1, false, reinterpret_cast<const GLfloat*>(&matrix));
    }

    void Uniform::Set(const glm::mat3* matrices, GLsizei count) const
    {
        glUniformMatrix3fv(mLocation, count, false, reinterpret_cast<const GLfloat*>(matrices));
    }

    // Family of functions setting 4x4 matrices uniforms

    Uniform& Uniform::operator=(const glm::mat4& matrix)
    {
        Set(matrix);
        return *this;
    }

    void Uniform::Set(const glm::mat4& matrix) const
    {
        glUniformMatrix4fv(mLocation, 1, false, reinterpret_cast<const GLfloat*>(&matrix));
    }

    void Uniform::Set(const glm::mat4* matrices, GLsizei count) const
    {
        glUniformMatrix4fv(mLocation, count, false, reinterpret_cast<const GLfloat*>(matrices));
    }
}