#ifndef POST_PROCESSOR_H
#define POST_PROCESSOR_H

#include "resource_mgr.h"
#include "shader.h"

#include "glm/glm.hpp"

namespace util {

class PostProcessor {
public:
	PostProcessor(const Shader       &post_processing_shader,
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

private:
	void initialize_render_data();

	Shader post_processing_shader_;
	Texture2D texture_;
	glm::vec2 position_;
	unsigned int width_;
	unsigned int height_;

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