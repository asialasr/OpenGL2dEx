#include "game.h"

#include "logging.h"

namespace util
{
	Game::Game(Dimension width, Dimension height) 
		: state_{ GameState::kActive }
		, keys_{}
		, width_{ width }
		, height_{ height }
	{

	}

	Game::~Game()
	{

	}

	void Game::initialize()
	{

	}

	void Game::process_input(float /*dt*/)
	{

	}

	void Game::update(float /*dt*/)
	{

	}

	void Game::render()
	{

	}

	void Game::set_key(size_t key, bool val)
	{
		ASSERT(key < kNumKeys, "Key out of bounds");
		keys_[key] = val;
	}

} // namespace util