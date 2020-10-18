#ifndef MENU_H
#define MENU_H

#include "element.h"
#include "logging.h"
#include "resource_mgr.h"
#include "sprite_renderer.h"
#include "text_renderer.h"
#include "types.h"
#include "optional.h"

#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

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
		OptionIndex selected_item_;
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

	void activate(const std::string &title, const std::string &subtitle, OptionList &options, OptionIndex selected_item);
	void open_sub_menu(const std::string &subtitle, OptionList &options, OptionIndex selected_item);
	void deactivate();

private:
	// Element
	void initialize_impl(const glm::mat4 &projection) override;
	void update_impl(Time dt) override;
	void render_impl(Optional<SpriteRenderer*> parent_sprite_renderer) override;

	enum class ButtonsHandled {
		kUpButton = 0,
		kDownButton,
		kEnterButton,
		kNumButtons,
		kUnknown,
	};
	size_t to_index(const ButtonsHandled button_id)
	{
		ASSERT(ButtonsHandled::kNumButtons > button_id,
			"Cannot convert to index (out of bounds)");
		return static_cast<size_t>(button_id);
	}
	// TODO(sasiala): improve how key IDs are defined
	ButtonsHandled convert_id(const KeyId button)
	{
		auto converted_id = ButtonsHandled::kUnknown;
		switch (button)
		{
		case GLFW_KEY_W:
			converted_id = ButtonsHandled::kUpButton;
			break;
		case GLFW_KEY_S:
			converted_id = ButtonsHandled::kDownButton;
			break;
		case GLFW_KEY_ENTER:
			converted_id = ButtonsHandled::kEnterButton;
			break;
		default:
			LOG("Unknown button id: " + std::to_string(button));
			break;
		}

		return converted_id;
	}
	void set_key_impl(KeyId key_id, bool val) override;
	void process_input_impl(float dt) override;

	void render_background();
	void render_title();
	void render_options(const OptionList &options, const OptionIndex selected_item);
	void render_submenu();
	void render_text(const TextRenderer		   &text_renderer,
					 const std::string		   &text,
					 float						x, 
					 float						y, 
					 float						scale, 
					 const Optional<glm::vec3> &color);

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
	static constexpr util::TextRenderer::FontSize kDefaultFontSize{ 24 };

	const Dimension loaded_width_;
	const Dimension loaded_height_;

	const struct {
		static constexpr auto kYTopRatio{ 0.20f };
		static constexpr auto kYBottomRatio{ 0.90f };
		static constexpr auto kRowHeightRatio{ 0.10f };
		static constexpr auto kMaxRows{ static_cast<int>((kYBottomRatio - kYTopRatio) / kRowHeightRatio) };
		static constexpr auto kXRatio{ 0.05f };
		static constexpr auto kTextScale{ 1.0f };

		const glm::vec3 kSelectedColor{ 1.0f, 1.0f, 1.0f };
		const glm::vec3 kDeselectedColor{ 0.0f, 1.0f, 0.0f };
	} kMenuList{};

	std::string title_;

	ResourceManager::Texture2DId background_texture_id_;

	ResourceManager::ShaderId sprite_shader_id_;
	ResourceManager::ShaderId font_shader_id_;

	ResourceManager::FontId default_font_id_;

	glm::vec3 background_color_;
	SpriteRenderer *sprite_renderer_;

	MenuList menu_stack_;

	MenuButtonHandler *menu_button_handler_;

	bool keys_pressed_[static_cast<size_t>(ButtonsHandled::kNumButtons)];
	bool keys_processed_[static_cast<size_t>(ButtonsHandled::kNumButtons)];
}; // class Menu

} // namespace util

#endif // MENU_H