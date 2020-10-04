#include "game_level.h"

#include "gl_debug.h"
#include "logging.h"
#include "resource_mgr.h"

#include <fstream>
#include <sstream>

namespace util {

void GameLevel::load(const char                   *file,
					 unsigned int                 level_width,
					 unsigned int                 level_height,
					 ResourceManager::Texture2DId block_solid_texture_id,
					 ResourceManager::Texture2DId block_texture_id)
{
	block_solid_texture_id_ = block_solid_texture_id;
	block_texture_id_ = block_texture_id;

	bricks_.clear();

	unsigned int tile_code{};
	std::string line;
	std::ifstream fstream(file);
	std::vector<std::vector<unsigned int>> tile_data{};

	if (fstream)
	{
		while (std::getline(fstream, line))
		{
			std::istringstream sstream(line);
			std::vector<unsigned int> row{};
			while (sstream >> tile_code)
				row.push_back(tile_code);
			tile_data.push_back(row);
		}

		if (!tile_data.empty())
		{
			initialize(tile_data, level_width, level_height);
		}
	}

	check_for_gl_errors();
}

void GameLevel::draw(SpriteRenderer &renderer)
{
	for (auto &tile : bricks_)
	{
		if (!tile.is_destroyed())
		{
			tile.draw(renderer);
		}
	}
}

bool GameLevel::is_completed()
{
	for (auto &tile : bricks_)
	{
		if (!tile.is_solid() && !tile.is_destroyed())
		{
			return false;
		}
	}
	return true;
}

void GameLevel::initialize(std::vector<std::vector<unsigned int>> tile_data,
	unsigned int level_width, unsigned int level_height)
{
	ASSERT(!tile_data.empty(), "No tile data");
	// calculate dimensions
	unsigned int height = tile_data.size();
	unsigned int width = tile_data.at(0).size();
	float unit_width = level_width / static_cast<float>(width);
	float unit_height = level_height / static_cast<float>(height);

	for (unsigned int y = 0; y < height; ++y)
	{
		for (unsigned int x = 0; x < width; ++x)
		{
			glm::vec2 pos(unit_width * x, unit_height * y);
			glm::vec2 size(unit_width, unit_height);

			auto current_val = tile_data.at(y).at(x);
			if (current_val == 1)
			{
				GameObject object(pos, size, ResourceManager::get_texture(block_solid_texture_id_), { glm::vec3(0.8f, 0.8f, 0.7f) }, {});
				object.set_solid(true);
				bricks_.push_back(object);
			}
			else if (current_val > 1)
			{
				// TODO(sasiala): better way to do colors
				auto color = glm::vec3(1.0f);
				switch (current_val)
				{
				case static_cast<unsigned int>(TileColor::kOne) :
					color = glm::vec3(1.0f);
					break;
				case static_cast<unsigned int>(TileColor::kTwo) :
					color = glm::vec3(0.2f, 0.6f, 1.0f);
					break;
				case static_cast<unsigned int>(TileColor::kThree) :
					color = glm::vec3(0.0f, 0.7f, 0.0f);
					break;
				case static_cast<unsigned int>(TileColor::kFour) :
					color = glm::vec3(0.8f, 0.8f, 0.4f);
					break;
				case static_cast<unsigned int>(TileColor::kFive) :
					color = glm::vec3(1.0f, 0.5f, 0.0f);
					break;
				default:
					ASSERT(false, "Unknown tile value: " + std::to_string(current_val));
				}

				bricks_.push_back(GameObject(pos, size, ResourceManager::get_texture(block_texture_id_), { color }, {}));
			}
			else if (current_val < 0)
			{
				ASSERT(false, "Tile data at y = " + std::to_string(y) + ", x = " + std::to_string(x));
			}
		}

		check_for_gl_errors();
	}
}

} // namespace util