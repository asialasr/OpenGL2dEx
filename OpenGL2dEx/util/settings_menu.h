#ifndef SETTINGS_MENU_H
#define SETTINGS_MENU_H

#include "element.h"
#include "menu.h"
#include "types.h"

namespace util {

namespace detail {

	using SettingsLabelType = Label<32>;
	using SettingsToggleType = ElementPair<SettingsLabelType, ElementPair<SettingsLabelType, SettingsLabelType>>;
	class SettingsMenuObject : public ElementUnion<SettingsLabelType, SettingsToggleType>
	{
	private:
		using ParentType = ElementUnion<SettingsLabelType, SettingsToggleType>;
	public:
		enum class ContainedType {
			kLabel,
			kToggle,
			kNumTypes,
			kUnknown,
		};

		SettingsMenuObject()
			: ParentType{}
			, contained_type_{ ContainedType::kUnknown }
		{
		}

		SettingsMenuObject(const SettingsLabelType& label)
			: ParentType{ label }
			, contained_type_{ ContainedType::kLabel }
		{
		}

		SettingsMenuObject(const SettingsToggleType& toggle)
			: ParentType{ toggle }
			, contained_type_{ ContainedType::kToggle }
		{
		}

		template<typename T>
		T& get()
		{
			ASSERT(contained_type_ < ContainedType::kNumTypes, "Invalid type contained");
			return ParentType::get<T>();
		}

		ContainedType contained_type() const
		{
			return contained_type_;
		}

	private:
		ContainedType contained_type_;
	};
	static constexpr size_t kSettingsMenuObjectCount{ 3 };
} // namespace detail

class SettingsMenu : public Element
				   , public Menu<detail::SettingsMenuObject, detail::kSettingsMenuObjectCount>::MenuButtonHandler
{
public:
	using LabelType = detail::SettingsLabelType;
	using ToggleType = detail::SettingsToggleType;

	static constexpr size_t kMenuObjectCount{ detail::kSettingsMenuObjectCount };
	using MenuObject = detail::SettingsMenuObject;
	using MenuType = Menu<MenuObject, kMenuObjectCount>;
	using MenuIndex = MenuType::OptionIndex;
	using MenuList = MenuType::OptionList;

	static const glm::vec3 kSelectedTextColor;
	static const glm::vec3 kDeselectedTextColor;

	class Handler {
	public:
		void close_settings()
		{
			close_settings_impl();
		}

	private:
		virtual void close_settings_impl() = 0;
	};
	SettingsMenu(Dimension load_width, Dimension load_height);

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

	void handle_menu_option_highlight_impl(const MenuIndex index) override;
	void handle_menu_option_acceptance_impl(const MenuIndex /*index*/) override;
	void handle_back_button_impl() override;
	void apply_highlight_impl(MenuType::ElementType &element) override;
	void remove_highlight_impl(MenuType::ElementType &element) override;

	MenuType menu_;
	Dimension loaded_width_;
	Dimension loaded_height_;
	Handler *handler_;
};

}

#endif // SETTINGS_MENU_H