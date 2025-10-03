#pragma once

// STL
#include <vector>

// GL
#include <glad/glad.h>

namespace X3
{
    class GLBufferBase
    {
    public:
        virtual GLuint GetID() = 0;
        virtual void Unbind() {};
        virtual void Bind() {};
    };

    template <class T> class GLBuffer : public GLBufferBase
    {
    public:

        // Constructor from type and usage hint
        GLBuffer(GLenum type, GLenum usage) :
            mID(0),
            mType(type),
            mUsage(usage),
            mNumElements(0)
        {
            glGenBuffers(1, &mID);
        };

        // Delete when destroyed
        ~GLBuffer()
        {
            if (mID != 0) glDeleteBuffers(1, &mID);
        }

        void Bind()
        {
            glBindBuffer(mType, mID);
        }

        void Unbind()
        {
            glBindBuffer(mType, 0);
        }

        // Data uploaders
        void AddSubData(const T* data, size_t offset = 0, size_t numElements = 1)
        {
            Bind();
            glBufferSubData(mType, offset * sizeof(T), numElements * sizeof(T), data);
            Unbind();

            mNumElements = ComputeNumElements();
        }

        void AddData(const T* data, size_t numElements = 1)
        {

            Bind();
            glBufferData(mType, numElements * sizeof(T), data, mUsage);
            Unbind();

            mNumElements = ComputeNumElements();
        }

        void AddData(const std::vector<T>& data)
        {

            Bind();
            glBufferData(mType, data.size() * sizeof(T), data.data(), mUsage);
            Unbind();

            mNumElements = ComputeNumElements();
        }

        void AddData(const GLsizeiptr& numElements)
        {
            Bind();
            glBufferData(mType, numElements * sizeof(T), 0, mUsage);
            Unbind();

            mNumElements = ComputeNumElements();
        }

        // Map from OpenGL to CPU
        T* MapBuffer(GLenum access) 
        {
            Bind();
            void* ptr = glMapBuffer(mType, access);
            if (!ptr) 
            {
                throw std::runtime_error("Failed to map OpenGL buffer!");
            }
            return static_cast<T*>(ptr);
        }

        void UnmapBuffer()
        {
            glUnmapBuffer(mType);
            Unbind();
        }

        // Compute number of elements
        unsigned int ComputeNumElements()
        {
            GLint nBufferSize = 0;
            Bind();
            glGetBufferParameteriv(mType, GL_BUFFER_SIZE, &nBufferSize);
            Unbind();

            return (nBufferSize / sizeof(T));
        }

        int NumElements() { return mNumElements; }
        GLuint GetID() { return mID; }

    protected:

        // Buffer data
        GLuint mID;
        GLenum mType;
        GLenum mUsage;
        GLuint mNumElements;

        // Helper functions
        void Generate()
        {
            glGenBuffers(1, &mID);
        }
    }; // class
} // namespace X3