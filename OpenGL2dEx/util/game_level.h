#ifndef GAME_LEVEL_H
#define GAME_LEVEL_H

#include "game_object.h"
#include "resource_mgr.h"
#include <vector>

namespace util {

class SpriteRenderer;

class GameLevel
{
public:
	GameLevel()
		: block_solid_texture_id_{}
		, block_texture_id_{}
		, bricks_{}
	{
	}

	// load from file
	void load(const char                   *file, 
			  unsigned int                 level_width, 
			  unsigned int                 level_height, 
			  ResourceManager::Texture2DId block_solid_texture_id,
			  ResourceManager::Texture2DId block_texture_id);

	void draw(SpriteRenderer &renderer);
	
	bool is_completed();

private:
	void initialize(std::vector<std::vector<unsigned int>> tile_data,
		unsigned int level_width, unsigned int level_height);

	ResourceManager::Texture2DId block_solid_texture_id_;
	ResourceManager::Texture2DId block_texture_id_;
	std::vector<GameObject> bricks_;


	enum class TileColor {
		kOne = 1,
		kTwo = 2,
		kThree = 3,
		kFour = 4,
		kFive = 5,
	};
};

} // namespace util

#endif // GAME_LEVEL_H