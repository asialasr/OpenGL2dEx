#ifndef GAME_LEVEL_H
#define GAME_LEVEL_H

#include "game_object.h"
#include "logging.h"
#include "resource_mgr.h"
#include <vector>

namespace util {

class SpriteRenderer;

class GameLevel
{
public:
	typedef std::vector<GameObject> BrickContainer;

	GameLevel()
		: block_solid_texture_id_{}
		, block_texture_id_{}
		, bricks_{}
		, bricks_alive_{}
	{
	}

	// TODO(sasiala): shouldn't need to load each time we reset level
	// load from file
	void load(const char                   *file, 
			  unsigned int                 level_width, 
			  unsigned int                 level_height, 
			  ResourceManager::Texture2DId block_solid_texture_id,
			  ResourceManager::Texture2DId block_texture_id);

	void draw(SpriteRenderer &renderer);
	
	bool is_completed()
	{
		return (bricks_alive_ == 0);
	}

	const BrickContainer &bricks() const
	{
		return bricks_;
	}

	void set_brick_destroyed(const size_t index, 
							 const bool destroyed)
	{
		ASSERT(bricks_alive_ > 0, "No bricks available to destroy");
		auto &brick = bricks_.at(index);
		ASSERT(!brick.is_solid(), "Brick is indestructible (solid brick)");
		ASSERT(!brick.is_destroyed(), "Brick is already destroyed");

		brick.set_destroyed(destroyed);
		--bricks_alive_;
	}

private:
	void initialize(std::vector<std::vector<unsigned int>> tile_data,
		unsigned int level_width, unsigned int level_height);

	ResourceManager::Texture2DId block_solid_texture_id_;
	ResourceManager::Texture2DId block_texture_id_;
	BrickContainer               bricks_;
	size_t                       bricks_alive_;

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