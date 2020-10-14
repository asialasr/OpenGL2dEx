#ifndef GAME_VIEWPORT_H
#define GAME_VIEWPORT_H

#include "game_level.h"
#include "element.h"

#include "logging.h"
#include "power_up.h"
#include "types.h"

#include <utility>
#include <map>

#include <GLFW/glfw3.h>

namespace util {

class BallObject;
class ParticleGenerator;
class PostProcessor;
class IResetGlProperties;

class GameViewport : public Element {
public:
	enum class Direction {
		kUp,
		kDown,
		kLeft,
		kRight,
		kNumDirections,
		kUnknown,
	};
	using Collision = std::tuple<bool, Direction, glm::vec2>;
	using LifeCount = unsigned int;

	class GameStateCallback {
	public:
		enum class EndingReason {
			kWon = 0,
			kLost,
			kNumReasons,
			kUnknown,
		};
		void game_ended(const EndingReason reason)
		{
			game_ended_impl(reason);
		}

	private:
		virtual void game_ended_impl(EndingReason reason) = 0;
	};
	// TODO(sasiala): allow changing dimensions & pos dynamically
	GameViewport(IResetGlProperties &gl_property_resetter,
				 Dimension width, 
				 Dimension height);

	void set_game_state_callback(GameStateCallback &callback)
	{
		game_state_callback_ = &callback;
	}

	void load_level(const char * const path);
	void reset_level();
	void reset();

	auto lives() const
	{
		return lives_;
	}

private:
	// Element
	void initialize_impl(const glm::mat4 &screen_projection) override;
	void update_impl(Time dt) override;
	void render_impl(Optional<SpriteRenderer*> parent_sprite_renderer) override;

	enum class ButtonsHandled {
		kLeftButton = 0,
		kRightButton,
		kLaunchButton,
		kNumButtons,
		kUnknown,
	};
	size_t to_index(const ButtonsHandled button_id)
	{
		ASSERT(ButtonsHandled::kNumButtons > button_id, 
			"Cannot convert to index (out of bounds)");
		return static_cast<size_t>(button_id);
	}
	// TODO(sasiala): improve how key IDs are defined
	ButtonsHandled convert_id(const KeyId button)
	{
		auto converted_id = ButtonsHandled::kUnknown;
		switch (button)
		{
		case GLFW_KEY_A:
			converted_id = ButtonsHandled::kLeftButton;
			break;
		case GLFW_KEY_D:
			converted_id = ButtonsHandled::kRightButton;
			break;
		case GLFW_KEY_SPACE:
			converted_id = ButtonsHandled::kLaunchButton;
			break;
		default:
			LOG("Unknown button id: " + std::to_string(button));
			break;
		}

		return converted_id;
	}
	bool keys_pressed_[static_cast<size_t>(ButtonsHandled::kNumButtons)];
	bool keys_processed_[static_cast<size_t>(ButtonsHandled::kNumButtons)];


	void handle_ball_box_collision(const Collision &collision_tuple, const size_t box_index, const GameObject &box);
	void activate_power_up(const PowerUp &power_up);
	void check_collisions();

	void set_key_impl(KeyId key_id, bool val) override;
	void handle_left_button(float dt);
	void handle_right_button(float dt);
	void handle_launch_button();
	void process_input_impl(float dt) override;

	static constexpr LifeCount kInitialLifeCount{ 3 };
	void reset_lives()
	{
		lives_ = kInitialLifeCount;
	}
	bool out_of_lives() const
	{
		return lives_ <= 0;
	}

	void reset_player();
	void kill_player();

	void delete_dynamic_data();

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
	bool spawn_power_ups(const GameObject &block);
	bool is_other_power_up_active(const PowerUpTypes type);
	void update_power_ups(float dt);

	IResetGlProperties &gl_property_resetter_;
	Dimension width_;
	Dimension height_;

	GameStateCallback *game_state_callback_;

	GameLevel level_;
	const char * level_path_;

	glm::vec2 paddle_size_from_viewport_size() const 
	{
		return{ kRelativePaddleSize.x * width_, kRelativePaddleSize.y * height_ };
	}

	float paddle_velocity_from_viewport_width() const
	{
		return kRelativePaddleVelocity * width_;
	}

	// textures
	static constexpr const char *kBackgroundImagePath = "textures/background.jpg";
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

	ResourceManager::Texture2DId background_texture_id_;
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

	// shaders
	ResourceManager::ShaderId particle_shader_id_;
	ResourceManager::ShaderId effects_shader_id_;
	ResourceManager::ShaderId sprite_shader_id_;

	LifeCount lives_;

	// speed/size based on initial values & width/height in tutorial
	const glm::vec2 kRelativePaddleSize = glm::vec2(100.0f / 800.0f, 20.0f / 600.0f);
	static constexpr float kRelativePaddleVelocity{ 500.0f / 800.0f };
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

	SpriteRenderer *sprite_renderer_;
};

} // namespace util

#endif // GAME_VIEWPORT_H