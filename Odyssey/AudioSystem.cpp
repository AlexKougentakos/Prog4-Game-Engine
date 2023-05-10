#include "AudioSystem.h"
#include <iostream>
#include <SDL_mixer.h>

namespace ody
{
	class AudioSystem::AudioSystemImpl final
	{
	public:
		//Todo: implement this class
		void PlaySound(SoundEffect soundEffect)
		{
			Mix_Chunk* effect1{};
			Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096);

			//If the sound effect doesn't exist in the map we return to avoid errors
			if (!m_SoundEffectLocationMap.count(soundEffect))
			{
				LOG_ERROR("You tried to play a non assigned sound.");
				return;
			}

			std::string fileName = std::string(m_SoundEffectLocationMap[soundEffect]);

			//todo: pass this job to the reousrce manager
			effect1 = Mix_LoadWAV(("../Data/" + fileName).c_str());

			Mix_PlayChannel(-1, effect1, 0);

			LOG_SOUND("Played Sound: " + fileName);
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
	private:
		std::map<SoundEffect, std::string> m_SoundEffectLocationMap
		{
			{SoundEffect::TEST, "test.wav"}
		};
	};

	AudioSystem::AudioSystem()
	{
		pImpl = new AudioSystemImpl();
	}

	AudioSystem::~AudioSystem()
	{
		delete pImpl;
	}

	void AudioSystem::PlaySound(SoundEffect soundEffect)
	{
		pImpl->PlaySound(soundEffect);
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