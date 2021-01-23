#ifndef OPENING_MENU_H
#define OPENING_MENU_H

#include "element.h"
#include "menu.h"

namespace util {

	// TODO(sasiala): I don't like this detail namespace used in these headers, b/c
	// it feels like there's a better way and it still requires that the items have
	// names specific to the header (e.g. "OpeningMenuObject" instead of "MenuObject")
	namespace detail {
		constexpr size_t kOpeningMenuStringLength{ 32 };
		class OpeningMenuObject : public ElementUnion<Label<kOpeningMenuStringLength>>
		{
		private:
			using ParentType = ElementUnion<Label<kOpeningMenuStringLength>>;
		public:
			OpeningMenuObject()
				: ParentType{}
			{
			}

			OpeningMenuObject(Label<kOpeningMenuStringLength>& label)
				: ParentType{ label }
			{
			}

			template<typename T>
			T& get()
			{
				return ParentType::get<T>();
			}
		};

		constexpr size_t kMaxOpeningMenuItems{ 3 };
	}

class OpeningMenu : public Element
				  , public Menu<detail::OpeningMenuObject, detail::kMaxOpeningMenuItems>::MenuButtonHandler {
public:
	static constexpr size_t kMaxStringLength = detail::kOpeningMenuStringLength;
	using StringType = string<kMaxStringLength>;
	using LabelType = Label<StringType::max_size()>;

	static constexpr size_t kMaxItems = detail::kMaxOpeningMenuItems;
	using MenuType = Menu<detail::OpeningMenuObject, kMaxItems>;
	using MenuIndex = MenuType::OptionIndex;
	using MenuList = MenuType::OptionList;
	using OptionList = const char *[kMaxItems];

	static const glm::vec3 kSelectedTextColor;
	static const glm::vec3 kDeselectedTextColor;

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

private:
	// Element
	void initialize_impl(const glm::mat4 &projection) override;
	void update_impl(Time dt) override;
	void activate_impl() override;
	void deactivate_impl() override;
	void render_impl(Optional<SpriteRenderer*> parent_sprite_renderer) override;
	void set_key_impl(KeyId key_id, bool val) override;
	// TODO(sasiala): improve event handling
	void process_input_impl(float dt) override;

	// Menu::MenuButtonHandler
	void handle_menu_option_highlight_impl(MenuIndex index) override;
	void handle_menu_option_acceptance_impl(MenuIndex index) override;
	void handle_back_button_impl() override;
	void apply_highlight_impl(MenuType::ElementType &element) override;
	void remove_highlight_impl(MenuType::ElementType &element) override;

	enum class Index {
		kLevelSelection,
		kSettings,
		kHelp,
		kNumIndices,
		kUnknown,
	};
	Index convert_index(const MenuIndex index)
	{
		ASSERT(index < static_cast<MenuIndex>(Index::kNumIndices), "Index out of bounds");
		return static_cast<Index>(index);
	}
	static constexpr OptionList kOptions{
		"Level Selection",
		"Settings",
		"Help"
	};
	MenuType menu_;
	Dimension load_width_;
	Dimension load_height_;
	Handler *handler_;
}; // class OpeningMenu

} // namespace util

#endif // OPENING_MENU_H