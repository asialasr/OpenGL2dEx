#ifndef ELEMENT_H
#define ELEMENT_H

#include "types.h"

#include "array_helpers.h"
#include "sprite_renderer.h"

namespace util {
	// TODO(sasiala): add is_active_ to element class and a getter function
	// for the classes which inherit from this.  Can set it in activate &deactivate
class Element {
public:
	Element(const bool is_active)
		: is_active_{is_active}
		, default_active_state_{is_active}
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
		is_active_ = default_active_state_;
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
	virtual void process_input_impl(Time dt) = 0;

	bool is_active_;
	bool default_active_state_;
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

// commonly used lambdas to perform operations on array of elements/element pointers
namespace ElementLambdas {
	static auto initialize = [](Element &e, const glm::mat4 &projection) { e.initialize(projection); };
	static auto initialize_pointer = [](Element *e, const glm::mat4 &projection) { e->initialize(projection); };
	static auto update = [](Element &e, const Time dt) { e.update(dt); };
	static auto update_pointer = [](Element *e, const Time dt) { e->update(dt); };
	static auto activate = [](Element &e) { e.activate(); };
	static auto activate_pointer = [](Element *e) { e->activate(); };
	static auto deactivate = [](Element &e) { e.deactivate(); };
	static auto deactivate_pointer = [](Element *e) { e->deactivate(); };
	static auto render = [](Element &e, Optional<SpriteRenderer*> parent_sprite_renderer) { e.render(parent_sprite_renderer); };
	static auto render_pointer = [](Element *e, Optional<SpriteRenderer*> parent_sprite_renderer) { e->render(parent_sprite_renderer); };
	static auto set_key = [](Element &e, const KeyId key_id, const bool val) { e.set_key(key_id, val); };
	static auto set_key_pointer = [](Element *e, const KeyId key_id, const bool val) { e->set_key(key_id, val); };
	static auto process_input = [](Element &e, const Time dt) { e.process_input(dt); };
	static auto process_input_pointer = [](Element *e, const Time dt) { e->process_input(dt); };
}

} // namespace util

#endif // ELEMENT_H