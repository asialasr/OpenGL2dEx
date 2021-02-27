#ifndef MENU_H
#define MENU_H

#include "element.h"

#include "array_helpers.h"
#include "element_list.h"
#include "logging.h"
#include "resource_mgr.h"
#include "sprite_renderer.h"
#include "text_renderer.h"
#include "types.h"
#include "optional.h"
#include "label.h"

#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

#include <vector>
#include <string>

namespace util {

template <typename ELEMENT_TYPE, size_t MAX_OBJECTS>
class Menu : public Element {
private:
	using StringType = string<32>;
	using LabelType = Label<StringType::max_size()>;

	enum class MenuLabelId {
		kTitle,
		kSubtitle,
		kBack,
		kNumIds,
		kUnknown
	};
	// TODO(sasiala): consider moving these common label definitions (title, subtitle, etc.) to a common header
	static LabelType make_label(const MenuLabelId label_id,
		                   const Dimension   viewport_width,
		                   const Dimension   viewport_height)
	{
		switch (label_id)
		{
		case MenuLabelId::kTitle:
			return{ true, 5.0f / 800.0f, 15.0f / 600.0f, 1.5f / 600.0f, glm::vec3{ 1.0f, 1.0f, 1.0f }, "", viewport_width, viewport_height };
		case MenuLabelId::kSubtitle:
			return{ true, 5.0f / 800.0f, 60.0f / 600.0f, 1.0f / 600.0f, glm::vec3{ 1.0f, 1.0f, 1.0f }, "", viewport_width, viewport_height };
		case MenuLabelId::kBack:
			return{ true, 5.0f / 800.0f, 95.0f / 600.0f, 1.0f / 600.0f, glm::vec3{ 0.0f, 1.0f, 0.0f }, "(B) BACK", viewport_width, viewport_height };
		default:
			ASSERT(false, "Unknown label type");
			break;
		}

		// return something; should not reach, but avoid warnings/static analysis findings (if added)
		return{ true, 5.0f / 800.0f, 15.0f / 600.0f, 1.5f / 600.0f, glm::vec3{ 1.0f, 1.0f, 1.0f }, "", viewport_width, viewport_height };
	}

public:
	using ElementType = ELEMENT_TYPE;
	static constexpr auto kMaxObjects = MAX_OBJECTS;
	using OptionIndex = size_t;
	using OptionList = ElementList<ElementType, kMaxObjects>;

	class MenuButtonHandler {
	public:
		void handle_menu_option_highlight(OptionIndex index)
		{
			handle_menu_option_highlight_impl(index);
		}

		void handle_menu_option_acceptance(OptionIndex index)
		{
			handle_menu_option_acceptance_impl(index);
		}

		void handle_back_button()
		{
			handle_back_button_impl();
		}

		void apply_highlight(ElementType &element)
		{
			apply_highlight_impl(element);
		}

		void remove_highlight(ElementType &element)
		{
			remove_highlight_impl(element);
		}

	private:
		virtual void handle_menu_option_highlight_impl(OptionIndex index) = 0;
		virtual void handle_menu_option_acceptance_impl(OptionIndex index) = 0;
		virtual void handle_back_button_impl() = 0;
		virtual void apply_highlight_impl(ElementType &element) = 0;
		virtual void remove_highlight_impl(ElementType &element) = 0;
	};

	Menu(Dimension load_width, Dimension load_height)
		: Element{ false }
		, selected_item_{}
		, option_list_{}
		, loaded_width_{ load_width }
		, loaded_height_{ load_height }
		, title_label_{ make_label(MenuLabelId::kTitle, load_width, load_height) }
		, subtitle_label_{ make_label(MenuLabelId::kSubtitle, load_width, load_height) }
		, back_label_{ make_label(MenuLabelId::kBack, load_width, load_height) }
		, font_shader_id_{}
		, default_font_id_{}
		, menu_button_handler_{ nullptr }
		, keys_pressed_{}
		, keys_processed_{}
	{
	}

	// TODO(sasiala): need to allow for rendering variable numbers of the labels in the 
	// array; this will probably need to be changed in the element list too
	
	// TODO(sasiala): I don't like providing outside access to the full list,
	// but it seems to make the most sense for now.  Reconsider.
	OptionList& option_list()
	{
		return option_list_;
	}

	void set_menu_handler(MenuButtonHandler &handler)
	{
		menu_button_handler_ = &handler;
	}

	void update_info(const StringType  &title,
		             const StringType  &subtitle,
		             bool               show_back_label,
		             const OptionList  &options,
		             OptionIndex        selected_item)
	{
		title_label_.set_text(title);
		subtitle_label_.set_text(subtitle);
		conditionally_activate(back_label_, show_back_label);
		selected_item_ = selected_item;
		option_list_ = options;
		menu_button_handler_->apply_highlight(option_list_.at(selected_item_));
	}

private:

	// Element
	void initialize_impl(const glm::mat4 &projection) override
	{
		font_shader_id_ = ResourceManager::load_shader("shaders/text_2d.vs", "shaders/text_2d.fs", { util::nullopt });

		// text
		default_font_id_ = ResourceManager::load_font(kDefaultFontPath, font_shader_id_, kDefaultFontSize, loaded_width_, loaded_height_);

		// label fonts
		title_label_.set_font(default_font_id_);
		title_label_.initialize(projection);

		subtitle_label_.set_font(default_font_id_);
		subtitle_label_.initialize(projection);

		back_label_.set_font(default_font_id_);
		back_label_.initialize(projection);

		option_list_.initialize(projection);
	}

