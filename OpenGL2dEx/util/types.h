#ifndef TYPES_H
#define TYPES_H

#include "optional.h"

#include <utility>

namespace util {

	using Time = float;
	using Dimension = unsigned int;
	using Distance = int;
	using Pos2D = std::pair<Distance, Distance>;

	using EventType = unsigned int;
	using KeyId = unsigned int;
	struct InputEvent {
		KeyId key_id_;
		EventType event_type_;

		// mouse events
		Optional<Pos2D> screen_position_;
	}; // struct InputEvent

} // namespace util

#endif // TYPES_H