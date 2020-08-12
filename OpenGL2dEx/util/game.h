#ifndef GAME_H
#define GAME_H

#include "game_level.h"
#include "resource_mgr.h"
#include "sprite_renderer.h"

#include <vector>

namespace util {

	class Game
	{
	public:
		typedef unsigned int Dimension;
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
		const char * const kLevelOnePath = "levels/one.lvl";
		const char * const kLevelTwoPath = "levels/two.lvl";
		const char * const kLevelThreePath = "levels/three.lvl";
		const char * const kLevelFourPath = "levels/four.lvl";

		const char * const kBackgroundImagePath = "textures/background.jpg";
		const char * const kSmileyImagePath = "textures/awesomeface.png";
		const char * const kBlockSolidImagePath = "textures/block_solid.png";
		const char * const kBlockImagePath = "textures/block.png";

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
	
		std::vector<GameLevel> levels_;
		size_t                 current_level_;
	}; // class Game

} // namespace util

#endif // GAME_H