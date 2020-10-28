#ifndef MAIN_MENU_H
#define MAIN_MENU_H

#include "element.h"
#include "level_selection_menu.h"
#include "opening_menu.h"
#include "resource_mgr.h"

namespace util {

class MainMenu : public Element
			   , public OpeningMenu::Handler
			   , public LevelSelectionMenu::Handler 
{
public:
	class MenuButtonHandler {
	public:
		void change_level(const LevelSelectionMenu::LevelIndex level)
		{
			change_level_impl(level);
		}

		void start_game()
		{
			start_game_impl();
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
		virtual void change_level_impl(LevelSelectionMenu::LevelIndex level) = 0;
		virtual void start_game_impl() = 0;
		virtual void show_level_preview_impl() = 0;
		virtual void hide_level_preview_impl() = 0;
	};
	MainMenu(Dimension load_width, Dimension load_height);

	void set_menu_handler(MenuButtonHandler &handler)
	{
		menu_button_handler_ = &handler;
	}

	void set_background_color(const glm::vec3 &background_color)
	{
		background_color_ = background_color;
	}

	void update_current_level(const LevelSelectionMenu::LevelIndex current_level)
	{
		level_selection_menu_.update_current_level(current_level);
	}

	void update_levels(const LevelSelectionMenu::LevelList &levels, const LevelSelectionMenu::LevelIndex current_level)
	{
		level_selection_menu_.update_levels(levels, current_level);
	}

	void activate(const std::string &title, const std::string &subtitle, Menu::OptionList &options, Menu::OptionIndex selected_item);
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
	void open_level_selection_impl() override
	{
		opening_menu_.deactivate();
		level_selection_menu_.activate();
	}

	void open_settings_impl() override
	{
		// TODO(sasiala)
	}

	void open_help_impl() override
	{
		// TODO(sasiala)
	}

	// LevelSelectionMenu::Handler
	void change_level_impl(const LevelSelectionMenu::LevelIndex level_index) override
	{
		ASSERT(menu_button_handler_, "No menu handler");
		menu_button_handler_->change_level(level_index);
	}

	void start_game_impl() override
	{
		ASSERT(menu_button_handler_, "No menu handler");
		menu_button_handler_->start_game();
	}

	void close_level_selection_impl() override
	{
		level_selection_menu_.deactivate();
		opening_menu_.activate();
	}

	void show_level_preview_impl() override
	{
		ASSERT(menu_button_handler_, "No menu handler");
		menu_button_handler_->show_level_preview();
	}

	void hide_level_preview_impl() override
	{
		ASSERT(menu_button_handler_, "No menu handler");
		menu_button_handler_->hide_level_preview();
	}

	void render_background();

	Dimension loaded_width_;
	Dimension loaded_height_;

	static constexpr const char *kBackgroundTexturePath = "textures/background.jpg";
	ResourceManager::Texture2DId background_texture_id_;

	ResourceManager::ShaderId sprite_shader_id_;

	glm::vec3 background_color_;
	SpriteRenderer *sprite_renderer_;

	MenuButtonHandler *menu_button_handler_;

	OpeningMenu opening_menu_;
	LevelSelectionMenu level_selection_menu_;

}; // class MainMenu

} // namespace util

#endif // MAIN_MENU_H