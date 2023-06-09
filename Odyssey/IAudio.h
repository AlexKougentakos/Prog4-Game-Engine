#pragma once
#include "Logger.h"


namespace ody
{
	class IAudio
	{
	public:
		virtual ~IAudio() {}

		virtual void PlaySound(unsigned int effectID) = 0;
		virtual void PauseSound() = 0;
		virtual void StopSound() = 0;
		virtual void StopAllSounds() = 0;
	};

	class NullSoundSystem final : public IAudio
	{
		virtual void PlaySound(unsigned int) override { LOG_WARNING("You are trying to use an empty AudioSystem"); }
		virtual void PauseSound() override {}
		virtual void StopSound() override {}
		virtual void StopAllSounds() override {}
	};
}