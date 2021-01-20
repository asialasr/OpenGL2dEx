#ifndef ELEMENT_LIST_H
#define ELEMENT_LIST_H

#include "element.h"
#include "element_pair.h"
#include "label.h"
#include "logging.h"
#include "template_helpers.h"
#include "text_toggle.h"
#include "union.h"

#include <type_traits>

namespace util {

template<typename ...Types>
class ElementUnion : public Element {
public:
	template<typename = std::enable_if_t<and<std::is_base_of<Element, Types>...>::value>,
			 typename = std::enable_if_t<impl::are_types_valid_for_union<Types...>()>>
	ElementUnion()
		: Element{ false }
		, element_{}
		, element_ptr_{ nullptr }
	{
	}

	template<typename ElementType,
		typename = std::enable_if_t<and<std::is_base_of<Element, Types>...>::value>,
		typename = std::enable_if_t<impl::are_types_valid_for_union<Types...>()>>
	ElementUnion(const ElementType &element)
		: Element{ false }
		, element_{ element }
		, element_ptr_{ nullptr }
	{
		element_ptr_ = element_.get_data_ptr();
	}

	ElementUnion(const ElementUnion<Types...> &other)
		: Element{other}
		, element_{ other.element_ }
		, element_ptr_{ nullptr }
	{
		// TODO(sasiala): this is a hacky way to do this
		element_ptr_ = element_.get_data_ptr();
	}

	template<typename ElementType>
	ElementType& get()
	{
		return element_.get<ElementType>();
	}

private:
	void initialize_impl(const glm::mat4 &projection) override
	{
		ASSERT(element_ptr_, "Element pointer uninitialized");

		element_ptr_->initialize(projection);
	}
	void update_impl(Time dt) override
	{
		ASSERT(element_ptr_, "Element pointer uninitialized");

		element_ptr_->update(dt);
	}
	void activate_impl() override
	{
		ASSERT(element_ptr_, "Element pointer uninitialized");

		element_ptr_->activate();
	}
	void deactivate_impl() override
	{
		ASSERT(element_ptr_, "Element pointer uninitialized");

		element_ptr_->deactivate();
	}
	void render_impl(Optional<SpriteRenderer*> parent_sprite_renderer) override
	{
		ASSERT(element_ptr_, "Element pointer uninitialized");

		element_ptr_->render(parent_sprite_renderer);
	}
	void set_key_impl(KeyId key_id, bool val) override
	{
		ASSERT(element_ptr_, "Element pointer uninitialized");

		element_ptr_->set_key(key_id, val);
	}
	void process_input_impl(float dt) override
	{
		ASSERT(element_ptr_, "Element pointer uninitialized");

		element_ptr_->process_input(dt);
	}

	UnionWithDataPtr<Element, Types...> element_;
	Element *element_ptr_;
}; // class ElementUnion

template<typename ELEMENT_TYPE, size_t MAX_OBJECTS,
		 typename = std::enable_if_t<std::is_base_of<Element, ELEMENT_TYPE>::value>>
class ElementList : public Element {
public:
	using ListObject = ELEMENT_TYPE;
	using Index = size_t;
	static constexpr Index kMaxObjects = MAX_OBJECTS;
	using ObjectArray = ListObject[kMaxObjects];

	ElementList()
		: Element{ false }
		, list_{}
		, size_{ 0 }
	{
	}

	ElementList(ObjectArray &list)
		: Element{ false }
		, list_{}
		, size_{kMaxObjects}
	{
		copy(list, list_);
	}

	ListObject& at(const Index index)
	{
		ASSERT(index < kMaxObjects, "Index out of bounds.");
		return list_[index];
	}

	void push_back(const ListObject &obj)
	{
		ASSERT(size_ < kMaxObjects, "No room in list");

		list_[size_++] = obj;
	}

	// TODO(sasiala): should be passed a size to know which elements
	// should be rendered
	Index size() const
	{
		return size_;
	}

	Index max_size() const
	{
		return kMaxObjects;
	}
private:
	// Element
	void initialize_impl(const glm::mat4 &projection) override
	{
		auto lambda = [](Element &e, const glm::mat4 &projection) {
			e.initialize(projection);
		};
		apply_with_count(list_, size_, lambda, projection);
	}
	void update_impl(const Time dt) override
	{
		auto lambda = [](Element &e, const Time dt) {
			e.update(dt);
		};
		apply_with_count(list_, size_, lambda, dt);
	}
	void activate_impl() override
	{
		auto lambda = [](Element &e) {
			e.activate();
		};
		apply_with_count(list_, size_, lambda);
	}
	void deactivate_impl() override
	{
		auto lambda = [](Element &e) {
			e.deactivate();
		};
		apply_with_count(list_, size_, lambda);
	}
	void render_impl(Optional<SpriteRenderer*> parent_sprite_renderer) override
	{
		auto lambda = [](Element &e, const Optional<SpriteRenderer*> &parent_sprite_renderer) {
			e.render(parent_sprite_renderer);
		};
		apply_with_count(list_, size_, lambda, parent_sprite_renderer);
	}
	void set_key_impl(KeyId /*key_id*/, bool /*val*/) override
	{
	}
	void process_input_impl(float /*dt*/) override
	{
	}

	ObjectArray list_;
	Index size_;
}; // class ElementList

} // namespace util

#endif // ELEMENT_LIST_H