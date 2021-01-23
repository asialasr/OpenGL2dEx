#include "settings_menu.h"

namespace util {
	const glm::vec3 SettingsMenu::kDeselectedTextColor{ 0.0f, 1.0f, 0.0f };
	const glm::vec3 SettingsMenu::kSelectedTextColor{ 1.0f, 1.0f, 1.0f };

namespace {
	SettingsMenu::LabelType make_label(const char     *str, 
					 size_t          button_index, 
					 const Dimension viewport_width, 
					 const Dimension viewport_height)
	{
		return{ true, 0.05f, static_cast<float>(0.25f + button_index * (.65 / 4)), 1.0f / 600.0f, SettingsMenu::kDeselectedTextColor, str, viewport_width, viewport_height };
	}
} // namespace

	SettingsMenu::SettingsMenu(Dimension load_width, Dimension load_height)
		: Element{ false }
		, menu_{ load_width, load_height }
		, loaded_width_{ load_width }
		, loaded_height_{ load_height }
		, handler_{ nullptr }
	{
	}

	void SettingsMenu::initialize_impl(const glm::mat4 &projection)
	{
		menu_.initialize(projection);
		menu_.set_menu_handler(*this);
	}

	void SettingsMenu::update_impl(Time dt)
	{
		if (!is_active())
		{
			return;
		}

		menu_.update(dt);
	}

	void SettingsMenu::activate_impl()
	{
		const auto loaded_width = loaded_width_;
		const auto loaded_height = loaded_height_;
		auto create_label = [loaded_width, loaded_height](const char *str, size_t index) {
			return MenuObject{ make_label(str, index, loaded_width, loaded_height) };
		};
		auto make_list = [create_label, loaded_width, loaded_height]() -> MenuType::OptionList
		{
			static MenuType::OptionList::ObjectArray arr{ create_label("A", 0), create_label("B", 1), create_label("C", 2) };
			return{ arr };
		};
		menu_.update_info("MAIN MENU", "SETTINGS TEST", true, make_list(), 0);
		menu_.activate();
	}

	void SettingsMenu::deactivate_impl()
	{
		menu_.deactivate();
	}

	void SettingsMenu::render_impl(Optional<SpriteRenderer*> parent_sprite_renderer)
	{
		if (!is_active())
		{
			return;
		}

		menu_.render(parent_sprite_renderer);
	}

	void SettingsMenu::set_key_impl(KeyId key_id, bool val)
	{
		if (!is_active())
		{
			return;
		}

		menu_.set_key(key_id, val);
	}

	void SettingsMenu::process_input_impl(float dt)
	{
		if (!is_active())
		{
			return;
		}

		menu_.process_input(dt);
	}

	void SettingsMenu::handle_menu_option_highlight_impl(const MenuIndex /*index*/)
	{
	}

	void SettingsMenu::handle_menu_option_acceptance_impl(const MenuIndex /*index*/)
	{
	}

	void SettingsMenu::handle_back_button_impl()
	{
		ASSERT(handler_, "No settings menu handler");

		handler_->close_settings();
	}

	void SettingsMenu::apply_highlight_impl(MenuType::ElementType &element)
	{
		if (element.contained_type() == MenuType::ElementType::ContainedType::kLabel)
		{
			element.get<LabelType>().set_color(kSelectedTextColor);
		}
		else
		{
			// TODO(sasiala): toggles, etc.
		}
	}

	void SettingsMenu::remove_highlight_impl(MenuType::ElementType &element)
	{
		if (element.contained_type() == MenuType::ElementType::ContainedType::kLabel)
		{
			element.get<LabelType>().set_color(kDeselectedTextColor);
		}
		else
		{
			// TODO(sasiala): toggles, etc.
		}
	}
} // namespace util