#include "opening_menu.h"

namespace util {
	const glm::vec3 OpeningMenu::kDeselectedTextColor{ 0.0f, 1.0f, 0.0f };
	const glm::vec3 OpeningMenu::kSelectedTextColor{ 1.0f, 1.0f, 1.0f };

namespace {
	OpeningMenu::MenuList::ListObject menu_list_object(const char *level_name, const OpeningMenu::MenuIndex index, const Dimension viewport_width, const Dimension viewport_height)
	{
		// TODO(sasiala): allow adjusting of ratios based on instance
		constexpr auto kYTopRatio{ 0.25f };
		constexpr auto kYBottomRatio{ 0.90f };
		constexpr auto kMaxRows{ OpeningMenu::kMaxItems };
		constexpr auto kRowHeightRatio{ static_cast<float>((kYBottomRatio - kYTopRatio) / kMaxRows) };
		constexpr auto kXRatio{ 0.05f };
		constexpr auto kTextScaleFromHeight{ 1.0f / 600.0f };

		const auto text_color = OpeningMenu::kDeselectedTextColor;
		return{ OpeningMenu::LabelType{ true, kXRatio, kYTopRatio + (index * kRowHeightRatio), kTextScaleFromHeight, text_color, level_name, viewport_width, viewport_height } };
	}

	std::pair<OpeningMenu::MenuIndex, OpeningMenu::MenuList> menu_list(const OpeningMenu::OptionList &option_list, const Dimension viewport_width, const Dimension viewport_height)
	{
		auto menu_list_pair = std::make_pair<OpeningMenu::MenuIndex, OpeningMenu::MenuList>({ 0 }, {});

		for (auto i = size_t{ 0 }; i < OpeningMenu::kMaxItems; ++i)
		{
			const auto &str = option_list[i];
			if (strcmp(str, "") == 0)
				break;

			++menu_list_pair.first;
			menu_list_pair.second.push_back(menu_list_object(str, i, viewport_width, viewport_height));
		}

		return menu_list_pair;
	}
} // namespace

	OpeningMenu::OpeningMenu(const Dimension load_width, const Dimension load_height)
		: Element{ false }
		, menu_ { load_width, load_height }
		, load_width_{ load_width }
		, load_height_{ load_height }
		, handler_{ nullptr }
	{
	}

	void OpeningMenu::initialize_impl(const glm::mat4 &projection)
	{
		menu_.initialize(projection);
		menu_.set_menu_handler(*this);
	}

	void OpeningMenu::update_impl(Time dt)
	{
		menu_.update(dt);
	}

	void OpeningMenu::activate_impl()
	{
		menu_.update_info("", "", false, menu_list(kOptions, load_width_, load_height_).second, 0);
		menu_.activate();
	}

	void OpeningMenu::deactivate_impl()
	{
		menu_.deactivate();
	}

	void OpeningMenu::render_impl(Optional<SpriteRenderer*> parent_sprite_renderer)
	{
		if (!is_active())
		{
			return;
		}

		menu_.render(parent_sprite_renderer);
	}

	void OpeningMenu::set_key_impl(KeyId key_id, bool val)
	{
		if (!is_active())
		{
			return;
		}
		
		menu_.set_key(key_id, val);
	}

	void OpeningMenu::process_input_impl(float dt)
	{
		if (!is_active())
		{
			return;
		}

		menu_.process_input(dt);
	}

	void OpeningMenu::handle_menu_option_highlight_impl(const MenuIndex /*index*/)
	{
		// do nothing
	}

	void OpeningMenu::handle_menu_option_acceptance_impl(MenuIndex index)
	{
		switch (convert_index(index))
		{
		case Index::kLevelSelection:
			handler_->open_level_selection();
			break;
		case Index::kSettings:
			handler_->open_settings();
			break;
		case Index::kHelp:
			handler_->open_help();
			break;
		default:
			ASSERT(false, "Unexpected index");
			break;
		}
	}

	void OpeningMenu::handle_back_button_impl()
	{
		ASSERT(false, "Back button not allowed on opening menu");
	}

} // namespace util