#include "main_menu.h"

#include <algorithm>

namespace util {

	MainMenu::MainMenu(Dimension load_width, Dimension load_height)
		: Element{ false }
		, loaded_width_{ load_width }
		, loaded_height_{ load_height }
		, background_texture_id_ {}
		, sprite_shader_id_{}
		, background_color_{ 0.0f, 0.0f, 1.0f }
		, sprite_renderer_{ nullptr }
		, menu_button_handler_{ nullptr }
		, opening_menu_{ load_width, load_height }
		, level_selection_menu_{ load_width, load_height }
		, settings_menu_{ load_width, load_height }
		, menu_stack_{}
	{
	}

	void MainMenu::initialize_impl(const glm::mat4 &projection)
	{
		background_texture_id_ = ResourceManager::load_texture(kBackgroundTexturePath, false);

		sprite_shader_id_ = ResourceManager::load_shader("shaders/sprite.vs", "shaders/sprite.fs", { util::nullopt });

		sprite_renderer_ = new SpriteRenderer{ ResourceManager::get_shader(sprite_shader_id_) };

		auto &sprite_shader = ResourceManager::get_shader(sprite_shader_id_);
		sprite_shader.use();
		sprite_shader.set_int("u_image_", 0, false);
		sprite_shader.set_mat4("u_projection_", projection, false);

		opening_menu_.initialize(projection);
		opening_menu_.set_handler(*this);

		level_selection_menu_.initialize(projection);
		level_selection_menu_.set_handler(*this);
	}

	void MainMenu::update_impl(const Time dt)
	{
		if (!is_active())
		{
			return;
		}

		ASSERT(!menu_stack_.empty(), "No menu to update");
		menu_stack_.back()->update(dt);
	}

	void MainMenu::activate_impl()
	{
		if (menu_stack_.empty())
		{
			open_menu(opening_menu_);
		}
	}

	void MainMenu::deactivate_impl()
	{
		menu_stack_.back()->deactivate();
		menu_stack_.clear();
	}

	void MainMenu::render_impl(Optional<SpriteRenderer*> parent_sprite_renderer)
	{
		if (!is_active())
		{
			return;
		}

		render_background();
		ASSERT(!menu_stack_.empty(), "No menu to render");
		menu_stack_.back()->render(parent_sprite_renderer);
	}

	void MainMenu::set_key_impl(const KeyId key_id, const bool val)
	{
		if (!is_active())
		{
			return;
		}

		ASSERT(!menu_stack_.empty(), "No menu open to set key");
		menu_stack_.back()->set_key(key_id, val);
	}

	void MainMenu::process_input_impl(const float dt)
	{
		if (!is_active())
		{
			return;
		}

		ASSERT(!menu_stack_.empty(), "No menu open to process input");
		menu_stack_.back()->process_input(dt);
	}

	void MainMenu::render_background()
	{
		const auto &background_texture = ResourceManager::get_texture(background_texture_id_);
		sprite_renderer_->draw(background_texture, { 0.0f, 0.0f }, { loaded_width_, loaded_height_ }, 0.0f, background_color_);
	}

} // namespace util