#ifndef GAME_H
#define GAME_H

#include "game_level.h"
#include "resource_mgr.h"
#include "sprite_renderer.h"

#include <vector>
#include <tuple>

namespace util {

	class BallObject;

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
		typedef unsigned int Dimension;
		typedef std::tuple<bool, Direction, glm::vec2> Collision;
		
		Game(Dimension width, Dimension height);
		~Game();

		void initialize();

		void process_input(float dt);
		void update(float dt);
		void check_collisions();
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
		void delete_dynamic_data();

		const char * const kLevelOnePath = "levels/one.lvl";
		const char * const kLevelTwoPath = "levels/two.lvl";
		const char * const kLevelThreePath = "levels/three.lvl";
		const char * const kLevelFourPath = "levels/four.lvl";

		const char * const kBackgroundImagePath = "textures/background.jpg";
		const char * const kSmileyImagePath = "textures/awesomeface.png";
		const char * const kBlockSolidImagePath = "textures/block_solid.png";
		const char * const kBlockImagePath = "textures/block.png";
		const char * const kPaddleImagePath = "textures/paddle.png";
		const char * const kBallImagePath = "textures/awesomeface.png";

		GameState state_;
		bool      keys_[kNumKeys];
		Dimension width_, height_;

		SpriteRenderer            *sprite_renderer_;
		ResourceManager::ShaderId sprite_shader_id_;

		// textures
		ResourceManager::Texture2DId background_texture_id_;
		ResourceManager::Texture2DId smiley_texture_id_;
		ResourceManager::Texture2DId block_texture_id_;
		ResourceManager::Texture2DId block_solid_texture_id_;
		ResourceManager::Texture2DId paddle_texture_id_;
		ResourceManager::Texture2DId ball_texture_id_;

		std::vector<GameLevel> levels_;
		size_t                 current_level_;

		// TODO(sasiala): what's the deal with this not being able
		// to be constexpr and/or static?
		const glm::vec2 kPlayerSize = glm::vec2( 100.0f, 20.0f );
		static constexpr float kPlayerVelocity{ 500.0f };
		GameObject *paddle_;

		const glm::vec2 kInitialBallVelocity{ 100.0f, -350.0f };
		static constexpr float kBallRadius{ 12.5f };
		BallObject *ball_;
	}; // class Game

} // namespace util

#endif // GAME_H