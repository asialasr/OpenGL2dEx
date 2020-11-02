#ifndef SETTINGS_MENU_H
#define SETTINGS_MENU_H

#include "element.h"
#include "menu.h"

namespace util {

class SettingsMenu : public Element
{
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

	Menu menu_;
};

}

#endif // SETTINGS_MENU_H