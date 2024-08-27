#ifdef __EMSCRIPTEN__
#include "WebAudioSystem.h"
#include <iostream>
#include <SDL_mixer.h>
#include <queue>
#include <stdexcept>

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
			// Initialize SDL_Mixer for audio
			std::cout << "Initializing SDL_Mixer...\n";
			if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1)
			{
				std::cerr << "Failed to initialize SDL_Mixer: " << Mix_GetError() << std::endl;
				return;
			}
			std::cout << "SDL_Mixer initialized successfully.\n";
		}

		~WebAudioSystemImpl()
		{
			// Clean up audio resources
			std::cout << "Cleaning up audio resources...\n";
			for (auto& soundPair : m_SoundEffectMap)
			{
				if (soundPair.second != nullptr)
				{
					std::cout << "Freeing sound: " << soundPair.first << "\n";
					Mix_FreeChunk(soundPair.second);
				}
			}
			Mix_CloseAudio();
		}

		void PlaySound(unsigned int effectID)
		{
			std::cout << "Attempting to play sound with effectID: " << effectID << "\n";

			// Check if sound is already loaded
			auto it = m_SoundEffectMap.find(effectID);
			if (it == m_SoundEffectMap.end()) {
				// Sound is not loaded, check if it needs to be loaded now (on-demand loading)
				auto soundEntry = m_SoundEffectLocationMap.find(effectID);
				if (soundEntry != m_SoundEffectLocationMap.end()) {
					if (!soundEntry->second.second) {
						std::cout << "Sound not preloaded, loading on-demand: " << soundEntry->second.first << "\n";
						LoadSound(effectID, soundEntry->second.first);
					}
				}
				else {
					std::cerr << "Sound effectID not found in location map: " << effectID << std::endl;
					return;
				}
			}

			// Now play the sound (either preloaded or on-demand loaded)
			it = m_SoundEffectMap.find(effectID);
			if (it != m_SoundEffectMap.end() && it->second != nullptr)
			{
				std::cout << "Sound found. Playing...\n";
				Mix_PlayChannel(-1, it->second, 0);
			}
			else
			{
				std::cerr << "Failed to play sound, effectID: " << effectID << std::endl;
			}
		}

		void PauseSound()
		{
			std::cout << "Pausing all sounds...\n";
			Mix_Pause(-1);
		}

		void StopSound()
		{
			std::cout << "Stopping all sounds...\n";
			Mix_HaltChannel(-1);
		}

		void StopAllSounds()
		{
			std::cout << "Stopping all sounds on all channels...\n";
			Mix_HaltChannel(-1);
		}

		void SetSoundEffectLocationMap(const std::map<unsigned int, std::pair<std::string, bool>>& soundEffectLocationMap)
		{
			std::cout << "Setting sound effect location map...\n";
			m_SoundEffectLocationMap = soundEffectLocationMap;
			PreloadSounds();
		}

	private:
		std::map<unsigned int, std::pair<std::string, bool>> m_SoundEffectLocationMap;  // Maps effectID to file path and whether to preload
		std::map<unsigned int, Mix_Chunk*> m_SoundEffectMap;  // Maps effectID to loaded Mix_Chunk*

		// Function to preload sounds that are marked as "preload" in the map
		void PreloadSounds()
		{
			std::cout << "Preloading sound effects...\n";
			for (const auto& soundEntry : m_SoundEffectLocationMap)
			{
				if (soundEntry.second.second)  // Preload only if the bool is true
				{
					std::cout << "Preloading sound file for effectID: " << soundEntry.first << " from path: " << soundEntry.second.first << "\n";
					LoadSound(soundEntry.first, soundEntry.second.first);
				}
			}
		}

		// Function to load a sound into memory
		void LoadSound(unsigned int effectID, const std::string& soundFilePath)
		{
			std::cout << "Loading sound file for effectID: " << effectID << " from path: " << soundFilePath << "\n";
			Mix_Chunk* chunk = Mix_LoadWAV(soundFilePath.c_str());
			if (chunk == nullptr)
			{
				std::cerr << "Failed to load sound: " << soundFilePath << ", Error: " << Mix_GetError() << std::endl;
				return;
			}
			m_SoundEffectMap[effectID] = chunk;
			std::cout << "Sound loaded successfully for effectID: " << effectID << "\n";
		}

		void AudioWorker()
		{
			// Placeholder for asynchronous audio handling if needed in the future
		}
	};

	WebAudioSystem::WebAudioSystem(const std::map<unsigned int, std::pair<std::string, bool>>& effectLocationMap)
	{
		std::cout << "Creating WebAudioSystem...\n";
		pImpl = new WebAudioSystemImpl();
		pImpl->SetSoundEffectLocationMap(effectLocationMap);
	}

	WebAudioSystem::~WebAudioSystem()
	{
		std::cout << "Destroying WebAudioSystem...\n";
		delete pImpl;
	}

	void WebAudioSystem::PlaySound(unsigned int effectID)
	{
		std::cout << "WebAudioSystem PlaySound called for effectID: " << effectID << "\n";
		pImpl->PlaySound(effectID);
	}

	void WebAudioSystem::PauseSound()
	{
		std::cout << "WebAudioSystem PauseSound called\n";
		pImpl->PauseSound();
	}

	void WebAudioSystem::StopSound()
	{
		std::cout << "WebAudioSystem StopSound called\n";
		pImpl->StopSound();
	}

	void WebAudioSystem::StopAllSounds()
	{
		std::cout << "WebAudioSystem StopAllSounds called\n";
		pImpl->StopAllSounds();
	}
}
#endif
