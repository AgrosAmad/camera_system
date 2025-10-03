#pragma once

// GL
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <stb_image.h>

namespace X3
{
    class Texture
    {
    public:

        // Common methods
        virtual void Bind(GLenum) {};
        virtual void Unbind(GLenum) {};

        // Get/Set
        virtual GLenum InternalFormat() { return 0; }
        virtual GLenum Target() { return 0; }
        virtual GLenum Filters() { return 0; }
        virtual GLenum Format() { return 0; }
        virtual GLenum Type() { return 0; }
        virtual GLint Wrap() { return 0; }
        virtual GLuint Id() { return 0; }
    };
} // namespace X3