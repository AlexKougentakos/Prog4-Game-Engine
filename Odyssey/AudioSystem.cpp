#include "AudioSystem.h"
#include <iostream>
#include <SDL_mixer.h>

#include "Audio.h"
#include "ResourceManager.h"

namespace ody
{
	class AudioSystem::AudioSystemImpl final
	{
	public:
		void PlaySound(unsigned int effectID)
		{
			Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096);
			
			//If the sound effect doesn't exist in the map we return to avoid errors
			if (!m_SoundEffectLocationMap.contains(effectID))
			{
				LOG_ERROR("You tried to play a non assigned sound.");
				return;
			}
			
			const std::string fileName = std::string(m_SoundEffectLocationMap[effectID].first);

			const auto effect = ResourceManager::GetInstance().LoadSoundEffect(fileName);

			Mix_PlayChannel(-1, effect->GetSoundEffect(), 0);

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

		void SetSoundEffectLocationMap(const 
			std::map<unsigned int, std::pair<std::string, bool>>& soundEffectLocationMap)
		{
			m_SoundEffectLocationMap = soundEffectLocationMap;
		}
	private:
		std::map<unsigned int, std::pair<std::string, bool>> m_SoundEffectLocationMap;
	};

	AudioSystem::AudioSystem(const std::map<unsigned int, std::pair<std::string, bool>>& effectLocationMap)
	{
		pImpl = new AudioSystemImpl();
		pImpl->SetSoundEffectLocationMap(effectLocationMap);
	}

	AudioSystem::~AudioSystem()
	{
		delete pImpl;
	}


	void AudioSystem::PlaySound(unsigned int effectID)
	{
		pImpl->PlaySound(effectID);
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
