#ifndef LEVEL_SELECTION_MENU_H
#define LEVEL_SELECTION_MENU_H

#include "element.h"
#include "menu.h"

namespace util {

class LevelSelectionMenu : public Element
						 , public Menu::MenuButtonHandler {
public:
	using LevelIndex = Menu::OptionIndex;
	using LevelList = Menu::OptionList;
	class Handler {
	public:
		void change_level(const LevelIndex level_index)
		{
			change_level_impl(level_index);
		}

		void start_game()
		{
			start_game_impl();
		}

		void close_level_selection()
		{
			close_level_selection_impl();
		}

		void show_level_preview()
		{
			show_level_preview_impl();
		}

		void hide_level_preview()
		{
			hide_level_preview_impl();
		}

	private:
		virtual void change_level_impl(LevelIndex level_index) = 0;
		virtual void start_game_impl() = 0;
		virtual void close_level_selection_impl() = 0;
		virtual void show_level_preview_impl() = 0;
		virtual void hide_level_preview_impl() = 0;
	};
	LevelSelectionMenu(Dimension load_width, Dimension load_height);

	void set_handler(Handler &handler)
	{
		handler_ = &handler;
	}

	void update_current_level(const LevelIndex current_level)
	{
		current_level_ = current_level;
	}

	void update_levels(const LevelList &levels, const LevelIndex current_level)
	{
		levels_ = levels;
		current_level_ = current_level;
	}

	void activate();
	void deactivate();
private:
	// Element
	void initialize_impl(const glm::mat4 &projection) override;
	void update_impl(Time dt) override;
	void render_impl(Optional<SpriteRenderer*> parent_sprite_renderer) override;
	void set_key_impl(KeyId key_id, bool val) override;
	// TODO(sasiala): improve event handling
	void process_input_impl(float dt) override;

	// Menu::MenuButtonHandler
	void handle_menu_option_highlight_impl(Menu::OptionIndex index) override;
	void handle_menu_option_acceptance_impl(Menu::OptionIndex index) override;
	void handle_back_button_impl() override;

	bool is_open_;
	Menu menu_options_;

	Handler *handler_;

	LevelList levels_;
	LevelIndex current_level_;

	static constexpr const char *kSubtitle = "LEVEL SELECTION";
}; // class LevelSelectionMenu

} // namespace util

#endif // LEVEL_SELECTION_MENU_H