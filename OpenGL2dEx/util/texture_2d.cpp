#include "texture_2d.h"
#include "logging.h"
#include <glad/glad.h>

namespace util {

	Texture2D::Texture2D()
		: width_{0}
		, height_{0}
		, internal_format_{GL_RGB}
		, image_format_{GL_RGB}
		, wrap_s_{GL_REPEAT}
		, wrap_t_{GL_REPEAT}
		, filter_min_{GL_LINEAR}
		, filter_max_{GL_LINEAR}
	{
		glGenTextures(1, &id_);
		ASSERT(id_ != 0, "Texture not properly generated");
	}

	void Texture2D::generate(const Dimension width, 
							 const Dimension height, 
							 unsigned char		*data,
							 bool               allow_no_data)
	{
		ASSERT(data || allow_no_data, "No image data");

		width_ = width;
		height_ = height;

		// create texture
		glBindTexture(GL_TEXTURE_2D, id_);
		glTexImage2D(GL_TEXTURE_2D, 0, internal_format_, width_, height_, 0, image_format_, GL_UNSIGNED_BYTE, data);

		// set texture wrap & filter modes
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_s_);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_t_);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter_min_);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter_max_);

		// unbind texture
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void Texture2D::bind() const
	{
		glBindTexture(GL_TEXTURE_2D, id_);
	}
}