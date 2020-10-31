#include "opening_menu.h"

namespace util {


	OpeningMenu::OpeningMenu(const Dimension load_width, const Dimension load_height)
		: Element{ false }
		, menu_options_ { load_width, load_height }
		, handler_{ nullptr }
	{
	}

	void OpeningMenu::initialize_impl(const glm::mat4 &projection)
	{
		menu_options_.initialize(projection);
		menu_options_.set_menu_handler(*this);
	}

	void OpeningMenu::update_impl(Time dt)
	{
		menu_options_.update(dt);
	}

	void OpeningMenu::activate_impl()
	{
		menu_options_.update_info("", "", false, kOptions, 0);
		menu_options_.activate();
	}

	void OpeningMenu::deactivate_impl()
	{
		menu_options_.deactivate();
	}

	void OpeningMenu::render_impl(Optional<SpriteRenderer*> parent_sprite_renderer)
	{
		if (!is_active())
		{
			return;
		}

		menu_options_.render(parent_sprite_renderer);
	}

	void OpeningMenu::set_key_impl(KeyId key_id, bool val)
	{
		if (!is_active())
		{
			return;
		}
		
		menu_options_.set_key(key_id, val);
	}

	void OpeningMenu::process_input_impl(float dt)
	{
		if (!is_active())
		{
			return;
		}

		menu_options_.process_input(dt);
	}

	void OpeningMenu::handle_menu_option_highlight_impl(Menu::OptionIndex /*index*/)
	{
		// do nothing
	}

	void OpeningMenu::handle_menu_option_acceptance_impl(Menu::OptionIndex index)
	{
		switch (convert_index(index))
		{
		case Index::kLevelSelection:
			handler_->open_level_selection();
			break;
		case Index::kSettings:
			handler_->open_settings();
			break;
		case Index::kHelp:
			handler_->open_help();
			break;
		default:
			ASSERT(false, "Unexpected index");
			break;
		}
	}

	void OpeningMenu::handle_back_button_impl()
	{
		ASSERT(false, "Back button not allowed on opening menu");
	}

} // namespace util