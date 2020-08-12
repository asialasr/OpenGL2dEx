#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include "optional.h"
#include "sprite_renderer.h"
#include "texture_2d.h"

#include <glad/glad.h>
#include <glm/glm.hpp>

namespace util {

class GameObject {
public:
	GameObject();

	const glm::vec3 kDefaultColor{ 1.0f };
	const glm ::vec2 kDefaultVelocity{ 0.0f, 0.0f };
	GameObject(glm::vec2           position, 
			   glm::vec2           size, 
			   Texture2D           sprite,
			   Optional<glm::vec3> color, 
			   Optional<glm::vec2> velocity);

	virtual void draw(SpriteRenderer &renderer);

	bool is_solid() const
	{
		return is_solid_;
	}

	void set_solid(bool is_solid)
	{
		is_solid_ = is_solid;
	}

	bool is_destroyed() const
	{
		return destroyed_;
	}

	void set_destroyed(bool destroyed)
	{
		destroyed_ = destroyed;
	}

private:
	// object state
	glm::vec2 position_;
	glm::vec2 size_;
	glm::vec2 velocity_;
	glm::vec3 color_;
	float     rotation_;
	bool      is_solid_;
	bool      destroyed_;

	// render state
	Texture2D sprite_;
}; // class GameObject

} // namespace util

#endif // GAME_OBJECT_H