#ifndef MENU_H
#define MENU_H

#include "element.h"
#include "resource_mgr.h"
#include "sprite_renderer.h"
#include "text_renderer.h"
#include "types.h"

#include <glm/glm.hpp>

#include <vector>
#include <string>

namespace util {

class Menu : public Element {
public:
	using OptionIndex = size_t;
	using SubmenuLevel = size_t;
	using OptionList = std::vector<std::string>;
private:
	struct MenuGroup {
		std::string subtitle_;
		OptionList  option_list_;
	};
	using MenuList = std::vector<MenuGroup>;

public:
	class MenuButtonHandler {
	public:
		void handle_menu_option_highlight(OptionIndex index, SubmenuLevel submenu_level)
		{
			handle_menu_option_highlight_impl(index, submenu_level);
		}

		void handle_menu_option_acceptance(OptionIndex index, SubmenuLevel submenu_level)
		{
			handle_menu_option_acceptance_impl(index, submenu_level);
		}

	private:
		virtual void handle_menu_option_highlight_impl(OptionIndex index, SubmenuLevel submenu_level) = 0;
		virtual void handle_menu_option_acceptance_impl(OptionIndex index, SubmenuLevel submenu_level) = 0;
	};

	Menu(Dimension load_width, Dimension load_height);

	void set_background_color(const glm::vec3 &background_color);
	void set_menu_handler(MenuButtonHandler &handler);

	void activate(const std::string &title, const std::string &subtitle, OptionList &options);
	void open_sub_menu(const std::string &subtitle, OptionList &options);
	void deactivate();

private:
	// Element
	void initialize_impl(const glm::mat4 &projection) override;
	void update_impl(Time dt) override;
	void render_impl(Optional<SpriteRenderer*> parent_sprite_renderer) override;
	void set_key_impl(KeyId key_id, bool val) override;
	void process_input_impl(float dt) override;

	void render_background();
	void render_title();
	void render_options(const OptionList &options);
	void render_submenu();

	static constexpr const char *kBackgroundTexturePath = "textures/background.jpg";

	const struct {
		float x_;
		float y_;
		float scale_;
		glm::vec3 color_;
	} kTitleText{ 5.0f, 15.0f, 1.5f, glm::vec3{ 1.0f, 1.0f, 1.0f } };
	const struct {
		float x_;
		float y_;
		float scale_;
		glm::vec3 color_;
	} kSubtitleText{ 5.0f, 60.0f, 1.0f, glm::vec3{ 1.0f, 1.0f, 1.0f } };
	static constexpr const char *kDefaultFontPath = "fonts/OCRAEXT.TTF";
	static constexpr double kDefaultFontSize{ 24.0 };

	const Dimension loaded_width_;
	const Dimension loaded_height_;

	std::string title_;

	ResourceManager::Texture2DId background_texture_id_;

	ResourceManager::ShaderId sprite_shader_id_;
	ResourceManager::ShaderId font_shader_id_;

	ResourceManager::FontId default_font_id_;

	glm::vec3 background_color_;
	SpriteRenderer *sprite_renderer_;

	MenuList menu_stack_;

	MenuButtonHandler *menu_button_handler_;
}; // class Menu

} // namespace util

#endif // MENU_H