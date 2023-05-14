#include <SDL_mixer.h>
#include <stdexcept>

#include "Audio.h"

#include <iostream>


Mix_Chunk* ody::Sound::GetSoundEffect() const
{
	return m_pMixChunk;
}

ody::Sound::Sound(const std::string& fullPath)
{
	m_pMixChunk = Mix_LoadWAV(fullPath.c_str());

	if (m_pMixChunk == nullptr)
		throw std::runtime_error(std::string("Failed to load sound: ") + Mix_GetError());

	m_Size = m_pMixChunk->alen;
}

void ody::Sound::Release() const
{
	Mix_FreeChunk(m_pMixChunk);
}

Mix_Music* ody::Music::GetMusic() const
{
	return m_pMusic;
}

ody::Music::Music(const std::string& fullPath)
{
	m_pMusic = Mix_LoadMUS(fullPath.c_str());
	if (m_pMusic == nullptr)
		throw std::runtime_error(std::string("Failed to load music: ") + Mix_GetError());
}

ody::Music::~Music()
{
	Mix_FreeMusic(m_pMusic);
}
