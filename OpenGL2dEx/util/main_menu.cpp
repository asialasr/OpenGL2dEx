#include "main_menu.h"

namespace util {


	MainMenu::MainMenu(Dimension load_width, Dimension load_height)
		: loaded_width_{ load_width }
		, loaded_height_{ load_height }
		, background_texture_id_ {}
		, sprite_shader_id_{}
		, background_color_{ 0.0f, 0.0f, 1.0f }
		, sprite_renderer_{ nullptr }
		, menu_button_handler_{ nullptr }
		, opening_menu_{ load_width, load_height }
	{
	}

	void MainMenu::activate(const std::string &title, const std::string &subtitle, Menu::OptionList &options, Menu::OptionIndex selected_item)
	{
		opening_menu_.activate(title, subtitle, options, selected_item);
	}

	void MainMenu::open_sub_menu(const std::string &subtitle, Menu::OptionList &options, Menu::OptionIndex selected_item)
	{
		opening_menu_.open_sub_menu(subtitle, options, selected_item);
	}

	void MainMenu::deactivate()
	{
		opening_menu_.deactivate();
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

		opening_menu_.set_menu_handler(*this);
	}

	void MainMenu::update_impl(Time dt)
	{
		opening_menu_.update(dt);
	}

	void MainMenu::render_impl(Optional<SpriteRenderer*> parent_sprite_renderer)
	{
		render_background();
		opening_menu_.render(parent_sprite_renderer);
	}

	void MainMenu::set_key_impl(KeyId key_id, bool val)
	{
		opening_menu_.set_key(key_id, val);
	}

	void MainMenu::process_input_impl(float dt)
	{
		opening_menu_.process_input(dt);
	}

	void MainMenu::render_background()
	{
		const auto &background_texture = ResourceManager::get_texture(background_texture_id_);
		sprite_renderer_->draw(background_texture, { 0.0f, 0.0f }, { loaded_width_, loaded_height_ }, 0.0f, background_color_);
	}

} // namespace util