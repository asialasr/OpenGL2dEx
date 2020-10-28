#ifndef GAME_H
#define GAME_H

#include "main_menu.h"
#include "game_level.h"
#include "game_viewport.h"
#include "power_up.h"
#include "resource_mgr.h"
#include "sprite_renderer.h"
#include "array_helpers.h"

#include <vector>
#include <tuple>

namespace util {
	
	class IResetGlProperties;

	class Game : public GameViewport::GameStateCallback
			   , public MainMenu::MenuButtonHandler
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
			kMainMenu,
			kLevelSelection,
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
		void change_level_impl(LevelSelectionMenu::LevelIndex index) override;
		void start_game_impl() override;
		void show_level_preview_impl() override;
		void hide_level_preview_impl() override;

		void load_current_level();

		void open_main_menu();
		void show_small_game_viewport(bool animate);
		void close_main_menu();
		void render_menu();
		void animate_game_viewport(float dt);

		void delete_dynamic_data();

		// TODO(sasiala): Come up with better values for dw/dh?  These were simply trial/error
		static constexpr auto kViewportAnimationDwMultiplier{ 1.5f };
		float viewport_animation_dw() const
		{
			return kViewportAnimationDwMultiplier * width_;
		}
		static constexpr auto kViewportAnimationDhMultiplier{ 1.5f };
		float viewport_animation_dh() const
		{
			return kViewportAnimationDhMultiplier * height_;
		}

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

		GameState state_;
		bool      keys_[kNumKeys];
		bool      keys_processed_[kNumKeys];
		Dimension width_, height_;

		GameViewport game_viewport_;
		MainMenu         main_menu_;

		SpriteRenderer            *sprite_renderer_;
		ResourceManager::ShaderId sprite_shader_id_;
		ResourceManager::ShaderId font_shader_id_;

		size_t                 current_level_;

		// TODO(sasiala): is there a less cluttered way to do this?  Could move the
		// function definitions, but they're so simple that it would almost be more 
		// annoying to move them
		class ViewportAnimation {
		public:
			ViewportAnimation(
				bool animation_in_progress,
				float dw,
				float dh,
				const glm::vec2 &target_position,
				const glm::vec2 &initial_position,
				Dimension target_width,
				Dimension target_height,
				Dimension initial_width,
				Dimension initial_height)
				: animation_in_progress_{ animation_in_progress }
				, dw_{ dw }
				, dh_{ dh }
				, target_position_{ target_position }
				, current_position_{ initial_position }
				, target_width_{ target_width }
				, target_height_{ target_height }
				, current_width_{ initial_width }
				, current_height_{ initial_height }
				, slope_pos_y_{ get_slope_pos_y() }
				, offset_pos_y_{ get_offset_pos_y() }
				, slope_pos_x_{ get_slope_pos_x() }
				, offset_pos_x_{ get_offset_pos_x() }
			{
			}

			ViewportAnimation(const ViewportAnimation&) = default;
			ViewportAnimation& operator=(const ViewportAnimation&) = default;

			void update(float dt);

			bool in_progress() const
			{
				return animation_in_progress_;
			}

			glm::vec2 position() const
			{
				return current_position_;
			}
			
			Dimension width() const
			{
				return current_width_;
			}

			Dimension height() const
			{
				return current_height_;
			}

		private:
			/*
			Calculating the slope & offset (for height/y - same for width/x):
			(start_height, start_y) (end_height, end_y)
			m = (end_y - start_y) / (end_height - start_height)
			y = mx + b
			y = ((end_y - start_y) / (end_height - start_height)) * x + b
			start_y = ((end_y - start_y) / (end_height - start_height)) * start_height + b
			-b = ((end_y - start_y) / (end_height - start_height)) * start_height - start_y
			b = ((start_y - end_y) / (end_height - start_height)) * start_height + start_y
			y = ((end_y - start_y) / (end_height - start_height)) * x + ((start_y - end_y) / (end_height - start_height)) * start_height + start_y
			// at construction
			m = ((target_position_.y - current_position_.y) / (target_height_ - current_height_));
			b = ((current_position_.y - target_position_.y) / (target_height_ - current_height_)) * current_height_ + current_position_.y;
			*/
			float get_slope_pos_y() const
			{
				return ((target_position_.y - current_position_.y) / (static_cast<float>(target_height_) - static_cast<float>(current_height_)));
			}

			float get_offset_pos_y() const
			{
				return -1.0f * slope_pos_y_ * static_cast<float>(current_height_) + current_position_.y;
			}

			float get_slope_pos_x() const
			{
				return ((target_position_.x - current_position_.x) / (static_cast<float>(target_width_) - static_cast<float>(current_width_)));
			}

			float get_offset_pos_x() const
			{
				return -1.0f * slope_pos_x_ * static_cast<float>(current_width_) + current_position_.x;
			}

			// TODO(sasiala): targets, slope, offset shouldn't change, but
			// couldn't assign to this if they are const since they need to
			// be calculated at construction

			bool  animation_in_progress_;
			// TODO(sasiala): these are all unsigned - could result in
			// weird behavior
			float dw_;
			float dh_;
			// TODO(sasiala): could be nice to have position and dimension
			// look alike (instead of a vector for position and single vals
			// for dimensions)
			glm::vec2 target_position_;
			glm::vec2 current_position_;
			Dimension target_width_;
			Dimension target_height_;
			Dimension current_width_;
			Dimension current_height_;

			// set only at construction
			float slope_pos_y_;
			float offset_pos_y_;

			float slope_pos_x_;
			float offset_pos_x_;
		}; // class ViewportAnimation
		ViewportAnimation viewport_animation_;
	}; // class Game

} // namespace util

#endif // GAME_H