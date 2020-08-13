#include "game_object.h"

namespace util {

	GameObject::GameObject()
		: position_{0.0f, 0.0f}
		, size_{1.0f, 1.0f}
		, velocity_{0.0f}
		, color_{1.0f, 1.0f, 1.0f}
		, rotation_{0.0f}
		, is_solid_{true}
		, destroyed_{false}
		, sprite_{}
	{
	}

	GameObject::GameObject(glm::vec2           position, 
						   glm::vec2           size, 
						   Texture2D           sprite,
						   Optional<glm::vec3> color, 
						   Optional<glm::vec2> velocity)
		: position_{position}
		, size_{size}
		, velocity_{ (velocity) ? *velocity : glm::vec2{} }
		, color_{ (color) ? *color : glm::vec3{} }
		, rotation_{}
		, is_solid_{}
		, destroyed_{}
		, sprite_{sprite}
	{
	}

	void GameObject::draw(SpriteRenderer &renderer)
	{
		renderer.draw(sprite_, position_, size_, rotation_, color_);
	}

} // namespace util