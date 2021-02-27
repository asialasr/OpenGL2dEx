#ifndef TEXT_TOGGLE_H
#define TEXT_TOGGLE_H

#include "array_helpers.h"
#include "element.h"
#include "label.h"
#include "logging.h"

namespace util {

template <size_t NUM_OPTIONS>
class TextToggle : public Element
{
public:
	static constexpr auto kNumOptions = NUM_OPTIONS;
	using Color = glm::vec3;
	using Index = size_t;

	TextToggle()
		: Element{ active }
		, selected_option_{}
		, selected_color_{}
		, deselected_color_{}
	{
		for (auto i = size_t{ 0 }; i < count_of(labels_); ++i)
		{
			labels_[i] = Label{};
		}
	}

	TextToggle(const bool         active,
		const float        x_ratio,
		const float        y_ratio,
		const float        width_ratio,
		const float        height_ratio,
		const float        text_scale_ratio,
		const Index        selected_option,
		const char* const  text[kNumOptions],
		const Dimension    viewport_width,
		const Dimension    viewport_height,
		const Color		 &selected_color,
		const Color		 &deselected_color)
		: Element{ active }
		, selected_option_{ selected_option }
		, selected_color_{ selected_color }
		, deselected_color_{ deselected_color }
	{
		for (auto i = size_t{ 0 }; i < count_of(labels_); ++i)
		{
			const auto x = x_ratio + (i * (width_ratio / kNumOptions));
			const auto color = (i == selected_option_) ? selected_color_ : deselected_color_;
			labels_[i] = Label{ active, x, y_ratio, text_scale_ratio, color, text[i], viewport_width, viewport_height };
		}
	}

	TextToggle(const TextToggle<kNumOptions> &other)
		: Element{ other }
		, selected_option_{ other.selected_option_ }
		, selected_color_{ other.selected_color_ }
		, deselected_color_{ other.deselected_color_ }
	{
		copy(other.labels_, labels_);
	}

	TextToggle<kNumOptions> &operator=(const TextToggle<kNumOptions> &other)
	{
		Element::operator=(other);
		selected_option_ = other.selected_option_;
		selected_color_ = other.selected_color_;
		deselected_color_ = other.deselected_color_;
		copy(other.labels_, labels_);
	}

	void selected_index() const
	{
		return selected_option_;
	}

	void toggle()
	{
		ASSERT(selected_option_ < kNumOptions, "Selected option out of bounds");

		labels_[selected_option_].set_color(deselected_color_);

		if (selected_option_ + 1 < kNumOptions)
		{
			++selected_option_;
		}
		else
		{
			selected_option_ = 0;
		}

		labels_[selected_option_].set_color(selected_color_);
	}

	void set_selected(const Index index)
	{
		ASSERT(index < kNumOptions, "Given index out of bounds");

		selected_option_ = index;
	}

private:
	// Element
	void initialize_impl(const glm::mat4 &projection) override
	{
		auto label_initialize = [](Label &label, const glm::mat4 &projection) { label.initialize(projection); };
		apply(labels_, label_initialize, projection);
	}
	void update_impl(Time dt) override
	{
		if (!is_active())
		{
			return;
		}

		apply(labels_, [](Label &label, const Time dt) { label.update(dt); }, dt);
	}
	void activate_impl() override
	{
		apply(labels_, [](Label &label) { label.activate(); });
	}
	void deactivate_impl() override
	{
		apply(labels_, [](Label &label) { label.deactivate(); });
	}
	void render_impl(Optional<SpriteRenderer*> parent_sprite_renderer) override
	{
		if (!is_active())
		{
			return;
		}

		auto label_render = [](Label &label, const Optional<SpriteRenderer*> &parent_sprite_renderer) { label.render(parent_sprite_renderer); };
		apply(labels_, label_render, parent_sprite_renderer);
	}
	void set_key_impl(KeyId key_id, bool val) override
	{
		if (!is_active())
		{
			return;
		}

		auto function = [](Label &label, KeyId key_id, bool val) { label.set_key(key_id, val); };
		apply(labels_, function, key_id, val);
	}
	void process_input_impl(Time dt) override
	{
		if (!is_active())
		{
			return;
		}

		auto label_process_input = [](Label &label, float dt) { label.process_input(dt); };
		apply(labels_, label_process_input, dt);
	}

	Label labels_[kNumOptions];
	Index selected_option_;

	const Color selected_color_;
	const Color deselected_color_;
};

} // namespace util

#endif // TEXT_TOGGLE_H