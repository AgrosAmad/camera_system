#pragma once

// STL
#include <iostream>
#include <string>
#include <vector>

// XE
#include <Texture.h>

namespace X3
{
	// Texture parameter container
	struct Texture2DParams
	{
		GLenum InternalFormat;
		GLint Width, Height;
		GLenum FilterMin;
		GLenum FilterMag;
		GLsizei Samples;
		GLenum Target;
		GLenum Format;
		GLenum Type;
		GLint Wrap;
	};

	class Texture2D : public Texture
	{
	public:

		// Constructors
		Texture2D(const std::string& fName, GLenum filterMin = GL_LINEAR_MIPMAP_LINEAR, GLenum filterMag = GL_LINEAR, GLint wrap = GL_REPEAT, int comp = STBI_rgb_alpha);
		Texture2D(GLenum target, void* data, GLenum internalFormat, int width, int height, GLenum format, GLenum type, GLenum filterMin, GLenum filterMag, GLint wrap);
		Texture2D(const glm::vec4& color, const int& width = 1920, const int& height = 1080, GLenum filterMin = GL_LINEAR_MIPMAP_LINEAR, GLenum filterMag = GL_LINEAR, GLint wrap = GL_REPEAT);
		Texture2D(const Texture2DParams& params, void* data = nullptr);
		~Texture2D();

		// Common methods
		void Bind(GLenum text);
		void Unbind(GLenum text);
		void Resize(glm::ivec2 wh);
		void Resize(int w, int h);
		void Data(void* data);
		void GenMipMaps();

		// Set/Get
		GLenum InternalFormat() const { return mParams.InternalFormat; }
		GLenum FilterMin() const { return mParams.FilterMin; }
		GLenum FilterMag() const { return mParams.FilterMag; }
		Texture2DParams const Params() { return mParams; }
		GLenum Format() const { return mParams.Format; }
		GLenum Target() const { return mParams.Target; }
		GLint Height() const { return mParams.Height; }
		GLint Width() const { return mParams.Width; }
		GLenum Type() const { return mParams.Type; }
		GLint Wrap() const { return mParams.Wrap; }
		bool IsResident() { return mResident; }
		GLuint Id() { return mID; }

		// Can be set on the fly
		void Filters(const GLenum& filterMin, const GLenum& filterMax);
		void MakeHandleResident(const GLuint64& handle) {};
		void Wrap(const GLint& wrap);
		void Target(const GLenum& t);


	private:

		// Texture data
		Texture2DParams mParams;
		bool mResident;
		GLuint mID;

		// Helpers
		bool MipMapEnabled();
		void DeleteTexture();
		void GenTexture();

	};
} // namespace X3