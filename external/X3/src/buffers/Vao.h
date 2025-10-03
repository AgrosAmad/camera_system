#pragma once

// STL
#include <iostream>
#include <vector>

// GL
#include <glad/glad.h>

// XE
#include <Shader.h>
#include <Vbo.h>

namespace X3
{
	// Wraps vertex atributes
	class VertexAttrib
	{
	public:
		// Construct by name
		VertexAttrib(std::shared_ptr<GLBufferBase> vbo, std::string name, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid* pointer) :
			mVbo(vbo), mName(name), mSize(size), Type(type), mNormalized(normalized), mStride(stride), mPointer(pointer) {}

		// Construct by ID
		VertexAttrib(std::shared_ptr<GLBufferBase> vbo, GLuint id, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid* pointer) :
			mVbo(vbo), mID(id), mSize(size), Type(type), mNormalized(normalized), mStride(stride), mPointer(pointer) {}

		// Data
		std::shared_ptr<GLBufferBase> mVbo;
		std::string mName;
		GLuint mID;
		GLint mSize;
		GLenum Type;
		GLboolean mNormalized;
		GLsizei mStride;
		const GLvoid* mPointer;
	};

	class Vao
	{
	public:

		// Constructor
		Vao();
		~Vao();
		Vao(const std::vector<VertexAttrib>& attributes);

		// Common methods
		void AddAttribs(const std::vector<VertexAttrib>& attributes, bool deleteOld = true);
		void UpdateVertAttribPointers(const std::shared_ptr<Shader>& shader = nullptr);
		void BindEbo(const std::shared_ptr<GLBufferBase>& ebo);
		void DeleteAttribs();
		void Unbind();
		void Bind();
	private:
		GLuint mID; // OpenGL assigned index
		std::vector<VertexAttrib> mAttributes; // vao attributes

		// Helper functions
		void SetVertexAttribPointer(const GLuint& index, const VertexAttrib& attrib);
		void Generate();
	};
} // namespace XE