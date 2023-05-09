#include "AudioSystem.h"
#include <iostream>
#include <SDL_mixer.h>

namespace ody
{
	class AudioSystem::AudioSystemImpl final
	{
	public:
		//Todo: implement this class
		void PlaySound([[maybe_unused]]unsigned int soundID)
		{
			std::cout << "sound played";
		}

		void PauseSound()
		{
			std::cout << "sound paused";
		}

		void StopSound()
		{
			std::cout << "sound stopped";
		}

		void StopAllSounds()
		{
			std::cout << "all sounds stopped";
		}
	};

	AudioSystem::AudioSystem()
	{
		pImpl = new AudioSystemImpl();
	}

	AudioSystem::~AudioSystem()
	{
		delete pImpl;
	}

	void AudioSystem::PlaySound(unsigned int soundID)
	{
		pImpl->PlaySound(soundID);
	}

	void AudioSystem::PauseSound()
	{
		pImpl->PauseSound();
	}

	void AudioSystem::StopSound()
	{
		pImpl->StopSound();
	}

	void AudioSystem::StopAllSounds()
	{
		pImpl->StopAllSounds();
	}
}