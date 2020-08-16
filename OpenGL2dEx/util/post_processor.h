#ifndef POST_PROCESSOR_H
#define POST_PROCESSOR_H

#include "resource_mgr.h"
#include "shader.h"

namespace util {

class PostProcessor {
public:
	PostProcessor(const Shader &post_processing_shader,
				  unsigned int width,
				  unsigned int height);

	void begin_render();
	void end_render();
	void render(float time);

	void set_confuse(bool confuse)
	{
		confuse_ = confuse;
	}

	void set_chaos(bool chaos)
	{
		chaos_ = chaos;
	}

	void set_shake(bool shake)
	{
		shake_ = shake;
	}

private:
	void initialize_render_data();

	Shader post_processing_shader_;
	Texture2D texture_;
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