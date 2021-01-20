#ifndef SETTINGS_MENU_H
#define SETTINGS_MENU_H

#include "element.h"
#include "menu.h"
#include "types.h"

namespace util {

class SettingsMenu : public Element
{
public:
	using LabelType = Label<32>;

private:
	using ToggleType = ElementPair<LabelType, ElementPair<LabelType, LabelType>>;
	class MenuObject : public ElementUnion<LabelType, ToggleType>
	{
	private:
		using ParentType = ElementUnion<LabelType, ToggleType>;
	public:
		MenuObject()
			: ParentType{}
		{
		}

		MenuObject(const LabelType& label)
			: ParentType{label}
		{
		}

		MenuObject(const ToggleType& toggle)
			: ParentType{toggle}
		{
		}

		template<typename T>
		T& get()
		{
			return ParentType::get<T>();
		}
	};
	static constexpr size_t kMenuObjectCount{ 3 };
	using MenuType = Menu<MenuObject, kMenuObjectCount>;

public:
	SettingsMenu(Dimension load_width, Dimension load_height);

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

	MenuType menu_;
	Dimension loaded_width_;
	Dimension loaded_height_;
};

}

#endif // SETTINGS_MENU_H