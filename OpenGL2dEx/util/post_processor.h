#ifndef POST_PROCESSOR_H
#define POST_PROCESSOR_H

#include "resource_mgr.h"
#include "shader.h"
#include "types.h"

#include "glm/glm.hpp"

namespace util {

class IResetGlProperties;

class PostProcessor {
public:
	PostProcessor(const IResetGlProperties& gl_property_resetter,
				  const Shader       &post_processing_shader,
				  glm::vec2          position,
				  unsigned int       width,
				  unsigned int       height,
				  const glm::mat4    &projection);

	void begin_render();
	void end_render();
	void render(float time);

	bool confuse() const
	{
		return confuse_;
	}

	void set_confuse(bool confuse)
	{
		confuse_ = confuse;
	}

	bool chaos() const
	{
		return chaos_;
	}

	void set_chaos(bool chaos)
	{
		chaos_ = chaos;
	}

	bool shake() const
	{
		return shake_;
	}

	void set_shake(bool shake)
	{
		shake_ = shake;
	}

	void clear_effects()
	{
		confuse_ = chaos_ = shake_ = false;
	}

	void set_size(const Dimension width, const Dimension height)
	{
		width_ = width;
		height_ = height;
		texture_.generate(width, height, nullptr, true);
	}

	void set_position(const glm::vec2 &position)
	{
		position_ = position;
	}

private:
	void initialize_render_data();

	const IResetGlProperties& gl_property_resetter_;
	Shader post_processing_shader_;
	Texture2D texture_;
	glm::vec2 position_;
	Dimension width_;
	Dimension height_;

	bool confuse_;
	bool chaos_;
	bool shake_;

	unsigned int msfbo_; // multisampled FBO
	unsigned int fbo_;
	unsigned int rbo_;
	unsigned int vao_;
};

} // namespace util

#endif // POST_PROCESSOR_H