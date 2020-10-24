#include "menu.h"

#include "logging.h"

#include "GLFW/glfw3.h"

namespace util {

Menu::Menu(Dimension load_width, Dimension load_height)
	: loaded_width_{ load_width }
	, loaded_height_{ load_height }
	, font_shader_id_{}
	, default_font_id_{}
	, menu_stack_{}
	, menu_button_handler_{ nullptr }
	, keys_pressed_{}
	, keys_processed_{}
{
}

void Menu::set_menu_handler(MenuButtonHandler &handler)
{
	menu_button_handler_ = &handler;
}

void Menu::activate(const std::string &title, const std::string &subtitle, OptionList &options, const OptionIndex selected_item)
{
	deactivate();

	title_ = title;
	menu_stack_.push_back({ subtitle, selected_item, options });
}

void Menu::open_sub_menu(const std::string &subtitle, OptionList &options, const OptionIndex selected_item)
{
	menu_stack_.push_back({ subtitle, selected_item, options });
}

void Menu::deactivate()
{
	title_.clear();
	menu_stack_.clear();
}

void Menu::initialize_impl(const glm::mat4 &projection)
{
	font_shader_id_ = ResourceManager::load_shader("shaders/text_2d.vs", "shaders/text_2d.fs", { util::nullopt });

	// text
	default_font_id_ = ResourceManager::load_font(kDefaultFontPath, font_shader_id_, kDefaultFontSize, loaded_width_, loaded_height_);
	
	// label fonts
	kTitleText.set_font(default_font_id_);
	subtitle_.set_font(default_font_id_);
	kBackText.set_font(default_font_id_);
}

void Menu::update_impl(Time /*dt*/)
{
}

void Menu::render_impl(Optional<SpriteRenderer*> /*parent_sprite_renderer*/)
{
	if (!menu_stack_.empty())
	{
		render_title();
		render_submenu();
	}
}

void Menu::set_key_impl(KeyId key_id, bool val)
{
	if (menu_stack_.empty())
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
	if (menu_stack_.empty())
	{
		return;
	}

	if (keys_pressed_[to_index(ButtonsHandled::kEnterButton)] 
		&& !keys_processed_[to_index(ButtonsHandled::kEnterButton)])
	{
		const auto index = menu_stack_.back().selected_item_;
		const auto level = menu_stack_.size() - 1;
		menu_button_handler_->handle_menu_option_acceptance(index, level);
		keys_processed_[to_index(ButtonsHandled::kEnterButton)] = true;
	}
	else if (keys_pressed_[to_index(ButtonsHandled::kBackButton)]
		&& !keys_processed_[to_index(ButtonsHandled::kBackButton)])
	{
		const auto level = menu_stack_.size() - 1;
		if (level > 0)
		{
			menu_stack_.pop_back();
			menu_button_handler_->handle_submenu_dismiss();
		}
		keys_processed_[to_index(ButtonsHandled::kBackButton)] = true;
	}
	else
	{
		if (keys_pressed_[to_index(ButtonsHandled::kDownButton)] 
			&& !keys_processed_[to_index(ButtonsHandled::kDownButton)])
		{
			auto &selected_item = menu_stack_.back().selected_item_;
			const auto number_of_items = menu_stack_.back().option_list_.size();
			selected_item = (selected_item + 1) % number_of_items;

			const auto level = menu_stack_.size() - 1;
			menu_button_handler_->handle_menu_option_highlight(selected_item, level);
			keys_processed_[to_index(ButtonsHandled::kDownButton)] = true;
		}
		if (keys_pressed_[to_index(ButtonsHandled::kUpButton)] 
			&& !keys_processed_[to_index(ButtonsHandled::kUpButton)])
		{
			auto &selected_item = menu_stack_.back().selected_item_;
			const auto number_of_items = menu_stack_.back().option_list_.size();
			if (selected_item > 0)
			{
				--selected_item;
			}
			else
			{
				// wrap around
				selected_item = number_of_items - 1;
			}

			const auto level = menu_stack_.size() - 1;
			menu_button_handler_->handle_menu_option_highlight(selected_item, level);
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

void Menu::render_options(const OptionList &options, const OptionIndex selected_item)
{
	// TODO(sasiala): consider adding scrolling to list
	ASSERT(options.size() < kMenuList.kMaxRows, "Too many rows for menu list");

	const auto &text_renderer = ResourceManager::get_font(default_font_id_);
	auto pos_y_ratio = kMenuList.kYTopRatio;
	Label option_label{ kMenuList.kXRatio, kMenuList.kYTopRatio, kMenuList.kTextScaleFromHeight, kMenuList.kDeselectedColor, "" };
	for (auto i = size_t{ 0 }; i < options.size(); ++i)
	{
		const auto color = (i == selected_item) ? kMenuList.kSelectedColor : kMenuList.kDeselectedColor;
		option_label.set_text(options.at(i));
		option_label.set_y_ratio(pos_y_ratio);
		option_label.set_color(color);
		option_label.render(loaded_width_, loaded_height_);

		pos_y_ratio += kMenuList.kRowHeightRatio;
	}
}

void Menu::render_submenu()
{
	ASSERT(!menu_stack_.empty(), "No submenu available...");
	const auto &subtitle = menu_stack_.back().subtitle_;
	const auto &options = menu_stack_.back().option_list_;
	const auto &selected_item = menu_stack_.back().selected_item_;

	// TOOD(sasiala): center text for subtitle
	subtitle_.set_text(subtitle);
	subtitle_.render(loaded_width_, loaded_height_);

	if (menu_stack_.size() > 1)
	{
		kBackText.render(loaded_width_, loaded_height_);
	}

	render_options(options, selected_item);
}

} // namespace util