#ifndef GAME_H
#define GAME_H

#include "game_level.h"
#include "game_viewport.h"
#include "power_up.h"
#include "resource_mgr.h"
#include "sprite_renderer.h"
#include "count_of.h"

#include <vector>
#include <tuple>

namespace util {
	
	class IResetGlProperties;

	class Game : public GameViewport::GameStateCallback
	{
	public:
		using Dimension = unsigned int;

		Game(IResetGlProperties &gl_property_resetter, 
			 Dimension width, 
			 Dimension height);
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
		// GameViewport::GameStateCallback
		void game_ended_impl(const EndingReason reason) override;

		void load_current_level();

		void render_menu() const;

		static constexpr struct {
			float x_;
			float y_;
			float scale_;
		} kLifeText{ 5.0f, 5.0f, 1.0f };
		void render_lives();

		void delete_dynamic_data();

		static constexpr const char *kLevelPaths[] = {
			"levels/one.lvl",
			"levels/two.lvl",
			"levels/three.lvl",
			"levels/four.lvl"
		};
		static constexpr size_t kMaxLevels{ util::count_of(kLevelPaths) };

		static constexpr const char *kSmileyImagePath = "textures/awesomeface.png";

		GameState state_;
		bool      keys_[kNumKeys];
		bool      keys_processed_[kNumKeys];
		Dimension width_, height_;

		GameViewport game_viewport_;

		SpriteRenderer            *sprite_renderer_;
		ResourceManager::ShaderId sprite_shader_id_;
		ResourceManager::ShaderId font_shader_id_;

		// textures
		ResourceManager::Texture2DId smiley_texture_id_;
		
		// fonts
		static constexpr const char *kDefaultFontPath = "fonts/OCRAEXT.TTF";

		static constexpr TextRenderer::FontSize kDefaultFontSize{ 24 };

		ResourceManager::FontId default_font_id_;

		size_t                 current_level_;
	}; // class Game

} // namespace util

#endif // GAME_H