#pragma once

// X3
#include <GLBuffer.h>

namespace X3
{
    // Wraps uniform buffer objects
    template <class T> class Ubo : public GLBuffer<T>
    {
    public:
        // Constructor
        Ubo(std::vector<T> data = std::vector<T>{}, GLenum usage = GL_DYNAMIC_DRAW) : GLBuffer<T>(GL_UNIFORM_BUFFER, usage)
        {
            this->AddData(data);
        };

        void UniformBlockBinding(const GLuint& shader_program, const GLuint& uniform_block_index, const GLuint& uniform_block_binding)
        {
            glUniformBlockBinding(shader_program, uniform_block_index, uniform_block_binding);
        }

        void BindBufferBase(const GLuint& binding_index)
        {
            glBindBufferBase(this->mType, binding_index, this->mID);
        }

        void BindBuffersRange(const GLuint& binding_index, const GLintptr& offset, const GLsizeiptr& size)
        {
            glBindBufferRange(this->mType, binding_index, this->mID, offset, size);
        }
    };
} // namespace X3