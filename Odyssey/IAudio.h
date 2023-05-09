#pragma once
#include "Logger.h"


class IAudio
{
public:
	virtual ~IAudio() {}

	virtual void PlaySound(unsigned int soundID) = 0;
	virtual void PauseSound() = 0;
	virtual void StopSound() = 0;
	virtual void StopAllSounds() = 0;
};

//TODO: Add logger call that tells the user they are trying to access an empty system
class NullSoundSystem final : public IAudio
{
	virtual void PlaySound(unsigned int) override { ody::Logger::Log<ody::Logger::LogLevel::Warning>("You are trying to use an empty AudioSystem"); }
	virtual void PauseSound() override {};
	virtual void StopSound() override {};
	virtual void StopAllSounds() override {};
};