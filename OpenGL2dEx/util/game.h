#ifndef GAME_H
#define GAME_H

#include "menu.h"
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
			   , public Menu::MenuButtonHandler
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
		void game_ended_impl(EndingReason reason) override;

		// Menu::MenuButtonHandler
		void handle_menu_option_highlight_impl(Menu::OptionIndex index, Menu::SubmenuLevel submenu_level) override;
		void handle_menu_option_acceptance_impl(Menu::OptionIndex index, Menu::SubmenuLevel submenu_level) override;

		void load_current_level();

		void open_main_menu();
		void close_main_menu();
		void render_menu();
		void animate_game_viewport(float dt);

		void delete_dynamic_data();

		static constexpr const char *kLevelPaths[] = {
			"levels/one.lvl",
			"levels/two.lvl",
			"levels/three.lvl",
			"levels/four.lvl"
		};
		const std::vector<std::string> kLevelNames{
			"One",
			"Two",
			"Three",
			"Four"
		};
		static constexpr size_t kMaxLevels{ util::count_of(kLevelPaths) };

		static constexpr const char *kSmileyImagePath = "textures/awesomeface.png";

		GameState state_;
		bool      keys_[kNumKeys];
		bool      keys_processed_[kNumKeys];
		Dimension width_, height_;

		GameViewport game_viewport_;
		Menu         main_menu_;

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

		struct {
			bool  animation_in_progress_;
			// TODO(sasiala): these are all unsigned - could result in
			// weird behavior
			float dx_;
			float dy_;
			Dimension target_x_;
			Dimension target_y_;
			Dimension current_x_;
			Dimension current_y_;
		} viewport_animation_;
	}; // class Game

} // namespace util

#endif // GAME_H