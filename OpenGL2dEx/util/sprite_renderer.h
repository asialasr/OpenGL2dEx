#ifndef SPRITE_RENDERER_H
#define SPRITE_RENDERER_H

#include "shader.h"
#include <glm/glm.hpp>

namespace util {

class Texture2D;

class SpriteRenderer {
public:
	SpriteRenderer(const Shader &shader);
	~SpriteRenderer();

	void draw(const Texture2D &texture, glm::vec2 position, 
			  glm::vec2 size = glm::vec2(10.0f, 10.0f), float rotate = 0.0f,
		      glm::vec3 color = glm::vec3(1.0f));

private:
	Shader		 shader_;
	unsigned int quad_vao_;

	void init_render_data();
};

} // namespace util

#endif // SPRITE_RENDERER_H