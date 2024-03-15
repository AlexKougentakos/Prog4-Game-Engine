#ifdef __EMSCRIPTEN__
#include "WebAudioSystem.h"
#include <iostream>
#include <SDL_mixer.h>
#include <queue>

#include "Audio.h"
#include "ResourceManager.h"
#include "SDL_AudioPlayer.h"

namespace ody
{
	class WebAudioSystem::WebAudioSystemImpl final
	{
	public:
		WebAudioSystemImpl()
		{
			printf("WebAudioSystemImpl created\n");
			Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096);
			m_pSoundEffect = Mix_LoadWAV("test.wav");
			if (m_pSoundEffect == nullptr)
				throw std::runtime_error(std::string("Failed to load sound: ") + Mix_GetError());
		}

		~WebAudioSystemImpl()
		{

		}

		void PlaySound(unsigned int effectID)
		{
			if (effectID == 923474) return;

			Mix_PlayChannel(-1, m_pSoundEffect, 0);
			//m_AudioPlayer.play("my_music", MAX_VOLUME, 0);
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
		//SDL_AudioPlayer m_AudioPlayer;
		Mix_Chunk* m_pSoundEffect{};

		void AudioWorker()
		{
		}
	};

	WebAudioSystem::WebAudioSystem(const std::map<unsigned int, std::pair<std::string, bool>>& effectLocationMap)
	{
		pImpl = new WebAudioSystemImpl();
		pImpl->SetSoundEffectLocationMap(effectLocationMap);
	}

	WebAudioSystem::~WebAudioSystem()
	{
		delete pImpl;
	}


	void WebAudioSystem::PlaySound(unsigned int effectID)
	{
		pImpl->PlaySound(effectID);
	}

	void WebAudioSystem::PauseSound()
	{
		pImpl->PauseSound();
	}

	void WebAudioSystem::StopSound()
	{
		pImpl->StopSound();
	}

	void WebAudioSystem::StopAllSounds()
	{
		pImpl->StopAllSounds();
	}
}
#endif