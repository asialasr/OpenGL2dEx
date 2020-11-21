#ifndef ELEMENT_PAIR_H
#define ELEMENT_PAIR_H

#include "element.h"

#include <type_traits>

namespace util {

// TODO(sasiala): element tuple would likely be better
template <typename T1, 
		  typename T2, 
		  typename = std::enable_if_t<and<std::is_base_of<Element, T1>, std::is_base_of<Element, T2>>::value>>
class ElementPair : public Element {
public:
	using Type1 = T1;
	using Type2 = T2;

	ElementPair()
		: one_{}
		, two_{}
	{
	}

	ElementPair(const Type1 &one, const Type2 &two)
		: one_{ one }
		, two_{ two }
	{
	}

	Type1 &one() const
	{
		return one_;
	}

	Type2 &two() const
	{
		return two_;
	}

private:
	// Element
	void initialize_impl(const glm::mat4 &projection) override
	{
		one_.initialize(projection);
		two_.initialize(projection);
	}
	void update_impl(const Time dt) override
	{
		one_.update(dt);
		two_.update(dt);
	}
	void activate_impl() override
	{
		one_.activate();
		two_.activate();
	}
	void deactivate_impl() override
	{
		one_.deactivate();
		two_.deactivate();
	}
	void render_impl(const Optional<SpriteRenderer*> parent_sprite_renderer) override
	{
		one_.render(parent_sprite_renderer);
		two_.render(parent_sprite_renderer);
	}
	void set_key_impl(const KeyId key_id, const bool val) override
	{
		one_.set_key(key_id, val);
		two_.set_key(key_id, val);
	}
	void process_input_impl(const float dt) override
	{
		one_.process_input(dt);
		two_.process_input(dt);
	}

	Type1 one_;
	Type2 two_;
}; // class ElementPair

} // namespace util

#endif // ELEMENT_PAIR_H