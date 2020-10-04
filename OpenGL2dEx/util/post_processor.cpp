#include "post_processor.h"

#include "logging.h"
#include "gl_debug.h"
#include "reset_gl_properties.h"

namespace util {


PostProcessor::PostProcessor(const IResetGlProperties& gl_property_resetter,
							 const Shader    &post_processing_shader,
							 glm::vec2       position,
							 unsigned int    width,
							 unsigned int    height,
							 const glm::mat4 &projection)
	: gl_property_resetter_{ gl_property_resetter }
	, post_processing_shader_ { post_processing_shader }
	, texture_{}
	, position_{ position }
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
	post_processing_shader_.set_mat4("u_projection_", projection, false);
	post_processing_shader_.set_int("u_scene_", 0, false);
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
	glViewport(0, 0, width_, height_);
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

	gl_property_resetter_.reset_fbo();
	gl_property_resetter_.reset_viewport();

	check_for_gl_errors();
}

void PostProcessor::render(float time)
{
	// TODO(sasiala): I'm 90% sure there's a far better way to accomplish all of this.
	// model & projection are used to allow this to behave as a window within a game.
	// e.g. the game level could be displayed next to the list of levels instead of 
	// as the background
	auto model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3{ position_ + glm::vec2{width_ * .5, height_ * .5}, 0.0f });
	
	// without the rotation, the game is displayed upside-down
	model = glm::rotate(model, glm::radians(180.0f), glm::vec3{ 1.0f, 0.0f, 0.0f });

	model = glm::scale(model, glm::vec3{ width_ *.5, height_ * .5, 1.0f });

	post_processing_shader_.use();
	post_processing_shader_.set_mat4("u_model_", model, false);
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