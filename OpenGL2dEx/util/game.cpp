#include "game.h"

#include "logging.h"
#include "gl_debug.h"

#include <GLFW/glfw3.h>

namespace util
{
	Game::Game(Dimension width, Dimension height) 
		: state_{ GameState::kActive }
		, keys_{}
		, width_{ width }
		, height_{ height }
		, sprite_renderer_{ nullptr }
		, sprite_shader_id_{}
		, background_texture_id_{}
		, smiley_texture_id_{}
		, block_texture_id_{}
		, block_solid_texture_id_{}
		, paddle_texture_id_{}
		, levels_{}
		, current_level_{0}
		, paddle_{}
	{
	}

	Game::~Game()
	{
		if (sprite_renderer_)
		{
			delete sprite_renderer_;
		}
		sprite_renderer_ = nullptr;

		if (paddle_)
		{
			delete paddle_;
		}
		paddle_ = nullptr;
	}

	void Game::initialize()
	{
		if (sprite_renderer_)
		{
			delete sprite_renderer_;
		}
		sprite_renderer_ = nullptr;

		sprite_shader_id_ = ResourceManager::load_shader("shaders/sprite.vs", "shaders/sprite.fs", {});
		
		glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(width_),
			static_cast<float>(height_), 0.0f, -1.0f, 1.0f);
		ResourceManager::get_shader(sprite_shader_id_).use();
		ResourceManager::get_shader(sprite_shader_id_).set_int("u_image_", 0, false);
		ResourceManager::get_shader(sprite_shader_id_).set_mat4("u_projection_", projection, false);

		sprite_renderer_ = new SpriteRenderer{ ResourceManager::get_shader(sprite_shader_id_) };

		background_texture_id_ = ResourceManager::load_texture(kBackgroundImagePath, false);
		smiley_texture_id_ = ResourceManager::load_texture(kSmileyImagePath, true);
		block_texture_id_ = ResourceManager::load_texture(kBlockImagePath, false);
		block_solid_texture_id_ = ResourceManager::load_texture(kBlockSolidImagePath, false);
		paddle_texture_id_ = ResourceManager::load_texture(kPaddleImagePath, true);

		auto one = GameLevel{};
		one.load(kLevelOnePath, width_, height_ / 2,
			block_solid_texture_id_, block_texture_id_);

		auto two = GameLevel{};
		two.load(kLevelTwoPath, width_, height_ / 2,
			block_solid_texture_id_, block_texture_id_);

		auto three = GameLevel{};
		three.load(kLevelThreePath, width_, height_ / 2,
			block_solid_texture_id_, block_texture_id_);

		auto four = GameLevel{};
		four.load(kLevelFourPath, width_, height_ / 2,
			block_solid_texture_id_, block_texture_id_);

		levels_.push_back(one);
		levels_.push_back(two);
		levels_.push_back(three);
		levels_.push_back(four);
		current_level_ = 0;

		auto player_pos = glm::vec2(
			width_ / 2.0f - kPlayerSize.x / 2.0f,
			height_ - kPlayerSize.y
		);
		paddle_ = new GameObject(player_pos, kPlayerSize, ResourceManager::get_texture(paddle_texture_id_), {}, {});

		check_for_gl_errors();
	}

	void Game::process_input(float dt)
	{
		if (GameState::kActive == state_)
		{
			auto velocity = kPlayerVelocity * dt;
			if (keys_[GLFW_KEY_A])
			{
				if (paddle_->position().x >= 0.0f)
				{
					paddle_->move_x(-velocity);
				}
			}
			if (keys_[GLFW_KEY_D])
			{
				if (paddle_->position().x <= width_ - paddle_->size().x)
				{
					paddle_->move_x(velocity);
				}
			}
		}
	}

	void Game::update(float /*dt*/)
	{

	}

	void Game::render()
	{
		if (GameState::kActive == state_)
		{
			sprite_renderer_->draw(ResourceManager::get_texture(background_texture_id_),
				glm::vec2(0.0f, 0.0f), glm::vec2(width_, height_), 0.0f);

			levels_.at(current_level_).draw(*sprite_renderer_);
			paddle_->draw(*sprite_renderer_);

			check_for_gl_errors();
		}
		//sprite_renderer_->draw(ResourceManager::get_texture(smiley_texture_id_),
		//	glm::vec2(200.0f, 200.0f), glm::vec2(300.0f, 400.0f), 45.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	}

	void Game::set_key(size_t key, bool val)
	{
		ASSERT(key < kNumKeys, "Key out of bounds");
		keys_[key] = val;
	}

} // namespace util