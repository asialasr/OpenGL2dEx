#include "level_selection_menu.h"
#include "logging.h"

namespace util {
	LevelSelectionMenu::LevelSelectionMenu(Dimension load_width, Dimension load_height)
		: menu_options_{ load_width, load_height }
		, is_open_{ false }
		, handler_{ nullptr }
		, levels_{}
		, current_level_{}
	{
	}

	void LevelSelectionMenu::activate()
	{
		ASSERT(!levels_.empty(), "No levels to display");
		menu_options_.activate("", kSubtitle, levels_, current_level_);
		is_open_ = true;
	}

	void LevelSelectionMenu::deactivate()
	{
		menu_options_.deactivate();
		is_open_ = false;
	}

	void LevelSelectionMenu::initialize_impl(const glm::mat4 &projection)
	{
		menu_options_.initialize(projection);
		menu_options_.set_menu_handler(*this);
	}

	void LevelSelectionMenu::update_impl(Time dt)
	{
		if (is_open_)
		{
			menu_options_.update(dt);
		}
	}

	void LevelSelectionMenu::render_impl(Optional<SpriteRenderer*> parent_sprite_renderer)
	{
		if (is_open_)
		{
			menu_options_.render(parent_sprite_renderer);
		}
	}

	void LevelSelectionMenu::set_key_impl(KeyId key_id, bool val)
	{
		if (is_open_)
		{
			menu_options_.set_key(key_id, val);
		}
	}

	void LevelSelectionMenu::process_input_impl(float dt)
	{
		if (is_open_)
		{
			menu_options_.process_input(dt);
		}
	}

	void LevelSelectionMenu::handle_menu_option_highlight_impl(Menu::OptionIndex index)
	{
		handler_->change_level(static_cast<LevelIndex>(index));
	}

	void LevelSelectionMenu::handle_menu_option_acceptance_impl(Menu::OptionIndex /*index*/)
	{
		handler_->start_game();
	}

	void LevelSelectionMenu::handle_submenu_dismiss_impl()
	{
		handler_->close_level_selection();
	}
} // namespace util