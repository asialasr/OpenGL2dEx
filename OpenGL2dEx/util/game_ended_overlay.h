#ifndef GAME_ENDED_OVERLAY_H
#define GAME_ENDED_OVERLAY_H

#include "element.h"

#include "element_list.h"
#include "label.h"

#include <GLFW/glfw3.h>

namespace util {

class GameEndedOverlay : public Element
{
private:
	// TODO(sasiala): string lengths
	static constexpr size_t kTitleMaxLength{ 20 };
	using TitleLabel = Label<kTitleMaxLength>;
	static constexpr size_t kSubtitleMaxLength{ 32 };
	using SubtitleLabel = Label<kSubtitleMaxLength>;
	static constexpr size_t kOptionMaxLength{ 20 };
	using Option = Label<kOptionMaxLength>;
	static constexpr size_t kMaxOptions{ 3 };
	using OptionList = ElementList<Option, kMaxOptions>;

public:
	enum class Mode {
		kWon,
		kLost,
		kNumModes,
		kUnknown,
	};

	class Handler {
	public:
		enum class Event {
			kOpenMainMenu = 0,
			kOpenLevelSelection,
			kRestartGame,
			kNumTypes,
			kUnknown
		};

		void handle_game_overlay_event(const Event event)
		{
			handle_game_overlay_event_impl(event);
		}

	private:
		virtual void handle_game_overlay_event_impl(Event event) = 0;
	};

	GameEndedOverlay();
	GameEndedOverlay(Handler &handler, Dimension width, Dimension height);

	void set_mode(Mode mode);

private:
	void initialize_impl(const glm::mat4 &projection) override;
	void update_impl(Time dt) override;
	void activate_impl() override;
	void deactivate_impl() override;
	void render_impl(Optional<SpriteRenderer*> parent_sprite_renderer) override;
	void set_key_impl(KeyId key_id, bool val) override;
	// TODO(sasiala): improve event handling
	void process_input_impl(Time dt) override;

	enum class ButtonsHandled {
		kMainMenu = 0,
		kLevelSelection,
		kRestartGame,
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
		case GLFW_KEY_M:
			converted_id = ButtonsHandled::kMainMenu;
			break;
		case GLFW_KEY_L:
			converted_id = ButtonsHandled::kLevelSelection;
			break;
		case GLFW_KEY_R:
			converted_id = ButtonsHandled::kRestartGame;
			break;
		default:
			LOG("Unknown button id: " + std::to_string(button));
			break;
		}

		return converted_id;
	}
	bool keys_pressed_[static_cast<size_t>(ButtonsHandled::kNumButtons)];
	bool keys_processed_[static_cast<size_t>(ButtonsHandled::kNumButtons)];

	Dimension loaded_width_;
	Dimension loaded_height_;

	TitleLabel    title_label_;
	SubtitleLabel subtitle_label_;
	OptionList    options_;

	Handler *handler_;

	Element * const all_element_members_[3] = {
		&title_label_,
		&subtitle_label_,
		&options_
	};

};

} // namespace util

#endif // GAME_ENDED_OVERLAY_H