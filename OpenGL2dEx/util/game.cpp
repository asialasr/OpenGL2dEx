#include "game.h"

#include "ball_object.h"
#include "logging.h"
#include "gl_debug.h"
#include "particle_generator.h"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace util
{
	Game::Game(Dimension width, Dimension height) 
		: state_{ GameState::kActive }
		, keys_{}
		, width_{ width }
		, height_{ height }
		, sprite_renderer_{ nullptr }
		, sprite_shader_id_{}
		, particle_shader_id_{}
		, effects_shader_id_{}
		, background_texture_id_{}
		, smiley_texture_id_{}
		, block_texture_id_{}
		, block_solid_texture_id_{}
		, paddle_texture_id_{}
		, levels_{}
		, current_level_{0}
		, paddle_{ nullptr }
		, ball_{ nullptr }
		, particle_generator_{ nullptr }
		, effects_{ nullptr }
		, shake_time_{ 0.0f }
	{
	}

	void Game::delete_dynamic_data()
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

		if (ball_)
		{
			delete ball_;
		}
		ball_ = nullptr;

		if (particle_generator_)
		{
			delete particle_generator_;
		}
		particle_generator_ = nullptr;

		if (effects_)
		{
			delete effects_;
		}
		effects_ = nullptr;
	}

	Game::~Game()
	{
		delete_dynamic_data();
	}

	void Game::initialize()
	{
		delete_dynamic_data();

		sprite_shader_id_ = ResourceManager::load_shader("shaders/sprite.vs", "shaders/sprite.fs", {});
		particle_shader_id_ = ResourceManager::load_shader("shaders/particle.vs", "shaders/particle.fs", {});
		effects_shader_id_ = ResourceManager::load_shader("shaders/effects.vs", "shaders/effects.fs", {});

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
		ball_texture_id_ = ResourceManager::load_texture(kBallImagePath, true);
		particle_texture_id_ = ResourceManager::load_texture(kParticleImagePath, true);

		for (size_t i = 0; i < kMaxLevels; ++i)
		{
			auto level = GameLevel{};
			level.load(kLevelPaths[i], width_, height_ / 2,
				       block_solid_texture_id_, block_texture_id_);
			levels_.push_back(level);
		}

		current_level_ = 0;

		auto player_pos = glm::vec2(
			width_ / 2.0f - kPlayerSize.x / 2.0f,
			height_ - kPlayerSize.y
		);
		paddle_ = new GameObject(player_pos, kPlayerSize, 
			ResourceManager::get_texture(paddle_texture_id_), {}, {});

		auto ball_pos = player_pos + 
						glm::vec2(kPlayerSize.x / 2.0f - kBallRadius, 
								  -kBallRadius * 2.0f);
		ball_ = new BallObject(ball_pos, kBallRadius, kInitialBallVelocity, 
			ResourceManager::get_texture(ball_texture_id_));
		
		particle_generator_ = new ParticleGenerator(
			ResourceManager::get_shader(particle_shader_id_),
			ResourceManager::get_texture(particle_texture_id_),
			kMaxParticles,
			projection
		);

		effects_ = new PostProcessor(
			ResourceManager::get_shader(effects_shader_id_),
			width_,
			height_
		);

		check_for_gl_errors();
	}

	void Game::process_input(float dt)
	{
		ASSERT(ball_, "No ball defined");
		ASSERT(paddle_, "No paddle defined");

		if (GameState::kActive == state_)
		{
			auto velocity = kPlayerVelocity * dt;
			if (keys_[GLFW_KEY_A])
			{
				if (paddle_->position().x >= 0.0f)
				{
					paddle_->move_x(-velocity);

					if (ball_->stuck())
					{
						ball_->move_x(-velocity);
					}
				}
			}
			if (keys_[GLFW_KEY_D])
			{
				if (paddle_->position().x <= width_ - paddle_->size().x)
				{
					paddle_->move_x(velocity);

					if (ball_->stuck())
					{
						ball_->move_x(velocity);
					}
				}
			}
			if (keys_[GLFW_KEY_SPACE])
			{
				ball_->set_stuck(false);
			}
		}
	}

	void Game::reset_level()
	{
		ASSERT(kMaxLevels >= current_level_, "Invalid level");

		// TODO(sasiala): I think a "reset" function in level would be better
		levels_[current_level_].load(kLevelPaths[current_level_], 
			width_, height_ / 2, 
			block_solid_texture_id_, block_texture_id_);
	}

	void Game::reset_player()
	{
		paddle_->set_size(kPlayerSize);
		paddle_->set_position(glm::vec2(width_ / 2.0f - kPlayerSize.x / 2.0f,
			height_ - kPlayerSize.y));
		ball_->reset(paddle_->position() + glm::vec2(paddle_->size().x / 2.0f - kBallRadius, -(kBallRadius * 2.0f)), kInitialBallVelocity);
	}

	void Game::update(float dt)
	{
		ASSERT(ball_, "No ball defined");
		ASSERT(particle_generator_, "No particle generator defined");

		ball_->move(dt, width_);
		check_collisions();

		if (ball_->position().y >= height_)
		{
			reset_level();
			reset_player();
		}

		particle_generator_->update(dt, *ball_, kNewParticlesPerUpdate, glm::vec2(ball_->radius() / 2.0f));
		
		if (shake_time_ > 0.0f)
		{
			shake_time_ -= dt;
			if (shake_time_ <= 0.0f)
			{
				effects_->set_shake(false);
			}
		}
	}

