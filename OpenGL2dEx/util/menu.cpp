#include "menu.h"

#include "logging.h"
#include "array_helpers.h"

#include "GLFW/glfw3.h"

namespace util {

Menu::Menu(Dimension load_width, Dimension load_height)
	: title_{ "" }
	, selected_item_{}
	, option_list_{}
	, loaded_width_{ load_width }
	, loaded_height_{ load_height }
	, font_shader_id_{}
	, default_font_id_{}
	, menu_button_handler_{ nullptr }
	, keys_pressed_{}
	, keys_processed_{}
	, render_back_button_{ false }
{
}

void Menu::set_menu_handler(MenuButtonHandler &handler)
{
	menu_button_handler_ = &handler;
}

void Menu::update_info(const std::string &title,
	                   const std::string &subtitle,
	                   const bool         show_back_label,
	                   const OptionList  &options,
	                   const OptionIndex  selected_item)
{
	title_ = title;
	subtitle_label_.set_text(subtitle);
	render_back_button_ = show_back_label;
	selected_item_ = selected_item;
	option_list_ = options;
}

void Menu::initialize_impl(const glm::mat4 &projection)
{
	font_shader_id_ = ResourceManager::load_shader("shaders/text_2d.vs", "shaders/text_2d.fs", { util::nullopt });

	// text
	default_font_id_ = ResourceManager::load_font(kDefaultFontPath, font_shader_id_, kDefaultFontSize, loaded_width_, loaded_height_);
	
	// label fonts
	kTitleText.set_font(default_font_id_);
	subtitle_label_.set_font(default_font_id_);
	kBackText.set_font(default_font_id_);
}

void Menu::update_impl(Time /*dt*/)
{
}

void Menu::activate_impl()
{
	fill(keys_pressed_, false);
	fill(keys_processed_, false);
}

void Menu::deactivate_impl()
{
	title_.clear();
	option_list_.clear();
}

void Menu::render_impl(Optional<SpriteRenderer*> /*parent_sprite_renderer*/)
{
	render_title();

	// TOOD(sasiala): center text for subtitle
	subtitle_label_.render(loaded_width_, loaded_height_);

	if (render_back_button_)
	{
		kBackText.render(loaded_width_, loaded_height_);
	}

	render_options();
}

void Menu::set_key_impl(KeyId key_id, bool val)
{
	if (!is_active())
	{
		return;
	}

	bool *key_ptr = nullptr;
	bool *key_processed_ptr = nullptr;
	switch (convert_id(key_id))
	{
	case ButtonsHandled::kUpButton:
	case ButtonsHandled::kDownButton:
	case ButtonsHandled::kEnterButton:
	case ButtonsHandled::kBackButton:
		key_ptr = &keys_pressed_[to_index(convert_id(key_id))];
		key_processed_ptr = &keys_processed_[to_index(convert_id(key_id))];
		break;
	default:
		LOG("Unhandled key ID");
		return;
	}

	// update keys & processed arrays accordingly
	*key_ptr = val;
	if (!val)
	{
		*key_processed_ptr = false;
	}
}

void Menu::process_input_impl(float dt)
{
	if (!is_active())
	{
		return;
	}

	if (keys_pressed_[to_index(ButtonsHandled::kEnterButton)] 
		&& !keys_processed_[to_index(ButtonsHandled::kEnterButton)])
	{
		menu_button_handler_->handle_menu_option_acceptance(selected_item_);
		keys_processed_[to_index(ButtonsHandled::kEnterButton)] = true;
	}
	else if (keys_pressed_[to_index(ButtonsHandled::kBackButton)]
		&& !keys_processed_[to_index(ButtonsHandled::kBackButton)])
	{
		if (render_back_button_)
		{
			menu_button_handler_->handle_back_button();
		}
		keys_processed_[to_index(ButtonsHandled::kBackButton)] = true;
	}
	else
	{
		if (keys_pressed_[to_index(ButtonsHandled::kDownButton)] 
			&& !keys_processed_[to_index(ButtonsHandled::kDownButton)])
		{
			const auto number_of_items = option_list_.size();
			selected_item_ = (selected_item_ + 1) % number_of_items;

			menu_button_handler_->handle_menu_option_highlight(selected_item_);
			keys_processed_[to_index(ButtonsHandled::kDownButton)] = true;
		}
		if (keys_pressed_[to_index(ButtonsHandled::kUpButton)] 
			&& !keys_processed_[to_index(ButtonsHandled::kUpButton)])
		{
			const auto number_of_items = option_list_.size();
			if (selected_item_ > 0)
			{
				--selected_item_;
			}
			else
			{
				// wrap around
				selected_item_ = number_of_items - 1;
			}

			menu_button_handler_->handle_menu_option_highlight(selected_item_);
			keys_processed_[to_index(ButtonsHandled::kUpButton)] = true;
		}
	}
}

void Menu::render_title()
{
	// TOOD(sasiala): center text
	const auto &text_renderer = ResourceManager::get_font(default_font_id_);
	kTitleText.render(loaded_width_, loaded_height_);
}

void Menu::render_options()
{
	// TODO(sasiala): consider adding scrolling to list
	ASSERT(option_list_.size() < kMenuList.kMaxRows, "Too many rows for menu list");

	const auto &text_renderer = ResourceManager::get_font(default_font_id_);
	auto pos_y_ratio = kMenuList.kYTopRatio;
	Label option_label{ kMenuList.kXRatio, kMenuList.kYTopRatio, kMenuList.kTextScaleFromHeight, kMenuList.kDeselectedColor, "" };
	for (auto i = size_t{ 0 }; i < option_list_.size(); ++i)
	{
		const auto color = (i == selected_item_) ? kMenuList.kSelectedColor : kMenuList.kDeselectedColor;
		option_label.set_text(option_list_.at(i));
		option_label.set_y_ratio(pos_y_ratio);
		option_label.set_color(color);
		option_label.render(loaded_width_, loaded_height_);

		pos_y_ratio += kMenuList.kRowHeightRatio;
	}
}

} // namespace util