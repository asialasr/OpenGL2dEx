#ifndef GAME_H
#define GAME_H

#include "game_level.h"
#include "particle_generator.h"
#include "post_processor.h"
#include "power_up.h"
#include "resource_mgr.h"
#include "sprite_renderer.h"
#include "count_of.h"

#include <vector>
#include <tuple>

namespace util {

	class BallObject;
	class ParticleGenerator;

	class Game
	{
	public:
		enum class Direction {
			kUp,
			kDown,
			kLeft,
			kRight,
			kNumDirections,
			kUnknown,
		};
		using Dimension = unsigned int;
		using Collision = std::tuple<bool, Direction, glm::vec2>;
		using LifeCount = unsigned int;

		Game(Dimension width, Dimension height);
		~Game();

		void initialize();

		void process_input(float dt);
		void update(float dt);
		void render();

		static constexpr size_t kNumKeys{ 1024 };
		void set_key(size_t key, bool val);

		enum class GameState {
			kActive,
			kMenu,
			kWin,
			kUnknown,
			kNumStates,
		}; // enum class GameState
		GameState state() const
		{
			return state_;
		}

	private:
		void render_menu() const;

		void handle_ball_box_collision(const Collision &collision_tuple, const size_t box_index, const GameObject &box);
		void activate_power_up(const PowerUp &power_up);
		void check_collisions();

		enum class PowerUpTypes : PowerUp::Type {
			kSpeed,
			kSticky,
			kPassThrough,
			kPadSizeIncrease,
			kConfuse,
			kChaos,
			kNumTypes,
			kUnknown,
		};

		void reset_level();
		void reset_player();

		bool out_of_lives() const
		{
			return lives_ <= 0;
		}

		static constexpr LifeCount kInitialLifeCount{ 3 };
		void reset_lives()
		{
			lives_ = kInitialLifeCount;
		}

		void kill_player();

		static constexpr struct {
			float x_;
			float y_;
			float scale_;
		} kLifeText{ 5.0f, 5.0f, 1.0f };
		void render_lives();

		void delete_dynamic_data();

		bool spawn_power_ups(const GameObject &block);
		bool is_other_power_up_active(const PowerUpTypes type);
		void update_power_ups(float dt);

		static constexpr const char *kLevelPaths[] = {
			"levels/one.lvl",
			"levels/two.lvl",
			"levels/three.lvl",
			"levels/four.lvl"
		};
		static constexpr size_t kMaxLevels{ util::count_of(kLevelPaths) };

		static constexpr const char *kBackgroundImagePath = "textures/background.jpg";
		static constexpr const char *kSmileyImagePath = "textures/awesomeface.png";
		static constexpr const char *kBlockSolidImagePath = "textures/block_solid.png";
		static constexpr const char *kBlockImagePath = "textures/block.png";
		static constexpr const char *kPaddleImagePath = "textures/paddle.png";
		static constexpr const char *kBallImagePath = "textures/awesomeface.png";
		static constexpr const char *kParticleImagePath = "textures/particle.png";

		static constexpr const char *kPupChaosImagePath = "textures/power_ups/powerup_chaos.png";
		static constexpr const char *kPupConfuseImagePath = "textures/power_ups/powerup_confuse.png";
		static constexpr const char *kPupIncreaseImagePath = "textures/power_ups/powerup_increase.png";
		static constexpr const char *kPupPassThroughImagePath = "textures/power_ups/powerup_passthrough.png";
		static constexpr const char *kPupSpeedImagePath = "textures/power_ups/powerup_speed.png";
		static constexpr const char *kPupStickyImagePath = "textures/power_ups/powerup_sticky.png";

		GameState state_;
		bool      keys_[kNumKeys];
		bool      keys_processed_[kNumKeys];
		Dimension width_, height_;

		SpriteRenderer            *sprite_renderer_;
		ResourceManager::ShaderId sprite_shader_id_;
		ResourceManager::ShaderId particle_shader_id_;
		ResourceManager::ShaderId effects_shader_id_;
		ResourceManager::ShaderId font_shader_id_;

		// textures
		ResourceManager::Texture2DId background_texture_id_;
		ResourceManager::Texture2DId smiley_texture_id_;
		ResourceManager::Texture2DId block_texture_id_;
		ResourceManager::Texture2DId block_solid_texture_id_;
		ResourceManager::Texture2DId paddle_texture_id_;
		ResourceManager::Texture2DId ball_texture_id_;
		ResourceManager::Texture2DId particle_texture_id_;

		ResourceManager::Texture2DId pup_chaos_texture_id_;
		ResourceManager::Texture2DId pup_confuse_texture_id_;
		ResourceManager::Texture2DId pup_size_texture_id_;
		ResourceManager::Texture2DId pup_pass_through_texture_id_;
		ResourceManager::Texture2DId pup_speed_texture_id_;
		ResourceManager::Texture2DId pup_sticky_texture_id_;
		
		// fonts
		static constexpr const char *kDefaultFontPath = "fonts/OCRAEXT.TTF";

		static constexpr TextRenderer::FontSize kDefaultFontSize{ 24 };

		ResourceManager::FontId default_font_id_;

		std::vector<GameLevel> levels_;
		size_t                 current_level_;

		LifeCount lives_;

		// TODO(sasiala): what's the deal with this not being able
		// to be constexpr and/or static?
		const glm::vec2 kPlayerSize = glm::vec2( 100.0f, 20.0f );
		static constexpr float kPlayerVelocity{ 500.0f };
		GameObject *paddle_;

		const glm::vec2 kInitialBallVelocity{ 100.0f, -350.0f };
		static constexpr float kBallRadius{ 12.5f };
		BallObject *ball_;

		static constexpr size_t kMaxParticles{ 500 };
		static constexpr size_t kNewParticlesPerUpdate{ 2 };
		ParticleGenerator *particle_generator_;

		PostProcessor *effects_;
		float shake_time_;

		std::vector<PowerUp> power_ups_;
	}; // class Game

} // namespace util

#endif // GAME_H