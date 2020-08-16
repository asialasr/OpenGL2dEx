#include "post_processor.h"

#include "logging.h"
#include "gl_debug.h"

namespace util {


PostProcessor::PostProcessor(const Shader &post_processing_shader,
							 unsigned int width,
							 unsigned int height)
	: post_processing_shader_{ post_processing_shader }
	, texture_{}
	, width_{ width }
	, height_{ height }
	, confuse_{ false }
	, chaos_{ false }
	, shake_{ false }
{
	glGenFramebuffers(1, &msfbo_);
	glGenFramebuffers(1, &fbo_);
	glGenRenderbuffers(1, &rbo_);

	// initialize renderbuffer storage w/ a multisampled color buffer
	glBindFramebuffer(GL_FRAMEBUFFER, msfbo_);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo_);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_RGB, width, height); // allocate storage for render buffer
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, rbo_);
	ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Failed to initialize MSFBO");

	// also initialize FBO/texture to blit multisampled color-buffer to
	glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
	texture_.generate(width, height, nullptr, true);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_.id(), 0);
	ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Failed to initialize FBO");
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	initialize_render_data();
	
	post_processing_shader_.use();
	post_processing_shader_.set_int("u_scene_", 0, true);
	static constexpr float offset = 1.0f / 300.0f;
	static const float offsets[9][2] = {
		{ -offset, offset },
		{ 0.0f,    offset },
		{ offset,  offset },
		{ -offset, 0.0f   },
		{ 0.0f,    0.0f   },
		{ offset,  0.0f   },
		{ -offset, -offset},
		{ 0.0f,    -offset},
		{ offset,  -offset}
	};
	glUniform2fv(glGetUniformLocation(post_processing_shader_.id(), "u_offsets_"), 9, (float*)offsets);
	
	const int edge_kernel[9] = {
		-1, -1, -1,
		-1,  8, -1,
		-1, -1, -1
	};
	glUniform1iv(glGetUniformLocation(post_processing_shader_.id(), "u_edge_kernel_"), 9, edge_kernel);

	const float blur_kernel[9] = {
		1.0f / 16.0f, 2.0f / 16.0f, 1.0f / 16.0f,
		2.0f / 16.0f, 4.0f / 16.0f, 2.0f / 16.0f,
		1.0f / 16.0f, 2.0f / 16.0f, 1.0f / 16.0f
	};
	glUniform1fv(glGetUniformLocation(post_processing_shader_.id(), "u_blur_kernel_"), 9, blur_kernel);
	
	check_for_gl_errors();
}

void PostProcessor::begin_render()
{
	glBindFramebuffer(GL_FRAMEBUFFER, msfbo_);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	check_for_gl_errors();
}

void PostProcessor::end_render()
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, msfbo_);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo_);
	glBlitFramebuffer(0, 0, width_, height_, 0, 0, width_, height_, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	check_for_gl_errors();
}

void PostProcessor::render(float time)
{
	post_processing_shader_.use();
	post_processing_shader_.set_float("u_time_", time, false);
	post_processing_shader_.set_bool("u_confuse_", confuse_, false);
	post_processing_shader_.set_bool("u_chaos_", chaos_, false);
	post_processing_shader_.set_bool("u_shake_", shake_, false);

	glActiveTexture(GL_TEXTURE0);
	texture_.bind();
	glBindVertexArray(vao_);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	check_for_gl_errors();
}

void PostProcessor::initialize_render_data()
{
	unsigned int vbo;
	const float vertices[] = {
		-1.0f, -1.0f, 0.0f, 0.0f,
		 1.0f,  1.0f, 1.0f, 1.0f,
		-1.0f,  1.0f, 0.0f, 1.0f,

		-1.0f, -1.0f, 0.0f, 0.0f,
		 1.0f, -1.0f, 1.0f, 0.0f,
		 1.0f,  1.0f, 1.0f, 1.0f
	};

	glGenVertexArrays(1, &vao_);
	glGenBuffers(1, &vbo);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindVertexArray(vao_);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	check_for_gl_errors();
}

} // namespace util