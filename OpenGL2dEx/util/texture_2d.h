#ifndef TEXTURE_2D_H
#define TEXTURE_2D_H

namespace util {

// Texture2D is able to store and configure a texture in OpenGL.
// It also hosts utility functions for easy management.
class Texture2D {
public:
	Texture2D();

	void generate(unsigned int width, unsigned int height, unsigned char* data);

	void bind() const;

	void set_internal_format(const unsigned int internal_format)
	{
		internal_format_ = internal_format;
	}

	void set_image_format(const unsigned int image_format)
	{
		image_format_ = image_format;
	}

	void set_wrap_s(const unsigned int wrap_s)
	{
		wrap_s_ = wrap_s;
	}

	void set_wrap_t(const unsigned int wrap_t)
	{
		wrap_t_ = wrap_t;
	}

	void set_filter_min(const unsigned int filter_min)
	{
		filter_min_ = filter_min;
	}

	void set_filter_max(const unsigned int filter_max)
	{
		filter_max_ = filter_max;
	}

private:
	// holds the ID of the texture object, used for all texture operations to reference 
	// this particular texture
	unsigned int id_;

	// texture image dimensions
	unsigned int width_; // width of loaded image in pixels
	unsigned int height_; // height of loaded image in pixels

	// texture format
	unsigned int internal_format_; // format of texture object
	unsigned int image_format_; // format of loaded image

	// texture configuration
	unsigned int wrap_s_; // wrapping mode on s-axis
	unsigned int wrap_t_; // wrapping mode on t-axis
	unsigned int filter_min_; // filter mode if texture pixels < screen pixels
	unsigned int filter_max_; // filter mode if texture pixels > screen pixels
};

} // namespace util

#endif // TEXTURE_2D_H