	void update_impl(Time dt) override
	{
		auto update_lambda = [](Element* e, Time dt) { e->update(dt); };
		apply(all_element_members, update_lambda, dt);
	}
	void activate_impl() override
	{
		fill(keys_pressed_, false);
		fill(keys_processed_, false);
	}
	void deactivate_impl() override
	{
	}
	void render_impl(Optional<SpriteRenderer*> parent_sprite_renderer) override
	{
		if (!is_active())
		{
			return;
		}

		auto render_lambda = [](Element* e, Optional<SpriteRenderer*> parent_sprite_renderer)
							 {
							    e->render(parent_sprite_renderer);
							 };
		apply(all_element_members, render_lambda, parent_sprite_renderer);
	}

	enum class ButtonsHandled {
		kUpButton = 0,
		kDownButton,
		kEnterButton,
		kBackButton,
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
		case GLFW_KEY_W:
			converted_id = ButtonsHandled::kUpButton;
			break;
		case GLFW_KEY_S:
			converted_id = ButtonsHandled::kDownButton;
			break;
		case GLFW_KEY_ENTER:
			converted_id = ButtonsHandled::kEnterButton;
			break;
		case GLFW_KEY_B:
			converted_id = ButtonsHandled::kBackButton;
			break;
		default:
			LOG("Unknown button id: " + std::to_string(button));
			break;
		}

		return converted_id;
	}
	void set_key_impl(KeyId key_id, bool val) override
	{
		if (!is_active())
		{
			return;
		}

		bool *key_ptr = nullptr;
		bool *key_processed_ptr = nullptr;
		switch (convert_id(key_id))
		{
		case ButtonsHandled::kUpButton:
		case ButtonsHandled::kDownButton:
		case ButtonsHandled::kEnterButton:
		case ButtonsHandled::kBackButton:
			key_ptr = &keys_pressed_[to_index(convert_id(key_id))];
			key_processed_ptr = &keys_processed_[to_index(convert_id(key_id))];
			break;
		default:
			LOG("Unhandled key ID");
			return;
		}

		// update keys & processed arrays accordingly
		*key_ptr = val;
		if (!val)
		{
			*key_processed_ptr = false;
		}
	}
	void process_input_impl(Time dt) override
	{
		if (!is_active())
		{
			return;
		}

		if (keys_pressed_[to_index(ButtonsHandled::kEnterButton)]
			&& !keys_processed_[to_index(ButtonsHandled::kEnterButton)])
		{
			menu_button_handler_->handle_menu_option_acceptance(selected_item_);
			keys_processed_[to_index(ButtonsHandled::kEnterButton)] = true;
		}
		else if (keys_pressed_[to_index(ButtonsHandled::kBackButton)]
			&& !keys_processed_[to_index(ButtonsHandled::kBackButton)])
		{
			if (back_label_.is_active())
			{
				menu_button_handler_->handle_back_button();
			}
			keys_processed_[to_index(ButtonsHandled::kBackButton)] = true;
		}
		else
		{
			menu_button_handler_->remove_highlight(option_list_.at(selected_item_));

			if (keys_pressed_[to_index(ButtonsHandled::kDownButton)]
				&& !keys_processed_[to_index(ButtonsHandled::kDownButton)])
			{
				const auto number_of_items = option_list_.size();
				selected_item_ = (selected_item_ + 1) % number_of_items;

				menu_button_handler_->handle_menu_option_highlight(selected_item_);
				keys_processed_[to_index(ButtonsHandled::kDownButton)] = true;
			}
			if (keys_pressed_[to_index(ButtonsHandled::kUpButton)]
				&& !keys_processed_[to_index(ButtonsHandled::kUpButton)])
			{
				const auto number_of_items = option_list_.size();
				if (selected_item_ > 0)
				{
					--selected_item_;
				}
				else
				{
					// wrap around
					selected_item_ = number_of_items - 1;
				}

				menu_button_handler_->handle_menu_option_highlight(selected_item_);
				keys_processed_[to_index(ButtonsHandled::kUpButton)] = true;
			}

			menu_button_handler_->apply_highlight(option_list_.at(selected_item_));
		}
	}

	static constexpr const char *kDefaultFontPath = "fonts/OCRAEXT.TTF";
	static constexpr util::TextRenderer::FontSize kDefaultFontSize{ 24 };

	const Dimension loaded_width_;
	const Dimension loaded_height_;

	LabelType title_label_;
	LabelType subtitle_label_;
	LabelType back_label_;

	OptionIndex selected_item_;
	OptionList  option_list_;

	Element* const all_element_members[4] = {&title_label_, &subtitle_label_, &back_label_, &option_list_};

	ResourceManager::ShaderId font_shader_id_;
	ResourceManager::FontId   default_font_id_;

	MenuButtonHandler *menu_button_handler_;

	bool keys_pressed_[static_cast<size_t>(ButtonsHandled::kNumButtons)];
	bool keys_processed_[static_cast<size_t>(ButtonsHandled::kNumButtons)];
}; // class Menu

} // namespace util

#endif // MENU_H