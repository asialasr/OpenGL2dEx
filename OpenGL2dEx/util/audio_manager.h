#ifndef AUDIO_MANAGER_H
#define AUDIO_MANAGER_H

namespace util {

class AudioManager {
public:
	enum class GameState {
		kMenu,
		kActive,
		kNumStates,
		kUnknown,
	};
	static void play_background_music(GameState state, bool loop);

	enum class BallBrickCollisionType {
		kNormal,
		kSolid,
		kPowerUp,
		kNumTypes,
		kUnknown,
	};
	static void play_ball_brick_collision_sound(BallBrickCollisionType collision_type);

	static void play_ball_paddle_collision_sound();

private:
	AudioManager()
	{
	}

	static constexpr const char * kActiveGameBgMusicPath = "audio/breakout.mp3";
	static constexpr const char * kNormalBallBrickCollisionAudioPath = "audio/bleep.mp3";
	static constexpr const char * kSolidBallBrickCollisionAudioPath = "audio/solid.wav";
	static constexpr const char * kPowerUpBallBrickCollisionAudioPath = "audio/powerup.wav";
	static constexpr const char * kBallPaddleCollisionAudioPath = "audio/bleep.wav";
};

} // namespace util

#endif // !AUDIO_MANAGER_H
