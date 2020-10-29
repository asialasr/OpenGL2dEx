#include "level_selection_menu.h"
#include "logging.h"

namespace util {
	LevelSelectionMenu::LevelSelectionMenu(Dimension load_width, Dimension load_height)
		: menu_options_{ load_width, load_height }
		, handler_{ nullptr }
		, levels_{}
		, current_level_{}
	{
	}

	void LevelSelectionMenu::initialize_impl(const glm::mat4 &projection)
	{
		menu_options_.initialize(projection);
		menu_options_.set_menu_handler(*this);
	}

	void LevelSelectionMenu::update_impl(Time dt)
	{
		if (!is_active())
		{
			return;
		}

		menu_options_.update(dt);
	}

	void LevelSelectionMenu::activate_impl()
	{
		ASSERT(!levels_.empty(), "No levels to display");
		ASSERT(handler_, "No handler for menu");
		menu_options_.update_info("", kSubtitle, true, levels_, current_level_);
		menu_options_.activate();
		handler_->show_level_preview();
	}

	void LevelSelectionMenu::deactivate_impl()
	{
		ASSERT(handler_, "No handler for menu");
		menu_options_.deactivate();
		handler_->hide_level_preview();
	}

	void LevelSelectionMenu::render_impl(Optional<SpriteRenderer*> parent_sprite_renderer)
	{
		if (!is_active())
		{
			return;
		}

		menu_options_.render(parent_sprite_renderer);
	}

	void LevelSelectionMenu::set_key_impl(KeyId key_id, bool val)
	{
		if (!is_active())
		{
			return;
		}

		menu_options_.set_key(key_id, val);
	}

	void LevelSelectionMenu::process_input_impl(float dt)
	{
		if (!is_active())
		{
			return;
		}
		menu_options_.process_input(dt);
	}

	void LevelSelectionMenu::handle_menu_option_highlight_impl(Menu::OptionIndex index)
	{
		current_level_ = static_cast<LevelIndex>(index);
		handler_->change_level(current_level_);
	}

	void LevelSelectionMenu::handle_menu_option_acceptance_impl(Menu::OptionIndex /*index*/)
	{
		handler_->start_game();
	}

	void LevelSelectionMenu::handle_back_button_impl()
	{
		handler_->close_level_selection();
	}
} // namespace util