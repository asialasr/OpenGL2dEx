#include "game_ended_overlay.h"

#include "array_helpers.h"

namespace util {
namespace {
	constexpr auto kFirstOptionPosY = float{ 330.0f / 600.0f };
	constexpr auto kOptionHeight = float{ 35.0f / 600.0f };

	enum class LabelId {
		kTitle,
		kSubtitle,
		kOption,
		kNumIds,
		kUnknown
	};
	// TODO(sasiala): consider moving these common label definitions (title, subtitle, etc.) to a common header
	template <size_t N>
	static Label<N> make_label(const LabelId label_id,
		                       const Dimension   viewport_width,
		                       const Dimension   viewport_height)
	{
		switch (label_id)
		{
		case LabelId::kTitle:
			return{ true, 5.0f / 800.0f, 250.0f / 600.0f, 1.5f / 600.0f, glm::vec3{ 1.0f, 1.0f, 1.0f }, "TITLE", viewport_width, viewport_height };
		case LabelId::kSubtitle:
			return{ true, 5.0f / 800.0f, 295.0f / 600.0f, 1.0f / 600.0f, glm::vec3{ 1.0f, 1.0f, 1.0f }, "SUBTITLE", viewport_width, viewport_height };
		case LabelId::kOption:
			return{ true, 5.0f / 800.0f, kFirstOptionPosY, 1.0f / 600.0f, glm::vec3{ 0.0f, 1.0f, 0.0f }, "OPTION", viewport_width, viewport_height };
		default:
			ASSERT(false, "Unknown label type");
			break;
		}

		// return something; should not reach, but avoid warnings/static analysis findings (if added)
		return{ true, 5.0f / 800.0f, 15.0f / 600.0f, 1.5f / 600.0f, glm::vec3{ 1.0f, 1.0f, 1.0f }, "", viewport_width, viewport_height };
	}
} // namespace

	GameEndedOverlay::GameEndedOverlay()
		: Element{ false }
		, loaded_width_{}
		, loaded_height_{}
		, title_label_{ make_label<TitleLabel::max_size()>(LabelId::kTitle, 0, 0) }
		, subtitle_label_{ make_label<SubtitleLabel::max_size()>(LabelId::kSubtitle, 0, 0)}
		, options_{}
		, keys_pressed_{}
		, keys_processed_{}
		, handler_{nullptr}
	{
	}

	GameEndedOverlay::GameEndedOverlay(Handler &handler, Dimension width, Dimension height)
		: Element{ false }
		, loaded_width_{ width }
		, loaded_height_{ height }
		, title_label_{ make_label<TitleLabel::max_size()>(LabelId::kTitle, width, height) }
		, subtitle_label_{ make_label<SubtitleLabel::max_size()>(LabelId::kSubtitle, width, height) }
		, options_{}
		, keys_pressed_{}
		, keys_processed_{}
		, handler_{&handler}
	{
		for (auto i = size_t{ 0 }; i < kMaxOptions; ++i)
		{
			options_.push_back(make_label<Option::max_size()>(LabelId::kOption, width, height));
			options_.back().set_y_ratio(kFirstOptionPosY + (i * kOptionHeight));
		}
	}

	void GameEndedOverlay::set_mode(const Mode mode)
	{
		switch (mode)
		{
		case Mode::kWon:
			title_label_.set_text("CONGRATULATIONS");
			subtitle_label_.set_text("You won the game!");
			options_.at(0).set_text("(M) Main Menu");
			options_.at(1).set_text("(L) Level Selection");
			options_.at(2).set_text("(R) Restart Level");
			break;
		case Mode::kLost:
			title_label_.set_text("OUT OF LIVES");
			subtitle_label_.set_text("");
			options_.at(0).set_text("(M) Main Menu");
			options_.at(1).set_text("(L) Level Selection");
			options_.at(2).set_text("(R) Restart Level");
			break;
		default:
			ASSERT(false, "Unhandled mode type");
			break;
		}
	}

	void GameEndedOverlay::initialize_impl(const glm::mat4 &projection)
	{
		apply(all_element_members_, ElementLambdas::initialize_pointer, projection);
	}

	void GameEndedOverlay::update_impl(const Time dt)
	{
		if (!is_active())
		{
			return;
		}

		apply(all_element_members_, ElementLambdas::update_pointer, dt);
	}
	
	void GameEndedOverlay::activate_impl()
	{
		apply(all_element_members_, ElementLambdas::activate_pointer);
		fill(keys_pressed_, false);
		fill(keys_processed_, false);
	}
	
	void GameEndedOverlay::deactivate_impl()
	{
		apply(all_element_members_, ElementLambdas::deactivate_pointer);
	}
	
	void GameEndedOverlay::render_impl(const Optional<SpriteRenderer*> parent_sprite_renderer)
	{
		if (!is_active())
		{
			return;
		}

		apply(all_element_members_, ElementLambdas::render_pointer, parent_sprite_renderer);
	}
	
	void GameEndedOverlay::set_key_impl(const KeyId key_id, const bool val)
	{
		if (!is_active())
		{
			return;
		}

		const auto converted_id = convert_id(key_id);
		if (converted_id == ButtonsHandled::kUnknown)
		{
			LOG("Unhandled key ID");
		}
		else
		{
			keys_pressed_[to_index(convert_id(key_id))] = val;

			if (!val)
			{
				keys_processed_[to_index(convert_id(key_id))] = false;
			}
		}

		apply(all_element_members_, ElementLambdas::set_key_pointer, key_id, val);
	}
	
	void GameEndedOverlay::process_input_impl(const Time dt)
	{
		if (!is_active())
		{
			return;
		}
		apply(all_element_members_, ElementLambdas::process_input_pointer, dt);

		if (keys_pressed_[to_index(ButtonsHandled::kLevelSelection)] && 
			!keys_processed_[to_index(ButtonsHandled::kLevelSelection)])
		{
			handler_->handle_game_overlay_event(Handler::Event::kOpenLevelSelection);
			keys_processed_[to_index(ButtonsHandled::kLevelSelection)] = true;
		}
		if (keys_pressed_[to_index(ButtonsHandled::kMainMenu)] &&
			!keys_processed_[to_index(ButtonsHandled::kMainMenu)])
		{
			handler_->handle_game_overlay_event(Handler::Event::kOpenMainMenu);
			keys_processed_[to_index(ButtonsHandled::kMainMenu)] = true;
		}
		if (keys_pressed_[to_index(ButtonsHandled::kRestartGame)] &&
			!keys_processed_[to_index(ButtonsHandled::kRestartGame)])
		{
			handler_->handle_game_overlay_event(Handler::Event::kRestartGame);
			keys_processed_[to_index(ButtonsHandled::kRestartGame)] = true;
		}
	}
} // namespace util