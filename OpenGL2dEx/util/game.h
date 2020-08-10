#ifndef GAME_H
#define GAME_H

#include "resource_mgr.h"
#include "sprite_renderer.h"

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
		GameState state_;
		bool      keys_[kNumKeys];
		Dimension width_, height_;

		SpriteRenderer            *sprite_renderer_;
		ResourceManager::ShaderId sprite_shader_id_;

		// textures
		ResourceManager::Texture2DId smiley_texture_id_;
	}; // class Game

} // namespace util

#endif // GAME_H