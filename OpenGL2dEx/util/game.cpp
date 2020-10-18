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
			if (keys_[GLFW_KEY_ENTER] && !keys_processed_[GLFW_KEY_ENTER])
			{
				state_ = GameState::kActive;
				close_main_menu();
				keys_processed_[GLFW_KEY_ENTER] = true;
			}
			else
			{
				if (keys_[GLFW_KEY_W] && !keys_processed_[GLFW_KEY_W])
				{
					current_level_ = (current_level_ + 1) % kMaxLevels;
					load_current_level();
					keys_processed_[GLFW_KEY_W] = true;
				}
				if (keys_[GLFW_KEY_S] && !keys_processed_[GLFW_KEY_S])
				{
					if (current_level_ > 0)
					{
						--current_level_;
					}
					else
					{
						// wrap around
						current_level_ = kMaxLevels - 1;
					}
					load_current_level();
					keys_processed_[GLFW_KEY_S] = true;
				}
			}
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
	}

	void Game::game_ended_impl(const EndingReason /*reason*/)
	{
		// TODO(sasiala): handle various ending reasons
		state_ = GameState::kMenu;
		open_main_menu();
	}

	void Game::open_main_menu()
	{
		main_menu_.activate("MAIN MENU", "LEVELS", Menu::OptionList{ "A", "B", "C" });
		game_viewport_.set_size(.5 * width_, .5 * height_);
		game_viewport_.set_position({ .45 * width_, .25 * height_ });
	}

	void Game::close_main_menu()
	{
		game_viewport_.set_size(width_, height_);
		game_viewport_.set_position({ 0.0f, 0.0f });
		main_menu_.deactivate();
	}

	void Game::render_menu()
	{
		main_menu_.render(sprite_renderer_);
	}

} // namespace util