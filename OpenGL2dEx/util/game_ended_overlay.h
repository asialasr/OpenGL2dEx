#ifndef GAME_ENDED_OVERLAY_H
#define GAME_ENDED_OVERLAY_H

#include "element.h"

#include "element_list.h"
#include "label.h"

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
	static constexpr size_t kMaxOptions{ 4 };
	using OptionList = ElementList<Option, kMaxOptions>;

public:
	GameEndedOverlay();
	GameEndedOverlay(Dimension width, Dimension height);

private:
	void initialize_impl(const glm::mat4 &projection) override;
	void update_impl(Time dt) override;
	void activate_impl() override;
	void deactivate_impl() override;
	void render_impl(Optional<SpriteRenderer*> parent_sprite_renderer) override;
	void set_key_impl(KeyId key_id, bool val) override;
	// TODO(sasiala): improve event handling
	void process_input_impl(Time dt) override;

	Dimension loaded_width_;
	Dimension loaded_height_;

	TitleLabel    title_label_;
	SubtitleLabel subtitle_label_;
	OptionList    options_;

	Element * const all_element_members_[3] = {
		&title_label_,
		&subtitle_label_,
		&options_
	};

};

} // namespace util

#endif // GAME_ENDED_OVERLAY_H