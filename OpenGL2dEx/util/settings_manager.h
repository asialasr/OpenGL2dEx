#ifndef  SETTINGS_MANAGER_H
#define SETTINGS_MANAGER_H

#include "audio_manager.h"
#include "game.h"

namespace util {

class SettingsManager{
public:
	using VolumePercentage = AudioManager::VolumePercentage;
	using GameSpeedMultiplier = Game::GameSpeedMultiplier;

	static void set_volume(VolumePercentage volume)
	{
		AudioManager::set_volume(volume);
	}

	static VolumePercentage volume()
	{
		return AudioManager::volume();
	}

	static void set_game_speed(GameSpeedMultiplier game_speed)
	{
		Game::set_game_speed_multiplier(game_speed);
	}

	static GameSpeedMultiplier game_speed()
	{
		return Game::game_speed_multiplier();
	}

private:
	// singleton
	SettingsManager()
	{
	}

}; // class SettingsManager

} // namespace util

#endif // ! SETTINGS_MANAGER_H
