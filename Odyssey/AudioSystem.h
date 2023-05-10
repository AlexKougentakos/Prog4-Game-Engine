#pragma once
#include "IAudio.h"

namespace ody
{
	class AudioSystem : public IAudio
	{
	private:
		class AudioSystemImpl;
		AudioSystemImpl* pImpl{};

	public:
		enum class SoundEffect
		{
			PLAYER_DIE,
			PLAYER_WALK,
			PLAYER_SHOOT,
			ENEMY_DIE
		};

		struct Sound
		{

		};

		virtual void PlaySound(unsigned int soundID) override;
		virtual void PauseSound() override;
		virtual void StopSound() override;
		virtual void StopAllSounds() override;

		explicit AudioSystem();
		~AudioSystem();
	};

}