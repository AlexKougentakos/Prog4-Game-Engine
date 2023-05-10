#pragma once
#include <map>

#include "IAudio.h"

namespace ody
{
	class AudioSystem : public IAudio
	{
	private:
		class AudioSystemImpl;
		AudioSystemImpl* pImpl{};

	public:

		virtual void PlaySound(SoundEffect soundEffect) override;
		virtual void PauseSound() override;
		virtual void StopSound() override;
		virtual void StopAllSounds() override;

		explicit AudioSystem();
		~AudioSystem();
	};
}