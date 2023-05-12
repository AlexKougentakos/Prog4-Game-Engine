#pragma once
#include <string>

//Todo: fix this forward declaration issue
#include <SDL_mixer.h>
//struct Mix_Music;
//struct Mix_Chunk;

//Simple RAII wrappers for SDL_mixer sfx & music structs
namespace ody
{
	class Sound final
	{
	public:
		Mix_Chunk* GetSoundEffect() const;
		explicit Sound(const std::string& fullPath, bool keepLoaded = false);
		~Sound();

		Sound(const Sound&) = delete;
		Sound(Sound&&) = delete;
		Sound& operator= (const Sound&) = delete;
		Sound& operator= (const Sound&&) = delete;

		bool KeepLoaded() const { return m_KeepLoaded; }
	private:
		Mix_Chunk* m_pMixChunk;
		bool m_KeepLoaded;
	};

	class Music final
	{
	public:
		Mix_Music* GetMusic() const;
		explicit Music(const std::string& fullPath);
		~Music();

		Music(const Music&) = delete;
		Music(Music&&) = delete;
		Music& operator= (const Music&) = delete;
		Music& operator= (const Music&&) = delete;
	private:
		Mix_Music* m_pMusic;
	};
}