namespace {
	/*
	bool check_collision(const GameObject &one, const GameObject &two)
	{
		const auto collision_x = (one.position().x + one.size().x) >= two.position().x &&
			(two.position().x + two.size().x) >= one.position().x;
		const auto collision_y = (one.position().y + one.size().y) >= two.position().y &&
			(two.position().y + two.size().y) >= one.position().y;

		return (collision_x && collision_y);
	}

	bool check_collision(const BallObject &one, const GameObject &two)
	{
		glm::vec2 center(one.position() + one.radius());

		glm::vec2 aabb_half_extents(two.size().x / 2.0f, two.size().y / 2.0f);
		glm::vec2 aabb_center(
			two.position().x + aabb_half_extents.x,
			two.position().y + aabb_half_extents.y
		);

		glm::vec2 difference = center - aabb_center;
		glm::vec2 clamped = glm::clamp(difference, -aabb_half_extents, aabb_half_extents);

		glm::vec2 closest = aabb_center + clamped;

		difference = closest - center;
		return glm::length(difference) < one.radius();
	}
	*/

	Game::Direction vector_direction(const glm::vec2 &target)
	{
		static const std::vector<std::pair<Game::Direction, glm::vec2>> compass{
			{ Game::Direction::kUp, glm::vec2(0.0f, 1.0f)},   // up
			{ Game::Direction::kRight, glm::vec2(1.0f, 0.0f)},   // right
			{ Game::Direction::kDown, glm::vec2(0.0f, -1.0f)},  // down
			{ Game::Direction::kLeft, glm::vec2(-1.0f, 0.0f)}   // left
		};
		
		float max = 0.0f;
		auto best_match = Game::Direction::kUnknown;

		for (auto i : compass)
		{
			float dot_product = glm::dot(glm::normalize(target), i.second);
			if (dot_product > max)
			{
				max = dot_product;
				best_match = i.first;
			}
		}
		return best_match;
	}

	Game::Collision check_collision(const BallObject &one, const GameObject &two)
	{
		glm::vec2 center(one.position() + one.radius());

		glm::vec2 aabb_half_extents(two.size().x / 2.0f, two.size().y / 2.0f);
		glm::vec2 aabb_center(
			two.position().x + aabb_half_extents.x,
			two.position().y + aabb_half_extents.y
		);

		glm::vec2 difference = center - aabb_center;
		glm::vec2 clamped = glm::clamp(difference, -aabb_half_extents, aabb_half_extents);

		glm::vec2 closest = aabb_center + clamped;

		difference = closest - center;

		if (glm::length(difference) <= one.radius())
		{
			return std::make_tuple(true, vector_direction(difference), difference);
		}
		else
		{
			return std::make_tuple(false, Game::Direction::kUnknown, glm::vec2(0.0f, 0.0f));
		}
	}
} // namespace

	void Game::check_collisions()
	{
		auto &current_level = levels_.at(current_level_);
		size_t index = 0;
		for (auto &box : current_level.bricks())
		{
			if (!box.is_destroyed())
			{
				auto collision_tuple = check_collision(*ball_, box);

				if (std::get<0>(collision_tuple))
				{
					if (!box.is_solid())
					{
						current_level.set_brick_destroyed(index, true);
					}
					else
					{
						shake_time_ = 0.05f;
						effects_->set_shake(true);
					}

					const auto direction = std::get<1>(collision_tuple);
					const auto diff_vector = std::get<2>(collision_tuple);
					ASSERT(direction != Game::Direction::kUnknown && 
						   direction != Game::Direction::kNumDirections, 
						   "Invalid collision direction");
					if (direction == Game::Direction::kLeft || 
						direction == Game::Direction::kRight)
					{
						auto velocity = ball_->velocity();
						velocity.x *= -1;
						ball_->set_velocity(velocity);

						// move outside of object
						float penetration = ball_->radius() - std::abs(diff_vector.x);
						if (direction == Game::Direction::kLeft)
						{
							ball_->move_x(penetration);
						}
						else
						{
							ball_->move_x(-penetration);
						}
					}
					else // vertical collision
					{
						auto velocity = ball_->velocity();
						velocity.y *= -1;
						ball_->set_velocity(velocity);

						// move outside of object
						float penetration = ball_->radius() - std::abs(diff_vector.y);
						if (direction == Game::Direction::kUp)
						{
							ball_->move_y(-penetration);
						}
						else
						{
							ball_->move_y(penetration);
						}
					}
				}
			}
			++index;
		}

		auto player_collision = check_collision(*ball_, *paddle_);
		if (!ball_->stuck() && std::get<0>(player_collision))
		{
			float center_board = paddle_->position().x + paddle_->size().x / 2.0f;
			float distance = (ball_->position().x + ball_->radius()) - center_board;
			float percentage = distance / (paddle_->size().x / 2.0f);

			float strength = 2.0f;
			glm::vec2 old_velocity = ball_->velocity();
			glm::vec2 new_velocity = ball_->velocity();
			new_velocity.x = kInitialBallVelocity.x * percentage * strength;
			// new_velocity.y = -old_velocity.y;
			// assuming that collision is always with top of paddle prevents sticky issue
			new_velocity.y = -1 * abs(old_velocity.y);
			new_velocity = glm::normalize(new_velocity) * glm::length(old_velocity);
			ball_->set_velocity(new_velocity);
		}
	}

	void Game::render()
	{
		ASSERT(ball_, "No ball defined");
		ASSERT(paddle_, "No paddle defined");

		if (GameState::kActive == state_)
		{
			effects_->begin_render();

			sprite_renderer_->draw(ResourceManager::get_texture(background_texture_id_),
				glm::vec2(0.0f, 0.0f), glm::vec2(width_, height_), 0.0f);

			levels_.at(current_level_).draw(*sprite_renderer_);
			paddle_->draw(*sprite_renderer_);
			particle_generator_->draw();
			ball_->draw(*sprite_renderer_);

			effects_->end_render();
			effects_->render(glfwGetTime());

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