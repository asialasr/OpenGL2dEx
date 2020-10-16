#ifndef TEXT_RENDERER_H
#define TEXT_RENDERER_H

#include "optional.h"
#include "shader.h"

#include <map>
#include <string>
#include <glm/glm.hpp>

namespace util
{

class TextRenderer
{
public:
	using Dimension = unsigned int;
	using FontSize = unsigned int;

	struct Character {
		unsigned int texture_id_;
		glm::ivec2   size_;
		glm::ivec2   bearing_;
		unsigned int advance_;
	};

private:
	using CharacterMap = std::map<char, Character>;

public:
	TextRenderer()
		: shader_{ nullptr }
		, character_map_{}
		, vao_{}
		, vbo_{}
	{
	}

	TextRenderer(const Shader &shader, Dimension width, Dimension height);
	TextRenderer(const TextRenderer &other)
	{
		*this = other;
	}

	TextRenderer& operator=(const TextRenderer &other)
	{
		shader_ = other.shader_;
		character_map_ = other.character_map_;
		vao_ = other.vao_;
		vbo_ = other.vbo_;

		return *this;
	}

	void load(const char *font_path, FontSize font_size);
	void update_size(float width, float height) const;
	void render_text(const std::string &text, 
					 float x, 
					 float y, 
					 float scale, 
					 const util::Optional<glm::vec3> &color) const;

private:
	const Shader *shader_;
	CharacterMap character_map_;

	unsigned int vao_, vbo_;

	const glm::vec3 kDefaultColor = glm::vec3{ 0.0f, 0.0f, 0.0f };
};

} // namespace util

#endif // TEXT_RENDERER_H