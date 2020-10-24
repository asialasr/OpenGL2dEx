#ifndef UTIL_LABEL_H
#define UTIL_LABEL_H

#include "resource_mgr.h"

#include <glm/glm.hpp>

namespace util {

class Label {
public:
	Label(const float      x_ratio,
		  const float      y_ratio,
		  const float      scale_ratio,
		  const glm::vec3 &color,
		  const std::string &text )
		: x_ratio_{x_ratio}
		, y_ratio_{y_ratio}
		, scale_ratio_{scale_ratio}
		, color_{color}
		, font_id_{}
		, text_{text}
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

	void render(const Dimension viewport_width, const Dimension viewport_height)
	{
		const auto &text_renderer = ResourceManager::get_font(font_id_);
		text_renderer.update_size(viewport_width, viewport_height);
		text_renderer.render_text(text_, x(viewport_width), y(viewport_height), scale(viewport_height), color_);
	}

private:
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
};

} // namespace util

#endif // UTIL_LABEL_H