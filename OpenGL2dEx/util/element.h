#ifndef ELEMENT_H
#define ELEMENT_H

#include "types.h"

#include "sprite_renderer.h"

namespace util {
	// TODO(sasiala): add is_active_ to element class and a getter function
	// for the classes which inherit from this.  Can set it in activate &deactivate
class Element {
public:
	Element(const bool is_active)
		: is_active_{is_active}
	{
	}

	virtual ~Element()
	{
	}

	// TODO(sasiala): I'm not sure initialize needs to be a part of Element and could
	// rather be a part of the child classes
	void initialize(const glm::mat4 &projection)
	{
		initialize_impl(projection);
		is_active_ = false;
	}

	void update(const Time dt)
	{
		update_impl(dt);
	}

	void activate()
	{
		activate_impl();
		is_active_ = true;
	}

	void deactivate()
	{
		deactivate_impl();
		is_active_ = false;
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

	bool is_active() const
	{
		return is_active_;
	}

private:
	virtual void initialize_impl(const glm::mat4 &projection) = 0;
	virtual void update_impl(Time dt) = 0;
	virtual void activate_impl() = 0;
	virtual void deactivate_impl() = 0;
	virtual void render_impl(Optional<SpriteRenderer*> parent_sprite_renderer) = 0;
	virtual void set_key_impl(KeyId key_id, bool val) = 0;
	// TODO(sasiala): improve event handling
	virtual void process_input_impl(float dt) = 0;

	bool is_active_;
}; // class Element

inline void conditionally_activate(Element &e, bool activate)
{
	if (activate)
	{
		e.activate();
	}
	else
	{
		e.deactivate();
	}
}

} // namespace util

#endif // ELEMENT_H