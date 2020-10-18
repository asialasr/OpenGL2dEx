#include "game.h"

#include "audio_manager.h"
#include "ball_object.h"
#include "logging.h"
#include "gl_debug.h"
#include "particle_generator.h"

#include <algorithm>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace util
{
	Game::Game(IResetGlProperties &gl_property_resetter,
			   Dimension width, 
		       Dimension height)
		: state_{ GameState::kMenu }
		, keys_{}
		, keys_processed_{}
		, width_{ width }
		, height_{ height }
		, game_viewport_{ gl_property_resetter, width, height }
		, main_menu_{ width, height }
		, sprite_renderer_{ nullptr }
		, sprite_shader_id_{}
		, smiley_texture_id_{}
		, default_font_id_{}
		, current_level_{0}
		, viewport_animation_{ false, 0, 0, 0, 0, 0, 0 }
	{
	}

	void Game::delete_dynamic_data()
	{
		if (sprite_renderer_)
		{
			delete sprite_renderer_;
		}
		sprite_renderer_ = nullptr;
	}

	Game::~Game()
	{
		delete_dynamic_data();
	}

	void Game::load_current_level()
	{
		game_viewport_.load_level(kLevelPaths[current_level_]);
	}

	void Game::initialize()
	{
		delete_dynamic_data();

		sprite_shader_id_ = ResourceManager::load_shader("shaders/sprite.vs", "shaders/sprite.fs", {});
		font_shader_id_ = ResourceManager::load_shader("shaders/text_2d.vs", "shaders/text_2d.fs", {});

		glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(width_),
			static_cast<float>(height_), 0.0f, -1.0f, 1.0f);
		ResourceManager::get_shader(sprite_shader_id_).use();
		ResourceManager::get_shader(sprite_shader_id_).set_int("u_image_", 0, false);
		ResourceManager::get_shader(sprite_shader_id_).set_mat4("u_projection_", projection, false);

		sprite_renderer_ = new SpriteRenderer{ ResourceManager::get_shader(sprite_shader_id_) };

		smiley_texture_id_ = ResourceManager::load_texture(kSmileyImagePath, true);

		default_font_id_ = ResourceManager::load_font(kDefaultFontPath, font_shader_id_, kDefaultFontSize, width_, height_);

		game_viewport_.initialize(projection);
		game_viewport_.set_game_state_callback(*this);

		main_menu_.initialize(projection);
		main_menu_.set_menu_handler(*this);

		current_level_ = 0;

		load_current_level();

		AudioManager::play_background_music(AudioManager::GameState::kActive, true);

		open_main_menu();

		check_for_gl_errors();
	}

	void Game::process_input(float dt)
	{
		if (GameState::kActive == state_)
		{
			game_viewport_.process_input(dt);
		}
		else if (GameState::kMenu == state_)
		{
			main_menu_.process_input(dt);
		}
	}

	void Game::update(float dt)
	{
		if (state_ == GameState::kActive)
		{
			game_viewport_.update(dt);
		}
		else if (state_ == GameState::kMenu)
		{
			main_menu_.update(dt);
		}

		if (viewport_animation_.animation_in_progress_)
		{
			animate_game_viewport(dt);
		}
	}

	void Game::render()
	{
		// render menu on bottom
		if (state_ == GameState::kMenu)
		{
			render_menu();
			check_for_gl_errors();
		}

		game_viewport_.render(sprite_renderer_);

		check_for_gl_errors();
	}

	void Game::set_key(size_t key, bool val)
	{
		ASSERT(key < kNumKeys, "Key out of bounds");
		keys_[key] = val;
		if (!val)
		{
			keys_processed_[key] = false;
		}

		game_viewport_.set_key(key, val);
		main_menu_.set_key(key, val);
	}

	void Game::game_ended_impl(const EndingReason /*reason*/)
	{
		// TODO(sasiala): handle various ending reasons
		state_ = GameState::kMenu;
		open_main_menu();
	}

	void Game::handle_menu_option_highlight_impl(Menu::OptionIndex index, Menu::SubmenuLevel submenu_level)
	{
		ASSERT(submenu_level == 0, "Unexpected submenu level");
		ASSERT(index >= 0 && index < kMaxLevels, "Unexpected menu index");
		game_viewport_.load_level(kLevelPaths[index]);
		current_level_ = index;
	}

	void Game::handle_menu_option_acceptance_impl(Menu::OptionIndex index, Menu::SubmenuLevel submenu_level)
	{
		close_main_menu();
	}

	void Game::open_main_menu()
	{
		main_menu_.activate("MAIN MENU", "LEVELS", Menu::OptionList{ kLevelNames }, current_level_);
		game_viewport_.set_position({ .45 * width_, .25 * height_ });

		// TODO(sasiala): centralize dx/dy values & come up with better values - these were simply trial/error
		// TODO(sasiala): centralize the viewport animation constants/max & min values/etc.
		const float dx = 1.5 * width_;
		const float dy = 1.5 * height_;
		viewport_animation_ = { true, -dx, -dy, static_cast<Dimension>(.5 * width_), static_cast<Dimension>(.5 * height_), width_, height_ };
		state_ = GameState::kMenu;
	}

	void Game::close_main_menu()
	{
		game_viewport_.set_position({ 0.0f, 0.0f });
		main_menu_.deactivate();

		// TODO(sasiala): centralize dx/dy values & come up with better values - these were simply trial/error
		// TODO(sasiala): centralize the viewport animation constants/max & min values/etc.
		const float dx = 1.5 * width_;
		const float dy = 1.5 * height_;
		viewport_animation_ = { true, dx, dy, width_, height_, static_cast<Dimension>(.5 * width_), static_cast<Dimension>(.5 * height_) };
		state_ = GameState::kActive;
	}

	void Game::render_menu()
	{
		main_menu_.render(sprite_renderer_);
	}

	namespace {
		Dimension animate_dimension(Dimension current, Dimension target, int dx)
		{
			if (current < target)
			{
				if (current + dx >= target)
				{
					current = target;
				}
				else
				{
					current += dx;
				}
			}
			else
			{
				if (std::abs(dx) > current || current + dx <= target)
				{
					current = target;
				}
				else
				{
					current += dx;
				}
			}

			return current;
		}
	}

	void Game::animate_game_viewport(float dt)
	{
		ASSERT(viewport_animation_.animation_in_progress_, "No animation in progress");

		viewport_animation_.current_x_ = animate_dimension(viewport_animation_.current_x_, 
			viewport_animation_.target_x_, static_cast<int>(dt * viewport_animation_.dx_));
		viewport_animation_.current_y_ = animate_dimension(viewport_animation_.current_y_, 
			viewport_animation_.target_y_, static_cast<int>(dt * viewport_animation_.dy_));

		const auto x_complete = (viewport_animation_.current_x_ == viewport_animation_.target_x_);
		const auto y_complete = (viewport_animation_.current_y_ == viewport_animation_.target_y_);
		if (x_complete && y_complete)
		{
			viewport_animation_.animation_in_progress_ = false;
		}
		if (x_complete)
		{
			viewport_animation_.dx_ = 0;
		}
		if (y_complete)
		{
			viewport_animation_.dy_ = 0;
		}

		game_viewport_.set_size(viewport_animation_.current_x_, viewport_animation_.current_y_);
	}

} // namespace util