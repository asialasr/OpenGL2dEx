#ifndef MAIN_MENU_H
#define MAIN_MENU_H

#include "element.h"
#include "opening_menu.h"
#include "resource_mgr.h"

namespace util {

class MainMenu : public Element
			   , public OpeningMenu::Handler {
public:
	class MenuButtonHandler {
	public:
		void handle_menu_option_highlight(Menu::OptionIndex index, Menu::SubmenuLevel submenu_level)
		{
			handle_menu_option_highlight_impl(index, submenu_level);
		}

		void handle_menu_option_acceptance(Menu::OptionIndex index, Menu::SubmenuLevel submenu_level)
		{
			handle_menu_option_acceptance_impl(index, submenu_level);
		}

		void handle_submenu_dismiss()
		{
			handle_submenu_dismiss_impl();
		}

	private:
		virtual void handle_menu_option_highlight_impl(Menu::OptionIndex index, Menu::SubmenuLevel submenu_level) = 0;
		virtual void handle_menu_option_acceptance_impl(Menu::OptionIndex index, Menu::SubmenuLevel submenu_level) = 0;
		virtual void handle_submenu_dismiss_impl() = 0;
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

	void activate(const std::string &title, const std::string &subtitle, Menu::OptionList &options, Menu::OptionIndex selected_item);
	void open_sub_menu(const std::string &subtitle, Menu::OptionList &options, Menu::OptionIndex selected_item);
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
		// TODO(sasiala)
	}

	void open_settings_impl() override
	{
		// TODO(sasiala)
	}

	void open_help_impl() override
	{
		// TODO(sasiala)
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
}; // class MainMenu

} // namespace util

#endif // MAIN_MENU_H