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
		enum class Sounds
		{
			PLAYER_DIE,
			PLAYER_WALK,
			PLAYER_SHOOT,
			ENEMY_DIE
		};

		virtual void PlaySound(unsigned int soundID) override;
		virtual void PauseSound() override;
		virtual void StopSound() override;
		virtual void StopAllSounds() override;

		explicit AudioSystem();
		~AudioSystem();
	};

}