#ifndef POWER_UP_H
#define POWER_UP_H

#include "game_object.h"

#include <glm/glm.hpp>

namespace util {

class PowerUp : public GameObject
{
public:
	// TODO(sasiala): use constants in constructor, but these don't get initialized early enough..?
	const glm::vec2 kSize = glm::vec2( 60.0f, 20.0f );
	const glm::vec2 kVelocity = glm::vec2( 0.0f, 150.0f );
	typedef unsigned int Type;

	PowerUp(const Type type, const glm::vec3 &color, const float duration,
		const glm::vec2 &position, const Texture2D texture)
		: GameObject(position, glm::vec2(60.0f, 20.0f), texture, color , glm::vec2(0.0f, 150.0f))
		, type_{ type }
		, duration_{ duration }
		, activated_{ false }
	{
	}

	PowerUp(const PowerUp &other)
		: GameObject{ other }
		, type_{ other.type_ }
		, duration_{ other.duration_ }
		, activated_{ other.activated_ }
	{
	}

	util::PowerUp &operator=(const util::PowerUp &rhs)
	{
		GameObject::operator=(rhs);
		type_ = rhs.type_;
		duration_ = rhs.duration_;
		activated_ = rhs.activated_;
		return *this;
	}

	Type type() const
	{
		return type_;
	}

	void set_type(const Type type)
	{
		type_ = type;
	}

	float duration() const
	{
		return duration_;
	}

	void set_duration(const float duration)
	{
		duration_ = duration;
	}

	bool activated() const
	{
		return activated_;
	}

	void set_activated(const bool activated)
	{
		activated_ = activated;
	}

private:
	Type  type_;
	float duration_;
	bool  activated_;
};

} // namespace util

#endif // POWER_UP_H