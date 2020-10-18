#include "menu.h"

#include "logging.h"

namespace util {

Menu::Menu(Dimension load_width, Dimension load_height)
	: loaded_width_{ load_width }
	, loaded_height_{ load_height }
	, background_texture_id_{}
	, sprite_shader_id_{}
	, font_shader_id_{}
	, default_font_id_{}
	, background_color_{ 0.0f, 0.0f, 1.0f }
	, sprite_renderer_{ nullptr }
	, menu_stack_{}
	, menu_button_handler_{ nullptr }
{
}

void Menu::set_background_color(const glm::vec3 &background_color)
{
	background_color_ = background_color;
}

void Menu::set_menu_handler(MenuButtonHandler &handler)
{
	menu_button_handler_ = &handler;
}

void Menu::activate(const std::string &title, const std::string &subtitle, OptionList &options)
{
	deactivate();

	title_ = title;
	menu_stack_.push_back({ subtitle, options });
}

void Menu::open_sub_menu(const std::string &subtitle, OptionList &options)
{
	menu_stack_.push_back({ subtitle, options });
}

void Menu::deactivate()
{
	title_.clear();
	menu_stack_.clear();
}

void Menu::initialize_impl(const glm::mat4 &projection)
{
	background_texture_id_ = ResourceManager::load_texture(kBackgroundTexturePath, false);

	sprite_shader_id_ = ResourceManager::load_shader("shaders/sprite.vs", "shaders/sprite.fs", { util::nullopt });
	font_shader_id_ = ResourceManager::load_shader("shaders/text_2d.vs", "shaders/text_2d.fs", { util::nullopt });

	// text
	default_font_id_ = ResourceManager::load_font(kDefaultFontPath, font_shader_id_, kDefaultFontSize, loaded_width_, loaded_height_);

	sprite_renderer_ = new SpriteRenderer{ ResourceManager::get_shader(sprite_shader_id_) };

	auto &sprite_shader = ResourceManager::get_shader(sprite_shader_id_);
	sprite_shader.use();
	sprite_shader.set_int("u_image_", 0, false);
	sprite_shader.set_mat4("u_projection_", projection, false);
}

void Menu::update_impl(Time /*dt*/)
{
}

void Menu::render_impl(Optional<SpriteRenderer*> /*parent_sprite_renderer*/)
{
	if (!menu_stack_.empty())
	{
		render_background();
		render_title();
		render_submenu();
	}
}

void Menu::set_key_impl(KeyId key_id, bool val)
{
	// TODO(sasiala)
}

void Menu::process_input_impl(float dt)
{
	// TODO(sasiala)
}

void Menu::render_background()
{
	const auto &background_texture = ResourceManager::get_texture(background_texture_id_);
	sprite_renderer_->draw(background_texture, { 0.0f, 0.0f }, { loaded_width_, loaded_height_ }, 0.0f, background_color_);
}

void Menu::render_title()
{
	// TOOD(sasiala): center text
	auto &text_renderer = ResourceManager::get_font(default_font_id_);
	text_renderer.update_size(loaded_width_, loaded_height_);
	text_renderer.render_text(title_, kTitleText.x_, kTitleText.y_, kTitleText.scale_, kTitleText.color_);
}

void Menu::render_options(const OptionList &options)
{
	// TODO(sasiala)
}

void Menu::render_submenu()
{
	ASSERT(!menu_stack_.empty(), "No submenu available...");
	const auto &subtitle = menu_stack_.back().subtitle_;
	const auto &options = menu_stack_.back().option_list_;

	// TOOD(sasiala): center text for subtitle
	auto &text_renderer = ResourceManager::get_font(default_font_id_);
	text_renderer.update_size(loaded_width_, loaded_height_);
	text_renderer.render_text(subtitle, kSubtitleText.x_, kSubtitleText.y_, kSubtitleText.scale_, kSubtitleText.color_);

	render_options(options);
}

} // namespace util