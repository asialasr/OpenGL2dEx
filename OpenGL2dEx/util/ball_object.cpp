#include "ball_object.h"

namespace util {

BallObject::BallObject()
	: radius_{ 1.0f }
	, stuck_{ true }
{
}

BallObject::BallObject(const glm::vec2 &position,
					   const float     radius,
					   const glm::vec2 &velocity,
					   const Texture2D &sprite)
	: GameObject{ position, glm::vec2{radius * 2.0f, radius * 2.0f}, sprite, {}, {velocity} }
	, radius_{ radius }
	, stuck_{ true }
	, sticky_{ false }
	, pass_through_{ false }
{
}

glm::vec2 BallObject::move(const float dt, const unsigned int window_width)
{
	if (!stuck_)
	{
		progress_time(dt);

		if (position().x <= 0.0f)
		{
			auto temp_velocity = velocity();
			temp_velocity.x = -temp_velocity.x;
			set_velocity(temp_velocity);
			set_pos_x(0.0f);
		}
		else if (position().x + size().x >= window_width)
		{
			auto temp_velocity = velocity();
			temp_velocity.x = -temp_velocity.x;
			set_velocity(temp_velocity);
			set_pos_x(window_width - size().x);
		}

		if (position().y <= 0.0f)
		{
			auto temp_velocity = velocity();
			temp_velocity.y = -temp_velocity.y;
			set_velocity(temp_velocity);
			set_pos_y(0.0f);
		}
	}

	return position();
}

void BallObject::reset(const glm::vec2 &position,
					   const glm::vec2 &velocity)
{
	set_position(position);
	set_velocity(velocity);
	stuck_ = true;
	sticky_ = false;
	pass_through_ = false;
}

} // namespace util