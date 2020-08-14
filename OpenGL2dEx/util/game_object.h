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

	GameObject(const GameObject &other)
		: position_{ other.position_ }
		, size_{ other.size_ }
		, velocity_{ other.velocity_ }
		, color_{ other.color_ }
		, rotation_{ other.rotation_ }
		, is_solid_{ other.is_solid_ }
		, destroyed_{ other.destroyed_ }
		, sprite_{ other.sprite_ }
	{
	}

	GameObject& operator=(const GameObject &other)
	{
		position_ = other.position_;
		size_ = other.size_;
		velocity_ = other.velocity_;
		color_ = other.color_;
		rotation_ = other.rotation_;
		is_solid_ = other.is_solid_;
		destroyed_ = other.destroyed_;
		sprite_ = other.sprite_;
		return *this;
	}

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

	const glm::vec2& position() const
	{
		return position_;
	}

	void progress_time(float dt)
	{
		position_ += velocity_ * dt;
	}

	void move_x(float dx)
	{
		position_.x += dx;
	}

	void move_y(float dy)
	{
		position_.y += dy;
	}

	void set_position(const glm::vec2 &position)
	{
		position_ = position;
	}

	void set_pos_x(float x)
	{
		position_.x = x;
	}

	void set_pos_y(float y)
	{
		position_.y = y;
	}

	const glm::vec2& velocity() const
	{
		return velocity_;
	}

	void set_velocity(const glm::vec2 &velocity)
	{
		velocity_ = velocity;
	}

	const glm::vec2& size() const
	{
		return size_;
	}

	void set_size(const glm::vec2 &size)
	{
		size_ = size;
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