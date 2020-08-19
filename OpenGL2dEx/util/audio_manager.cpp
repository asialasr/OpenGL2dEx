#include "audio_manager.h"

#include "logging.h"

#include <irrklang-32bit-1.6.0/irrKlang.h>

namespace {
	// TODO(sasiala): deal with the .dll's and all for irrklang
	irrklang::ISoundEngine *sound_engine = irrklang::createIrrKlangDevice();
} // namespace

namespace util {

void AudioManager::play_background_music(const GameState state, const bool loop)
{
	ASSERT(GameState::kActive == state, "Invalid game state");

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