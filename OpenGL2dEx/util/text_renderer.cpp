#include "text_renderer.h"

#include "logging.h"

#include <utility>

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H

namespace util
{

namespace {
auto make_ortho(const TextRenderer::Dimension width,
				const TextRenderer::Dimension height)
{
	return glm::ortho(0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f);
}
}

TextRenderer::TextRenderer(const Shader &shader, const Dimension width, const Dimension height)
{
	shader_ = &shader;

	shader_->use();
	shader_->set_mat4("u_projection_", make_ortho(width, height), false);
	shader_->set_int("u_text_", 0, false);

	// TODO(sasiala): I think I could make some utility functions which would
	// make this simpler/clearer across all of the files
	glGenVertexArrays(1, &vao_);
	glGenBuffers(1, &vbo_);
	glBindVertexArray(vao_);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void TextRenderer::load(const char *font_path, 
						FontSize font_size)
{
	character_map_.clear();

	// initialize/load freetype library
	FT_Library ft;
	if (FT_Init_FreeType(&ft))
	{
		ASSERT(false, "FREETYPE: Could not init FreeType library");
	}

	FT_Face face;
	if (FT_New_Face(ft, font_path, 0, &face))
	{
		ASSERT(false, "FREETYPE: Failed to load font");
	}

	// set size to load glyphs
	FT_Set_Pixel_Sizes(face, 0, font_size);
	// disable byte alignment restriction
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	for (auto c = GLubyte{ 0 }; c < 128; ++c)
	{
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			LOG("FREETYPE: Failed to load glyph");
			continue;
		}

		unsigned int texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			face->glyph->bitmap.width,
			face->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			face->glyph->bitmap.buffer
		);
		// set texture options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		auto character = Character{
			texture,
			glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			static_cast<unsigned int>(face->glyph->advance.x)
		};
		character_map_.insert(std::make_pair(static_cast<char>(c), character));
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	FT_Done_Face(face);
	FT_Done_FreeType(ft);
}

void TextRenderer::render_text(const std::string &text, 
							   const float x, 
							   const float y, 
							   const float scale, 
							   const util::Optional<glm::vec3> &color) const
{
	ASSERT(shader_, "Text shader not valid");

	shader_->use();
	if (color)
	{
		shader_->set_vec3("u_text_color_", *color, false);
	}
	else
	{
		shader_->set_vec3("u_text_color_", kDefaultColor, false);
	}
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(vao_);

	auto next_x = x;
	for (auto c = text.cbegin(); c != text.cend(); ++c)
	{
		// TODO(sasiala): should we allow unknown characters and display something in
		// particular?  Perhaps a box?
		ASSERT(character_map_.find(*c) != character_map_.end(), "Unknown character");
		auto ch = character_map_.at(*c);

		float xpos = next_x + ch.bearing_.x * scale;
		// TODO(sasiala): is the H used as a constant?  Find a better way or move into its own function
		float ypos = y + (character_map_.at('H').bearing_.y - ch.bearing_.y) * scale;

		float w = ch.size_.x * scale;
		float h = ch.size_.y * scale;

		float vertices[6][4] = {
			{ xpos,     ypos + h,    0.0f, 1.0f },
			{ xpos + w, ypos,        1.0f, 0.0f },
			{ xpos,     ypos,        0.0f, 0.0f },
			
			{ xpos,     ypos + h,    0.0f, 1.0f },
			{ xpos + w, ypos + h,    1.0f, 1.0f },
			{ xpos + w, ypos,        1.0f, 0.0f }
		};
		// render glyph texture over quad
		glBindTexture(GL_TEXTURE_2D, ch.texture_id_);
		// update content of VBO memory
		glBindBuffer(GL_ARRAY_BUFFER, vbo_);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// render quad
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// advance cursors for next glyph
		// bitshift by 6 to get value in pixels (1/64gh times 2^6 = 64)
		next_x += (ch.advance_ >> 6) * scale;
	}

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}
}