#include "level_selection_menu.h"
#include "logging.h"

namespace util {
	const glm::vec3 LevelSelectionMenu::kDeselectedTextColor{ 0.0f, 1.0f, 0.0f };
	const glm::vec3 LevelSelectionMenu::kSelectedTextColor{ 1.0f, 1.0f, 1.0f };

namespace {
	LevelSelectionMenu::MenuList::ListObject menu_list_object(const char *level_name, const LevelSelectionMenu::MenuIndex index, const Dimension viewport_width, const Dimension viewport_height)
	{
		// TODO(sasiala): allow adjusting of ratios based on instance
		constexpr auto kYTopRatio{ 0.25f };
		constexpr auto kYBottomRatio{ 0.90f };
		constexpr auto kMaxRows{ LevelSelectionMenu::kMaxItems };
		constexpr auto kRowHeightRatio{ static_cast<float>((kYBottomRatio - kYTopRatio) / kMaxRows) };
		constexpr auto kXRatio{ 0.05f };
		constexpr auto kTextScaleFromHeight{ 1.0f / 600.0f };

		const auto text_color = LevelSelectionMenu::kDeselectedTextColor;
		return {LevelSelectionMenu::LabelType{ true, kXRatio, kYTopRatio + (index * kRowHeightRatio), kTextScaleFromHeight, text_color, level_name, viewport_width, viewport_height }};
	}
} // namespace

	LevelSelectionMenu::LevelSelectionMenu(Dimension load_width, Dimension load_height)
		: Element{ false }
		, menu_options_ {load_width, load_height}
		, load_width_{ load_width }
		, load_height_{load_height}
		, handler_{ nullptr }
		, current_level_{}
	{
	}

	void LevelSelectionMenu::initialize_impl(const glm::mat4 &projection)
	{
		menu_options_.initialize(projection);
		menu_options_.set_menu_handler(*this);
	}

	void LevelSelectionMenu::update_impl(Time dt)
	{
		if (!is_active())
		{
			return;
		}

		menu_options_.update(dt);
	}

	void LevelSelectionMenu::activate_impl()
	{
		ASSERT(handler_, "No handler for menu");
		menu_options_.activate();
		handler_->show_level_preview();
	}

	void LevelSelectionMenu::deactivate_impl()
	{
		ASSERT(handler_, "No handler for menu");
		menu_options_.deactivate();
		handler_->hide_level_preview();
	}

	void LevelSelectionMenu::render_impl(Optional<SpriteRenderer*> parent_sprite_renderer)
	{
		if (!is_active())
		{
			return;
		}

		menu_options_.render(parent_sprite_renderer);
	}

	void LevelSelectionMenu::set_key_impl(KeyId key_id, bool val)
	{
		if (!is_active())
		{
			return;
		}

		menu_options_.set_key(key_id, val);
	}

	void LevelSelectionMenu::process_input_impl(Time dt)
	{
		if (!is_active())
		{
			return;
		}
		menu_options_.process_input(dt);
	}

	void LevelSelectionMenu::handle_menu_option_highlight_impl(const MenuIndex index)
	{
		current_level_ = static_cast<MenuIndex>(index);
		handler_->change_level(current_level_);
	}

	void LevelSelectionMenu::handle_menu_option_acceptance_impl(const MenuIndex /*index*/)
	{
		handler_->start_game();
	}

	void LevelSelectionMenu::handle_back_button_impl()
	{
		handler_->close_level_selection();
	}
	
	void LevelSelectionMenu::apply_highlight_impl(MenuType::ElementType &element)
	{
		element.get<LabelType>().set_color(kSelectedTextColor);
	}

	void LevelSelectionMenu::remove_highlight_impl(MenuType::ElementType &element)
	{
		element.get<LabelType>().set_color(kDeselectedTextColor);
	}

	std::pair<LevelSelectionMenu::MenuIndex, LevelSelectionMenu::MenuList> LevelSelectionMenu::menu_list(
		const LevelList &level_list, 
		const Dimension load_width, 
		const Dimension load_height)
	{
		auto menu_list_pair = std::make_pair<MenuIndex, MenuList>({0}, {});
		for (auto i = size_t{ 0 }; i < MenuList::kMaxObjects; ++i)
		{
			const auto str = level_list[i];
			if (strcmp(str, "") == 0)
				break;

			++menu_list_pair.first;
			menu_list_pair.second.push_back(menu_list_object(level_list[i], i, load_width, load_height));
		}

		return menu_list_pair;
	}

} // namespace util