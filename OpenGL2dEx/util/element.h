#ifndef ELEMENT_H
#define ELEMENT_H

#include "types.h"

#include "sprite_renderer.h"

namespace util {

class Element {
public:
	Element() = default;
	virtual ~Element()
	{
	}

	void initialize(const glm::mat4 &projection)
	{
		initialize_impl(projection);
	}

	void update(const Time dt)
	{
		update_impl(dt);
	}

	// TODO(sasiala): when optional is improved, swith back to 
	// Optional<SpriteRenderer&>
	void render(const Optional<SpriteRenderer*> parent_sprite_renderer)
	{
		render_impl(parent_sprite_renderer);
	}

	void set_key(const KeyId key_id, const bool val)
	{
		set_key_impl(key_id, val);
	}

	void process_input(float dt)
	{
		process_input_impl(dt);
	}

private:
	virtual void initialize_impl(const glm::mat4 &projection) = 0;
	virtual void update_impl(Time dt) = 0;
	virtual void render_impl(Optional<SpriteRenderer*> parent_sprite_renderer) = 0;
	virtual void set_key_impl(KeyId key_id, bool val) = 0;
	// TODO(sasiala): improve event handling
	virtual void process_input_impl(float dt) = 0;
}; // class Element

} // namespace util

#endif // ELEMENT_H