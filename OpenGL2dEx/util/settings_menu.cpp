#include "settings_menu.h"

namespace util {

	SettingsMenu::SettingsMenu(Dimension load_width, Dimension load_height)
		: Element{ false }
		, menu_{ load_width, load_height }
	{
	}

	void SettingsMenu::initialize_impl(const glm::mat4 &projection)
	{
		menu_.initialize(projection);
	}

	void SettingsMenu::update_impl(Time dt)
	{
		if (!is_active())
		{
			return;
		}

		menu_.update(dt);
	}

	void SettingsMenu::activate_impl()
	{
		menu_.update_info("", "SETTINGS TEST", true, { "A", "B", "C" }, 0);
		menu_.activate();
	}

	void SettingsMenu::deactivate_impl()
	{
		menu_.deactivate();
	}

	void SettingsMenu::render_impl(Optional<SpriteRenderer*> parent_sprite_renderer)
	{
		if (!is_active())
		{
			return;
		}

		menu_.render(parent_sprite_renderer);
	}

	void SettingsMenu::set_key_impl(KeyId key_id, bool val)
	{
		if (!is_active())
		{
			return;
		}

		menu_.set_key(key_id, val);
	}

	void SettingsMenu::process_input_impl(float dt)
	{
		if (!is_active())
		{
			return;
		}

		menu_.process_input(dt);
	}

} // namespace util