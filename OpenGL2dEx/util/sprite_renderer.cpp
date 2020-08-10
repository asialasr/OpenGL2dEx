#include "sprite_renderer.h"
#include "texture_2d.h"

#include "gl_debug.h"

namespace util {

SpriteRenderer::SpriteRenderer(const Shader &shader)
	: shader_{shader}
{
	init_render_data();
}

SpriteRenderer::~SpriteRenderer()
{
	glDeleteVertexArrays(1, &quad_vao_);
}

void SpriteRenderer::draw(const Texture2D &texture, glm::vec2 position,
						  glm::vec2 size, float rotate, glm::vec3 color)
{
	shader_.use();
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(position, 0.0f));

	model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f));
	model = glm::rotate(model, glm::radians(rotate), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f));

	model = glm::scale(model, glm::vec3(size, 1.0f));

	shader_.set_mat4("u_model_", model, false);
	shader_.set_vec3("u_sprite_color_", color, false);

	glActiveTexture(GL_TEXTURE0);
	texture.bind();

	glBindVertexArray(quad_vao_);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

void SpriteRenderer::init_render_data()
{
	// configure vao/vbo
	unsigned int vbo;
	float vertices[] = {
		// pos      // tex
		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,

		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f
	};

	glGenVertexArrays(1, &quad_vao_);
	glGenBuffers(1, &vbo);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	
	glBindVertexArray(quad_vao_);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	check_for_gl_errors();
}

} // namespace util