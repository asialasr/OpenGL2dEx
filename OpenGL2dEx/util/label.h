#ifndef UTIL_LABEL_H
#define UTIL_LABEL_H

#include "element.h"
#include "resource_mgr.h"

#include <glm/glm.hpp>

namespace util {

// TODO(sasiala): shouldn't the label be an element?
class Label : public Element {
public:
	Label(const bool         initially_active,
		  const float        x_ratio,
		  const float        y_ratio,
		  const float        scale_ratio,
		  const glm::vec3   &color,
		  const std::string &text,
		  const Dimension    viewport_width,
		  const Dimension    viewport_height)
		: Element{initially_active}
		, x_ratio_ { x_ratio }
		, y_ratio_{y_ratio}
		, scale_ratio_{scale_ratio}
		, color_{color}
		, font_id_{}
		, text_{text}
		, viewport_width_{ viewport_width }
		, viewport_height_{ viewport_height }
	{
	}

	void set_font(const ResourceManager::FontId font_id)
	{
		font_id_ = font_id;
	}

	void set_x_ratio(const float x_ratio)
	{
		x_ratio_ = x_ratio;
	}

	void set_y_ratio(const float y_ratio)
	{
		y_ratio_ = y_ratio;
	}

	void sey_scale_ratio(const float scale_ratio)
	{
		scale_ratio_ = scale_ratio;
	}

	void set_color(const glm::vec3 &color)
	{
		color_ = color;
	}

	void set_text(const std::string &text)
	{
		text_ = text;
	}

	void set_viewport_size(const Dimension viewport_width, 
		                   const Dimension viewport_height)
	{
		viewport_width_ = viewport_width;
		viewport_height_ = viewport_height;
	}

private:
	// Element
	void initialize_impl(const glm::mat4 &/*projection*/) override
	{
	}
	void update_impl(Time /*dt*/) override
	{
	}
	void activate_impl() override
	{
	}
	void deactivate_impl() override
	{
	}
	void render_impl(Optional<SpriteRenderer*> parent_sprite_renderer) override
	{
		if (!is_active())
		{
			return;
		}

		const auto &text_renderer = ResourceManager::get_font(font_id_);
		text_renderer.update_size(viewport_width_, viewport_height_);
		text_renderer.render_text(text_, x(viewport_width_), y(viewport_height_), scale(viewport_height_), color_);
	}
	void set_key_impl(KeyId /*key_id*/, bool /*val*/) override
	{
	}
	void process_input_impl(float /*dt*/) override
	{
	}

	float x(const Dimension width) const
	{
		return x_ratio_ * width;
	}

	float y(const Dimension height) const
	{
		return y_ratio_ * height;
	}

	float scale(const Dimension height) const
	{
		return scale_ratio_ * height;
	}

	float x_ratio_;
	float y_ratio_;
	float scale_ratio_;
	glm::vec3 color_;

	ResourceManager::FontId font_id_;

	std::string text_;

	Dimension viewport_width_;
	Dimension viewport_height_;
};

} // namespace util

#endif // UTIL_LABEL_H