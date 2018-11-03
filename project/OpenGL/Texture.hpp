#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include "Base/Base.hpp"
#include "OpenGL/OpenGL.hpp"

class Texture
{
public:
	typedef std::function<void(void)> HandleFunc;

	Texture()
	{
		glGenTextures(1, &texture_);
	};

	~Texture()
	{
		glDeleteTextures(1, &texture_);
	};

	virtual void Use(uint32_t unit)
	{
		glActiveTexture((uint32_t)GL_TEXTURE0 + unit);
		glBindTexture(GL_TEXTURE_2D, texture_);
	};

	virtual void Import(const std::string& resource)
	{
		glBindTexture(GL_TEXTURE_2D, texture_);
		Mode();
		Load(resource);
		glBindTexture(GL_TEXTURE_2D, 0);
	};
	
	virtual void Mode() = 0;

	virtual bool Load(const std::string& resource) = 0;

protected:
	void Mode(uint32_t min_filter, uint32_t mag_filter, uint32_t s_wrap, uint32_t t_wrap,  float* s_border_color = nullptr, float* t_border_color = nullptr)
	{ 
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filter);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, s_wrap);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, t_wrap);
		if (s_wrap == GL_CLAMP_TO_BORDER)
		{
			if (s_border_color == nullptr)
			{
				float board_color[] = { 1.0f, 1.0f, 0.0f, 1.0f };
				glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, board_color);
			}
			else
			{
				glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, s_border_color);
			}
		}
		
		if (t_wrap == GL_CLAMP_TO_BORDER)
		{
			if (t_border_color == nullptr)
			{
				float board_color[] = { 1.0f, 1.0f, 0.0f, 1.0f };
				glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, board_color);
			}
			else
			{
				glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, t_border_color);
			}
		}
	};

public:
	bool loaded() { return loaded_; };
	bool index() { return texture_; };

protected:
	unsigned int texture_;
	bool loaded_ = false;
};

class SimpleTexture : public Texture
{
public:
	SimpleTexture() : Texture()
	{};

	virtual ~SimpleTexture()
	{};

	bool Load(const std::string& resource)
	{
		glBindTexture(GL_TEXTURE_2D, texture_);
		int width, height, channels;
		stbi_set_flip_vertically_on_load(true);
		unsigned char* data = stbi_load(resource.c_str(), &width, &height, &channels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
			
			loaded_ = true;
			return true;
		}
		else
		{
			std::cerr << "[Texture] Failed to load texture! texture is " << resource << std::endl;
			return false;
		}
	};

	void Mode()
	{
		Texture::Mode(GL_NEAREST, GL_NEAREST, GL_REPEAT, GL_REPEAT);
	};
};

class MipMapTexture : public Texture
{
public:
	MipMapTexture() : Texture()
	{};

	bool Load(const std::string& resource)
	{
		glBindTexture(GL_TEXTURE_2D, texture_);
		int width, height, channels;
		stbi_set_flip_vertically_on_load(true);
		unsigned char* data = stbi_load(resource.c_str(), &width, &height, &channels, 0);
		if (data)
		{
			if (boost::ends_with(resource, ".jpg"))
			{
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			}
			else if (boost::ends_with(resource, ".png"))
			{
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			}
			else
			{
				std::cerr << "[Texture] resource suffix is not valid! resource is " << resource << std::endl;
			}
			
			glGenerateMipmap(GL_TEXTURE_2D);
			stbi_image_free(data);

			loaded_ = true;
			return true;
		}
		else
		{
			std::cerr << "[Texture] Failed to load texture! texture is " << resource << std::endl;
			return false;
		}
	};

	void Mode()
	{
		Texture::Mode(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_LINEAR, GL_REPEAT, GL_REPEAT);
	};
};

#endif