#include "Texture2D.h"


// STL
#include <sys/stat.h>

// GL
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace X3
{
	Texture2D::Texture2D(const std::string& fName, GLenum filterMin, GLenum filterMag, GLint wrap, int comp)
	{

		// Setup internals
		unsigned char* imgData = nullptr;
		GLenum internalFormat = GL_RGBA;
		GLenum type = GL_UNSIGNED_BYTE;
		GLenum format = GL_RGBA;
		int w = 0;
		int h = 0;

		// Loads
		struct stat buffer;
		if (stat(fName.c_str(), &buffer) == 0)
		{
			int numChannels;
			stbi_set_flip_vertically_on_load(1);
			imgData = stbi_load(fName.c_str(), &w, &h, &numChannels, comp);

			// Single channel
			if (comp == STBI_grey)
			{
				internalFormat = GL_R8;
				format = GL_RED;
			}

			else if (stbi_is_hdr(fName.c_str()))
			{
				internalFormat = GL_RGB32F;
				wrap = GL_CLAMP_TO_EDGE;
				format = GL_RGB;
				type = GL_FLOAT;
			}
		}
		else
		{
			printf("Texture at %s was not found\n", fName.c_str());
		}

		// Parameters
		mParams.InternalFormat = internalFormat;
		mParams.Target = GL_TEXTURE_2D;
		mParams.FilterMin = filterMin;
		mParams.FilterMag = filterMag;
		mParams.Format = format;
		mParams.Type = type;
		mParams.Wrap = wrap;
		mParams.Samples = 0;
		mParams.Height = h;
		mParams.Width = w;
		mResident = false;
		mID = 0;

		// Generate and clean
		GenTexture();

		if (imgData)
		{
			Data(imgData);
			stbi_image_free(imgData);
		}

	}

	Texture2D::Texture2D(GLenum target, void* data, GLenum internalFormat, int width, int height, GLenum format, GLenum type, GLenum filterMin, GLenum filterMag, GLint wrap)
	{
		mResident = false;
		mID = 0;

		mParams.InternalFormat = internalFormat;
		mParams.FilterMag = filterMag;
		mParams.FilterMin = filterMin;
		mParams.Target = target;
		mParams.Format = format;
		mParams.Height = height;
		mParams.Width = width;
		mParams.Type = type;
		mParams.Wrap = wrap;
		mParams.Samples = 0;

		GenTexture();
		Data(data);
	}

	Texture2D::Texture2D(const glm::vec4& color, const int& width, const int& height, GLenum filterMin, GLenum filterMag, GLint wrap)
	{
		mResident = false;
		mID = 0;

		mParams.InternalFormat = GL_RGBA;
		mParams.FilterMag = filterMag;
		mParams.FilterMin = filterMin;
		mParams.Target = GL_TEXTURE_2D;
		mParams.Format = GL_RGBA;
		mParams.Height = height;
		mParams.Width = width;
		mParams.Type = GL_UNSIGNED_BYTE;
		mParams.Wrap = wrap;
		mParams.Samples = 0;

		// Create a buffer to hold the color data for the entire texture
		GLubyte* textureData = new GLubyte[width * height * 4];
		#pragma omp parallel for
		for (int i = 0; i < width * height; ++i)
		{
			textureData[i * 4 + 0] = color[0];
			textureData[i * 4 + 1] = color[1];
			textureData[i * 4 + 2] = color[2];
			textureData[i * 4 + 3] = color[3];
		}

		GenTexture();
		Data(textureData);
	}

	Texture2D::Texture2D(const Texture2DParams& params, void* data)
	{
		mResident = false;
		mID = 0;

		mParams = params;
		GenTexture();
		Data(data);
	}

	Texture2D::~Texture2D()
	{
		DeleteTexture();
	}

	void Texture2D::Bind(GLenum text)
	{
		glActiveTexture(text);
		glBindTexture(mParams.Target, mID);
	}

	void Texture2D::Unbind(GLenum text)
	{
		glActiveTexture(text);
		glBindTexture(mParams.Target, 0);
	}

	void Texture2D::Resize(glm::ivec2 wh)
	{
		Resize(wh.x, wh.y);
	}

	void Texture2D::Resize(int w, int h)
	{
		if (mResident)
		{
			printf("Texture: texture resident, can not resize\n");
		}
		else if (mParams.Width != w && mParams.Height != h)
		{
			mParams.Height = h;
			mParams.Width = w;

			Data(0);
		}
	}

	void Texture2D::Data(void* data)
	{
		if (mParams.Target == GL_TEXTURE_2D_MULTISAMPLE)
		{
			glBindTexture(mParams.Target, mID);
			glTexImage2DMultisample(mParams.Target, mParams.Samples, mParams.InternalFormat, mParams.Width, mParams.Height, GL_TRUE);
			glBindTexture(mParams.Target, 0);
		}
		else
		{
			glBindTexture(mParams.Target, mID);
			glTexImage2D(mParams.Target, 0, mParams.InternalFormat, mParams.Width, mParams.Height, 0, mParams.Format, mParams.Type, data);
			glBindTexture(mParams.Target, 0);

			Filters(mParams.FilterMin, mParams.FilterMag);
			Wrap(mParams.Wrap);
		}

		if (MipMapEnabled())
		{
			glTexParameteri(mParams.Target, GL_TEXTURE_BASE_LEVEL, 0);
			glTexParameteri(mParams.Target, GL_TEXTURE_MAX_LEVEL, 3);

			GenMipMaps();
		}
	}

	void Texture2D::GenMipMaps()
	{
		if (!MipMapEnabled()) return;

		glBindTexture(mParams.Target, mID);
		glGenerateMipmap(mParams.Target);
		glBindTexture(mParams.Target, 0);
	}

	void Texture2D::Filters(const GLenum& filterMin, const GLenum& filterMag)
	{
		mParams.FilterMin = filterMin;
		mParams.FilterMag = filterMag;

		glBindTexture(mParams.Target, mID);
		glTexParameteri(mParams.Target, GL_TEXTURE_MIN_FILTER, mParams.FilterMin);
		glTexParameteri(mParams.Target, GL_TEXTURE_MAG_FILTER, mParams.FilterMag);
		glBindTexture(mParams.Target, 0);
	}

	void Texture2D::Wrap(const GLint& wrap)
	{
		mParams.Wrap = wrap;

		glBindTexture(mParams.Target, mID);
		glTexParameteri(mParams.Target, GL_TEXTURE_WRAP_S, mParams.Wrap);
		glTexParameteri(mParams.Target, GL_TEXTURE_WRAP_T, mParams.Wrap);
		glBindTexture(mParams.Target, 0);

	}

	void Texture2D::Target(const GLenum& t)
	{
		mParams.Target = t;
	}

	bool Texture2D::MipMapEnabled()
	{
		if (mParams.FilterMin == GL_LINEAR_MIPMAP_LINEAR ||
			mParams.FilterMin == GL_LINEAR_MIPMAP_NEAREST ||
			mParams.FilterMin == GL_NEAREST_MIPMAP_LINEAR ||
			mParams.FilterMin == GL_NEAREST_MIPMAP_NEAREST)
			return true;
		else
			return false;
	}

	void Texture2D::DeleteTexture()
	{
		glDeleteTextures(1, &mID);
	}

	void Texture2D::GenTexture()
	{
		mID = 0;
		glGenTextures(1, &mID);
	}
}