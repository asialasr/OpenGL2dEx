#include "audio_manager.h"

#include "logging.h"

#include <irrKlang.h>
#include <string>

namespace {
	// TODO(sasiala): deal with the .dll's and all for irrklang
	irrklang::ISoundEngine *sound_engine = irrklang::createIrrKlangDevice();
} // namespace

namespace util {

void AudioManager::set_volume(const VolumePercentage volume)
{
	sound_engine->setSoundVolume(volume / 100.0f);
}

AudioManager::VolumePercentage AudioManager::volume()
{
	return sound_engine->getSoundVolume() * 100.0f;
}

void AudioManager::play_background_music(const GameState state, const bool loop)
{
	ASSERT(GameState::kActive == state, "Invalid game state");
	LOG("Volume: " + std::to_string(sound_engine->getSoundVolume()));
	sound_engine->play2D(kActiveGameBgMusicPath, loop);
}

void AudioManager::play_ball_brick_collision_sound(const BallBrickCollisionType collision_type)
{

	switch (collision_type)
	{
	case util::AudioManager::BallBrickCollisionType::kNormal:
		sound_engine->play2D(kNormalBallBrickCollisionAudioPath, false);
		break;
	case util::AudioManager::BallBrickCollisionType::kSolid:
		sound_engine->play2D(kSolidBallBrickCollisionAudioPath, false);
		break;
	case util::AudioManager::BallBrickCollisionType::kPowerUp:
		sound_engine->play2D(kPowerUpBallBrickCollisionAudioPath, false);
		break;
	default:
		ASSERT(false, "Invalid ball-brick collision type");
		break;
	}
}

void AudioManager::play_ball_paddle_collision_sound()
{
	sound_engine->play2D(kBallPaddleCollisionAudioPath, false);
}

} // namespace util