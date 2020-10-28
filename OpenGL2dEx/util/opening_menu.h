#ifndef OPENING_MENU_H
#define OPENING_MENU_H

#include "element.h"
#include "menu.h"

namespace util {

class OpeningMenu : public Element
				  , public Menu::MenuButtonHandler {
public:
	class Handler {
	public:
		void open_level_selection()
		{
			open_level_selection_impl();
		}

		void open_settings()
		{
			open_settings_impl();
		}

		void open_help()
		{
			open_help_impl();
		}

	private:
		virtual void open_level_selection_impl() = 0;
		virtual void open_settings_impl() = 0;
		virtual void open_help_impl() = 0;
	};
	OpeningMenu(Dimension load_width, Dimension load_height);
	
	void set_handler(Handler &handler)
	{
		handler_ = &handler;
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

	enum class Index {
		kLevelSelection,
		kSettings,
		kHelp,
		kNumIndices,
		kUnknown,
	};
	Index convert_index(Menu::OptionIndex index)
	{
		ASSERT(index < static_cast<Menu::OptionIndex>(Index::kNumIndices), "Index out of bounds");
		return static_cast<Index>(index);
	}
	const Menu::OptionList kOptions{
		"Level Selection",
		"Settings",
		"Help"
	};

	bool is_open_;
	Menu menu_options_;

	Handler *handler_;
}; // class OpeningMenu

} // namespace util

#endif // OPENING_MENU_H