#include "game_viewport.h"

#include "audio_manager.h"
#include "ball_object.h"
#include "array_helpers.h"
#include "logging.h"
#include "gl_debug.h"
#include "particle_generator.h"
#include "post_processor.h"

#include <algorithm>

namespace util {
	GameViewport::GameViewport(IResetGlProperties &gl_property_resetter,
							   Dimension width, 
							   Dimension height)
		: Element{false}
		, keys_pressed_{}
		, keys_processed_{}
		, gl_property_resetter_{ gl_property_resetter }
		, width_{ width }
		, height_{ height }
		, game_state_callback_{ nullptr }
		, level_{}
		, level_path_{ nullptr }
		, background_texture_id_{}
		, block_texture_id_{}
		, block_solid_texture_id_{}
		, paddle_texture_id_{}
		, pup_chaos_texture_id_{}
		, pup_confuse_texture_id_{}
		, pup_size_texture_id_{}
		, pup_pass_through_texture_id_{}
		, pup_speed_texture_id_{}
		, pup_sticky_texture_id_{}
		, particle_shader_id_{}
		, effects_shader_id_{}
		, sprite_shader_id_{}
		, font_shader_id_{}
		, default_font_id_{}
		, state_{State::kUnknown}
		, lives_{ kInitialLifeCount }
		, paddle_{ nullptr }
		, ball_{ nullptr }
		, particle_generator_{ nullptr }
		, effects_{ nullptr }
		, shake_time_{ 0.0f }
		, power_ups_{}
	{
	}

	void GameViewport::set_size(Dimension width, Dimension height)
	{
		// TODO(sasiala): should make separation between loaded height/width (width_ & height_)
		// and render height/width (set here) very clear.  ball, paddle, brick size should all remain
		// the same since being loaded, but the rendered width and height can change (which will just
		// stretch the texture)
		effects_->set_size(width, height);
	}

	void GameViewport::set_position(const glm::vec2 &position)
	{
		effects_->set_position(position);
	}

	void GameViewport::load_level(const char * const path)
	{
		// TODO(sasiala) this clearing should be acomplished by reset()
		// call, but that currently calls load_level()
		reset_lives();
		power_ups_.clear();
		effects_->clear_effects();
		particle_generator_->clear_particles();
		reset_player();

		level_path_ = path;
		level_.load(level_path_, width_, height_ / 2,
			block_solid_texture_id_, block_texture_id_);
		state_ = State::kBefore;
	}

	void GameViewport::reset_level()
	{
		// TODO(sasiala): add reset to level class
		load_level(level_path_);
	}

	void GameViewport::reset()
	{
		reset_lives();
		reset_level();
		power_ups_.clear();
		effects_->clear_effects();
		particle_generator_->clear_particles();
		reset_player();
	}

	void GameViewport::initialize_impl(const glm::mat4 &screen_projection)
	{
		// textures
		background_texture_id_ = ResourceManager::load_texture(kBackgroundImagePath, false);
		block_texture_id_ = ResourceManager::load_texture(kBlockImagePath, false);
		block_solid_texture_id_ = ResourceManager::load_texture(kBlockSolidImagePath, false);
		paddle_texture_id_ = ResourceManager::load_texture(kPaddleImagePath, true);
		ball_texture_id_ = ResourceManager::load_texture(kBallImagePath, true);
		particle_texture_id_ = ResourceManager::load_texture(kParticleImagePath, true);

		pup_chaos_texture_id_ = ResourceManager::load_texture(kPupChaosImagePath, true);
		pup_confuse_texture_id_ = ResourceManager::load_texture(kPupConfuseImagePath, true);
		pup_size_texture_id_ = ResourceManager::load_texture(kPupIncreaseImagePath, true);
		pup_pass_through_texture_id_ = ResourceManager::load_texture(kPupPassThroughImagePath, true);
		pup_speed_texture_id_ = ResourceManager::load_texture(kPupSpeedImagePath, true);
		pup_sticky_texture_id_ = ResourceManager::load_texture(kPupStickyImagePath, true);

		// shaders
		particle_shader_id_ = ResourceManager::load_shader("shaders/particle.vs", "shaders/particle.fs", {util::nullopt});
		effects_shader_id_ = ResourceManager::load_shader("shaders/effects.vs", "shaders/effects.fs", {util::nullopt});
		sprite_shader_id_ = ResourceManager::load_shader("shaders/sprite.vs", "shaders/sprite.fs", {util::nullopt});
		font_shader_id_ = ResourceManager::load_shader("shaders/text_2d.vs", "shaders/text_2d.fs", {util::nullopt});

		// fonts
		default_font_id_ = ResourceManager::load_font(kDefaultFontPath, font_shader_id_, kDefaultFontSize, width_, height_);
		const auto paddle_size = paddle_size_from_viewport_size();
		const auto player_pos = glm::vec2(
			width_ / 2.0f - paddle_size.x / 2.0f,
			height_ - paddle_size.y
		);
		paddle_ = new GameObject(player_pos, paddle_size,
			ResourceManager::get_texture(paddle_texture_id_), {}, {});

		const auto ball_radius = ball_radius_from_viewport_width();
		const auto ball_pos = player_pos +
			glm::vec2(paddle_->size().x / 2.0f - ball_radius,
				-ball_radius * 2.0f);
		const auto ball_velocity = initial_ball_velocity();
		ball_ = new BallObject(ball_pos, ball_radius, ball_velocity,
			ResourceManager::get_texture(ball_texture_id_));

		glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(width_),
			static_cast<float>(height_), 0.0f, -1.0f, 1.0f);

