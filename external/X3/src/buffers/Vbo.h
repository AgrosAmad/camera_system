#pragma once

#include "GLBuffer.h"

namespace X3
{
    template <class T> class Vbo : public GLBuffer<T>
    {
    public:
        Vbo(std::vector<T> data = std::vector<T>{}, GLenum usage = GL_STATIC_DRAW, GLenum type = GL_ARRAY_BUFFER) : GLBuffer<T>(type, usage)
        {
            this->AddData(data);
        };
    };
} // namespace X3