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
	Game::GameSpeedMultiplier Game::game_speed_multiplier_{ 1.0f };

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
				if (static_cast<unsigned int>(std::abs(dx)) > current || current + dx <= target)
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
	} // namespace

	Game::Game(IResetGlProperties &gl_property_resetter,
		Dimension width,
		Dimension height)
		: state_{ GameState::kMainMenu }
		, keys_{}
		, keys_processed_{}
		, width_{ width }
		, height_{ height }
		, game_viewport_{ gl_property_resetter, width, height }
		, main_menu_{ width, height }
		, sprite_renderer_{ nullptr }
		, sprite_shader_id_{}
		, current_level_{ 0 }
		, viewport_animation_{ false, 0, 0, {0.0f, 0.0f}, {0.0f, 0.0f}, 0, 0, 0, 0 }
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

		game_viewport_.initialize(projection);
		game_viewport_.set_game_state_callback(*this);

		main_menu_.initialize(projection);
		main_menu_.set_menu_handler(*this);

		current_level_ = 0;

		load_current_level();

		main_menu_.update_levels(kLevelNames, current_level_);

		AudioManager::play_background_music(AudioManager::GameState::kActive, true);

		open_main_menu();

		check_for_gl_errors();
	}

	void Game::process_input(float dt)
	{
		if (GameState::kActive == state_)
		{
			game_viewport_.process_input(game_speed_multiplier_ * dt);
		}
		else if (GameState::kMainMenu == state_)
		{
			main_menu_.process_input(dt);
		}
	}

	void Game::update(float dt)
	{
		if (state_ == GameState::kActive)
		{
			game_viewport_.update(game_speed_multiplier_ * dt);
		}
		else if (state_ == GameState::kMainMenu)
		{
			main_menu_.update(dt);
		}

		if (viewport_animation_.in_progress())
		{
			animate_game_viewport(dt);
		}
	}

	void Game::render()
	{
		// render menu on bottom
		if (state_ == GameState::kMainMenu)
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

	void Game::handle_game_viewport_action_impl(const Action reason)
	{
		switch (reason)
		{
		case Action::kShowLevelSelection:
			open_level_selection(true);
			break;
		case Action::kReturnToMainMenu:
			open_main_menu();
			break;
		default:
			ASSERT(false, "Unknown reason for game end");
			break;
		}
	}

	void Game::change_level_impl(const LevelSelectionMenu::MenuIndex index)
	{
		ASSERT(index >= 0 && index < kMaxLevels, "Unexpected menu index");
		game_viewport_.load_level(kLevelPaths[index]);
		current_level_ = index;
	}

	void Game::start_game_impl()
	{
		game_viewport_.start_game();
		close_main_menu();
	}

	// TODO(sasiala): show & hide should not change state
	void Game::show_level_preview_impl()
	{
		show_small_game_viewport(false);
	}
	
	void Game::hide_level_preview_impl()
	{
		game_viewport_.deactivate();
		state_ = GameState::kMainMenu;
	}

	void Game::open_main_menu()
	{
		game_viewport_.deactivate();
		main_menu_.activate();
		state_ = GameState::kMainMenu;
	}

	void Game::open_level_selection(const bool animate)
	{
		main_menu_.update_current_level(current_level_);
		main_menu_.open_level_selection_next_activate();
		main_menu_.activate();
		state_ = GameState::kMainMenu;
		show_small_game_viewport(animate);
	}

	void Game::show_small_game_viewport(const bool animate)
	{
		const auto target_pos = glm::vec2{ .45 * width_, .25 * height_ };
		const auto target_width = static_cast<Dimension>(.5 * width_);
		const auto target_height = static_cast<Dimension>(.5 * height_);
		if (animate)
		{
			const float dw = viewport_animation_dw();
			const float dh = viewport_animation_dh();
			const auto start_pos = glm::vec2{ 0.0f, 0.0f };
			viewport_animation_ = { true, -dw, -dh, target_pos, start_pos, target_width, target_height, width_, height_ };
		}
		else
		{
			game_viewport_.set_size(target_width, target_height);
			game_viewport_.set_position(target_pos);
		}
		game_viewport_.activate();
	}

	void Game::close_main_menu()
	{
		main_menu_.deactivate();
		
		const float dw = viewport_animation_dw();
		const float dh = viewport_animation_dh();
		const auto start_pos = glm::vec2{ .45 * width_, .25 * height_ };
		const auto end_pos = glm::vec2{ 0.0f, 0.0f };
		viewport_animation_ = { true, dw, dh, end_pos, start_pos, width_, height_, static_cast<Dimension>(.5 * width_), static_cast<Dimension>(.5 * height_) };
		state_ = GameState::kActive;
		game_viewport_.activate();
	}

	void Game::render_menu()
	{
		main_menu_.render(sprite_renderer_);
	}

	void Game::animate_game_viewport(const float dt)
	{
		ASSERT(viewport_animation_.in_progress(), "No animation in progress");

		viewport_animation_.update(dt);
		game_viewport_.set_size(viewport_animation_.width(), viewport_animation_.height());
		game_viewport_.set_position(viewport_animation_.position());
	}

	void Game::ViewportAnimation::update(const float dt)
	{
		ASSERT(animation_in_progress_, "No animation in progress");

		current_width_ = animate_dimension(current_width_,
			target_width_, static_cast<int>(dt * dw_));
		current_height_ = animate_dimension(current_height_,
			target_height_, static_cast<int>(dt * dh_));

		const auto x_complete = (current_width_ == target_width_);
		const auto y_complete = (current_height_ == target_height_);
		if (x_complete && y_complete)
		{
			animation_in_progress_ = false;
		}
		if (x_complete)
		{
			dw_ = 0;
		}
		if (y_complete)
		{
			dh_ = 0;
		}

		// update position
		const auto pos_x = slope_pos_x_ * current_width_ + offset_pos_x_;
		const auto pos_y = slope_pos_y_ * current_height_ + offset_pos_y_;
		current_position_ = { pos_x, pos_y };
	}

} // namespace util