		particle_generator_ = new ParticleGenerator(
			ResourceManager::get_shader(particle_shader_id_),
			ResourceManager::get_texture(particle_texture_id_),
			kMaxParticles,
			projection
		);

		effects_ = new PostProcessor(
			gl_property_resetter_,
			ResourceManager::get_shader(effects_shader_id_),
			{ 0.0f, 0.0f },
			width_,
			height_,
			screen_projection
		);

		sprite_renderer_ = new SpriteRenderer{ ResourceManager::get_shader(sprite_shader_id_) };
		auto &sprite_shader = ResourceManager::get_shader(sprite_shader_id_);
		sprite_shader.use();
		sprite_shader.set_int("u_image_", 0, false);
		sprite_shader.set_mat4("u_projection_", projection, false);
	}

	void GameViewport::update_impl(Time dt)
	{
		ASSERT(state_ < State::kNumStates, "Invalid game viewport state");

		if (!is_active() || state_ != State::kPlaying)
		{
			return;
		}

		ASSERT(ball_, "No ball defined");
		ASSERT(particle_generator_, "No particle generator defined");

		ball_->move(dt, width_);
		check_collisions();

		if (state_ == State::kLost)
		{
			reset();
			return;
		}
		if (state_ == State::kWon)
		{
			return;
		}

		if (ball_->position().y >= height_)
		{
			kill_player();
		}

		particle_generator_->update(dt, *ball_, kNewParticlesPerUpdate, glm::vec2(ball_->radius() / 2.0f));

		update_power_ups(dt);

		if (shake_time_ > 0.0f)
		{
			shake_time_ -= dt;
			if (shake_time_ <= 0.0f)
			{
				effects_->set_shake(false);
			}
		}
	}

	void GameViewport::activate_impl()
	{
		fill(keys_pressed_, false);
		fill(keys_processed_, false);
	}

	void GameViewport::deactivate_impl()
	{
	}

	void GameViewport::render_impl(Optional<SpriteRenderer*> /*parent_sprite_renderer*/)
	{
		ASSERT(ball_, "No ball defined");
		ASSERT(paddle_, "No paddle defined");

		if (!is_active())
		{
			return;
		}

		effects_->begin_render();

		sprite_renderer_->draw(ResourceManager::get_texture(background_texture_id_),
			glm::vec2(0.0f, 0.0f), glm::vec2(width_, height_), 0.0f);

		level_.draw(*sprite_renderer_);
		paddle_->draw(*sprite_renderer_);
		particle_generator_->draw();
		ball_->draw(*sprite_renderer_);

		for (auto &i : power_ups_)
		{
			if (!i.is_destroyed())
			{
				i.draw(*sprite_renderer_);
			}
		}

		render_lives();

		effects_->end_render();
		effects_->render(static_cast<float>(glfwGetTime()));

		check_for_gl_errors();
	}

	namespace {
		GameViewport::Direction vector_direction(const glm::vec2 &target)
		{
			static const std::vector<std::pair<GameViewport::Direction, glm::vec2>> compass{
				{ GameViewport::Direction::kUp, glm::vec2(0.0f, 1.0f) },   // up
				{ GameViewport::Direction::kRight, glm::vec2(1.0f, 0.0f) },   // right
				{ GameViewport::Direction::kDown, glm::vec2(0.0f, -1.0f) },  // down
				{ GameViewport::Direction::kLeft, glm::vec2(-1.0f, 0.0f) }   // left
			};

			float max = 0.0f;
			auto best_match = GameViewport::Direction::kUnknown;

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

		bool check_collision(const GameObject &one, const GameObject &two)
		{
			bool collision_x = one.position().x + one.size().x >= two.position().x &&
				two.position().x + two.size().x >= one.position().x;
			bool collision_y = one.position().y + one.size().y >= two.position().y &&
				two.position().y + two.size().y >= one.position().y;
			return collision_x && collision_y;
		}

		GameViewport::Collision check_collision(const BallObject &one, const GameObject &two)
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
				return std::make_tuple(false, GameViewport::Direction::kUnknown, glm::vec2(0.0f, 0.0f));
			}
		}
	} // namespace

	void GameViewport::handle_ball_box_collision(const Collision &collision_tuple, const size_t box_index, const GameObject &box)
	{
		if (!box.is_solid())
		{
			level_.set_brick_destroyed(box_index, true);
			if (level_.is_completed())
			{
				level_complete();
			}

			const auto power_ups_spawned = spawn_power_ups(box);
			if (power_ups_spawned)
			{
				AudioManager::play_ball_brick_collision_sound(AudioManager::BallBrickCollisionType::kPowerUp);
			}
			else
			{
				AudioManager::play_ball_brick_collision_sound(AudioManager::BallBrickCollisionType::kNormal);
			}

			if (state_ != State::kPlaying)
			{
				return;
			}
		}
		else
		{
			shake_time_ = 0.05f;
			effects_->set_shake(true);
			AudioManager::play_ball_brick_collision_sound(AudioManager::BallBrickCollisionType::kSolid);
		}

		if (!ball_->pass_through())
		{
			const auto direction = std::get<1>(collision_tuple);
			const auto diff_vector = std::get<2>(collision_tuple);
			ASSERT(direction != GameViewport::Direction::kUnknown &&
				direction != GameViewport::Direction::kNumDirections,
				"Invalid collision direction");
			if (direction == GameViewport::Direction::kLeft ||
				direction == GameViewport::Direction::kRight)
			{
				auto velocity = ball_->velocity();
				velocity.x *= -1;
				ball_->set_velocity(velocity);

				// move outside of object
				float penetration = ball_->radius() - std::abs(diff_vector.x);
				if (direction == GameViewport::Direction::kLeft)
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
				if (direction == GameViewport::Direction::kUp)
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

	void GameViewport::activate_power_up(const PowerUp &power_up)
	{
		switch (power_up.type())
		{
		case static_cast<PowerUp::Type>(PowerUpTypes::kSpeed) :
			ball_->set_velocity(ball_->velocity() * 1.2f);
			break;
		case static_cast<PowerUp::Type>(PowerUpTypes::kSticky) :
			ball_->set_sticky(true);
			paddle_->set_color(glm::vec3(1.0f, 0.5f, 1.0f));
			break;
		case static_cast<PowerUp::Type>(PowerUpTypes::kPassThrough) :
			ball_->set_pass_through(true);
			ball_->set_color(glm::vec3(1.0f, 0.5f, 0.5f));
			break;
		case static_cast<PowerUp::Type>(PowerUpTypes::kPadSizeIncrease) :
			paddle_->set_size(paddle_->size() + glm::vec2(50.0f, 0.0f));
			break;
		case static_cast<PowerUp::Type>(PowerUpTypes::kConfuse) :
			if (!effects_->chaos())
			{
				effects_->set_confuse(true);
			}
																break;
		case static_cast<PowerUp::Type>(PowerUpTypes::kChaos) :
			if (!effects_->confuse())
			{
				effects_->set_chaos(true);
			}
		}
	}

	void GameViewport::check_collisions()
	{
		size_t index = 0;
		for (const auto &box : level_.bricks())
		{
			if (!box.is_destroyed())
			{
				auto collision_tuple = check_collision(*ball_, box);

				if (std::get<0>(collision_tuple))
				{
					handle_ball_box_collision(collision_tuple, index, box);
					if (state_ != State::kPlaying)
					{
						return;
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
			new_velocity.x = initial_ball_velocity().x * percentage * strength;
			// new_velocity.y = -old_velocity.y;
			// assuming that collision is always with top of paddle prevents sticky issue
			new_velocity.y = -1 * abs(old_velocity.y);
			new_velocity = glm::normalize(new_velocity) * glm::length(old_velocity);
			ball_->set_velocity(new_velocity);
			ball_->set_stuck(ball_->sticky());
		}

		for (auto &power_up : power_ups_)
		{
			if (!power_up.is_destroyed())
			{
				if (power_up.position().y >= height_)
				{
					power_up.set_destroyed(true);
				}
				if (check_collision(*paddle_, power_up))
				{
					activate_power_up(power_up);
					power_up.set_destroyed(true);
					power_up.set_activated(true);
				}
			}
		}
	}

	void GameViewport::set_key_impl(KeyId key_id, bool val)
	{
		if (!is_active())
		{
			return;
		}

		bool *key_ptr = nullptr;
		bool *key_processed_ptr = nullptr;
		switch (convert_id(key_id))
		{
		case ButtonsHandled::kLeftButton:
		case ButtonsHandled::kRightButton:
		case ButtonsHandled::kLaunchButton:
			key_ptr = &keys_pressed_[to_index(convert_id(key_id))];
			key_processed_ptr = &keys_processed_[to_index(convert_id(key_id))];
			break;
		default:
			LOG("Unhandled key ID");
			return;
		}

		// update keys & processed arrays accordingly
		*key_ptr = val;
		if (!val)
		{
			*key_processed_ptr = false;
		}
	}

	void GameViewport::handle_left_button(float dt)
	{
		ASSERT(ball_, "No ball defined");
		ASSERT(paddle_, "No paddle defined");

		const auto velocity = paddle_velocity_from_viewport_width() * dt;
		if (paddle_->position().x >= 0.0f)
		{
			paddle_->move_x(-velocity);

			if (ball_->stuck())
			{
				ball_->move_x(-velocity);
			}
		}
	}

	void GameViewport::handle_right_button(float dt)
	{
		ASSERT(ball_, "No ball defined");
		ASSERT(paddle_, "No paddle defined");

		const auto velocity = paddle_velocity_from_viewport_width() * dt;
		if (paddle_->position().x <= width_ - paddle_->size().x)
		{
			paddle_->move_x(velocity);

			if (ball_->stuck())
			{
				ball_->move_x(velocity);
			}
		}
	}

	void GameViewport::handle_launch_button()
	{
		ASSERT(ball_, "No ball defined");

		ball_->set_stuck(false);
	}

	void GameViewport::process_input_impl(float dt)
	{
		if (!is_active())
		{
			return;
		}

		if (keys_pressed_[to_index(convert_id(GLFW_KEY_A))])
		{
			handle_left_button(dt);
			keys_processed_[to_index(convert_id(GLFW_KEY_A))] = true;
		}
		if (keys_pressed_[to_index(convert_id(GLFW_KEY_D))])
		{
			handle_right_button(dt);
			keys_processed_[to_index(convert_id(GLFW_KEY_D))] = true;
		}
		if (keys_pressed_[to_index(convert_id(GLFW_KEY_SPACE))])
		{
			handle_launch_button();
			keys_processed_[to_index(convert_id(GLFW_KEY_SPACE))] = true;
		}
	}

	void GameViewport::render_lives()
	{
		ResourceManager::get_font(default_font_id_).render_text("Lives: " + 
			std::to_string(lives_), 
			convert_ratio_from_width(kLifeText.relative_x_), 
			convert_ratio_from_height(kLifeText.relative_y_), 
			convert_ratio_from_height(kLifeText.scale_ratio_from_height_), 
			{ glm::vec3{1.0f, 1.0f, 1.0f} });
	}

	void GameViewport::reset_player()
	{
		const auto paddle_size = paddle_size_from_viewport_size();
		paddle_->set_size(paddle_size);
		paddle_->set_position(glm::vec2(width_ / 2.0f - paddle_size.x / 2.0f,
			height_ - paddle_size.y));

		const auto ball_radius = ball_radius_from_viewport_width();
		const auto ball_velocity = initial_ball_velocity();
		ball_->reset(paddle_->position() + glm::vec2(paddle_->size().x / 2.0f - ball_radius, 
												-(ball_radius * 2.0f)), ball_velocity);
	}

	void GameViewport::kill_player()
	{
		ASSERT(lives_ > 0, "No lives to lose");

		--lives_;
		if (out_of_lives())
		{
			reset();
			game_state_callback_->game_ended(GameStateCallback::EndingReason::kLost);
			state_ = State::kLost;
		}
		else
		{
			reset_player();
		}
	}

	void GameViewport::delete_dynamic_data()
	{
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

	namespace {
		// should be true ~ 1/chance
		bool should_spawn(unsigned int chance)
		{
			unsigned int random = rand() % chance;
			return random == 0;
		}

		// should_spawn_*() functions are just for clarifying code
		bool should_spawn_speed()
		{
			return should_spawn(75);
		}

		bool should_spawn_sticky()
		{
			return should_spawn(75);
		}

		bool should_spawn_pass_through()
		{
			return should_spawn(75);
		}

		bool should_spawn_size()
		{
			return should_spawn(75);
		}

		bool should_spawn_confuse()
		{
			return should_spawn(15);
		}

		bool should_spawn_chaos()
		{
			return should_spawn(15);
		}
	} // namespace

	bool GameViewport::spawn_power_ups(const GameObject &block)
	{
		auto power_up_spawned = false;
		ASSERT(!block.is_solid(), "Don't spawn power ups for solid blocks");
		if (should_spawn_speed())
		{
			power_ups_.push_back(
				PowerUp(static_cast<PowerUp::Type>(PowerUpTypes::kSpeed),
					glm::vec3(0.5f, 0.5f, 1.0f), 0.0f, block.position(),
					ResourceManager::get_texture(pup_speed_texture_id_))
			);
			power_up_spawned = true;
		}

		if (should_spawn_sticky())
		{
			power_ups_.push_back(
				PowerUp(static_cast<PowerUp::Type>(PowerUpTypes::kSticky),
					glm::vec3(1.0f, 0.5f, 1.0f), 20.0f, block.position(),
					ResourceManager::get_texture(pup_sticky_texture_id_))
			);
			power_up_spawned = true;
		}

		if (should_spawn_pass_through())
		{
			power_ups_.push_back(
				PowerUp(static_cast<PowerUp::Type>(PowerUpTypes::kPassThrough),
					glm::vec3(0.5f, 1.0f, 0.5f), 10.0f, block.position(),
					ResourceManager::get_texture(pup_pass_through_texture_id_))
			);
			power_up_spawned = true;
		}

		if (should_spawn_size())
		{
			power_ups_.push_back(
				PowerUp(static_cast<PowerUp::Type>(PowerUpTypes::kPadSizeIncrease),
					glm::vec3(1.0f, 0.6f, 0.4f), 0.0f, block.position(),
					ResourceManager::get_texture(pup_size_texture_id_))
			);
			power_up_spawned = true;
		}

		if (should_spawn_confuse())
		{
			power_ups_.push_back(
				PowerUp(static_cast<PowerUp::Type>(PowerUpTypes::kConfuse),
					glm::vec3(1.0f, 0.3f, 0.3f), 15.0f, block.position(),
					ResourceManager::get_texture(pup_confuse_texture_id_))
			);
			power_up_spawned = true;
		}

		if (should_spawn_chaos())
		{
			power_ups_.push_back(
				PowerUp(static_cast<PowerUp::Type>(PowerUpTypes::kChaos),
					glm::vec3(0.9f, 0.25f, 0.25f), 15.0f, block.position(),
					ResourceManager::get_texture(pup_chaos_texture_id_))
			);
			power_up_spawned = true;
		}

		return power_up_spawned;
	}

	bool GameViewport::is_other_power_up_active(const PowerUpTypes type)
	{
		for (auto &i : power_ups_)
		{
			if (i.activated() && static_cast<PowerUp::Type>(type) == i.type())
			{
				return true;
			}
		}
		return false;
	}

	void GameViewport::update_power_ups(float dt)
	{
		for (auto &i : power_ups_)
		{
			i.progress_time(dt);

			if (i.activated())
			{
				i.set_duration(i.duration() - dt);
				if (i.duration() <= 0.0f)
				{
					i.set_activated(false);
					switch (i.type())
					{
					case static_cast<PowerUp::Type>(PowerUpTypes::kSticky) :
						if (!is_other_power_up_active(PowerUpTypes::kSticky))
						{
							ball_->set_sticky(false);
							paddle_->set_color(glm::vec3(1.0f));
						}
						break;
					case static_cast<PowerUp::Type>(PowerUpTypes::kPassThrough) :
						if (!is_other_power_up_active(PowerUpTypes::kPassThrough))
						{
							ball_->set_pass_through(false);
							ball_->set_color(glm::vec3(1.0f));
						}
						break;
					case static_cast<PowerUp::Type>(PowerUpTypes::kConfuse) :
						if (!is_other_power_up_active(PowerUpTypes::kConfuse))
						{
							effects_->set_confuse(false);
						}
						break;
					case static_cast<PowerUp::Type>(PowerUpTypes::kChaos) :
						if (!is_other_power_up_active(PowerUpTypes::kChaos))
						{
							effects_->set_chaos(false);
						}
					}
				}
			}
		}

		power_ups_.erase(std::remove_if(power_ups_.begin(), power_ups_.end(),
			[](const PowerUp &power_up) {return power_up.is_destroyed() && !power_up.activated(); }),
			power_ups_.end());
	}

	void GameViewport::level_complete()
	{
		// TODO(sasiala): I don't think we want a reset here, but rather a game won state
		game_state_callback_->game_ended(GameStateCallback::EndingReason::kWon);
		state_ = State::kWon;
	}

}