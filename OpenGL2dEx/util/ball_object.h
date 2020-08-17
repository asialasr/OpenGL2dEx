#ifndef BALL_OBJECT_H
#define BALL_OBJECT_H

#include "game_object.h"

namespace util {

class BallObject : public GameObject {
public:
	BallObject();
	BallObject(const glm::vec2 &pos, 
			   float           radius, 
			   const glm::vec2 &velocity, 
			   const Texture2D &sprite);

	glm::vec2 move(float dt, unsigned int window_width);
	void      reset(const glm::vec2 &position, 
					const glm::vec2 &velocity);

	float radius() const
	{
		return radius_;
	}

	bool stuck() const
	{
		return stuck_;
	}

	void set_stuck(const bool stuck)
	{
		stuck_ = stuck;
	}

	bool sticky() const
	{
		return sticky_;
	}

	void set_sticky(const bool sticky)
	{
		sticky_ = sticky;
	}

	bool pass_through() const
	{
		return pass_through_;
	}

	void set_pass_through(const bool pass_through)
	{
		pass_through_ = pass_through;
	}

private:
	float radius_;
	bool  stuck_;

	bool sticky_;
	bool pass_through_;
}; // class BallObject

} // namespace util

#endif // !BALL_OBJECT_H
