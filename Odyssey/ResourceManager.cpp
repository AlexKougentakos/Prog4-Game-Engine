#include <stdexcept>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include "ResourceManager.h"

#include "Audio.h"
#include "Renderer.h"
#include "Texture2D.h"
#include "Font.h"

void ody::ResourceManager::Init(const std::string& dataPath)
{
	m_DataPath = dataPath;

	if (TTF_Init() != 0)
	{
		throw std::runtime_error(std::string("Failed to load support for fonts: ") + SDL_GetError());
	}
}

std::shared_ptr<ody::Texture2D> ody::ResourceManager::LoadTexture(const std::string& file) const
{
    const auto fullPath = m_DataPath + file;

    // Load the image
    SDL_Surface* surface = IMG_Load(fullPath.c_str());

    if (surface == nullptr)
    {
        throw std::runtime_error(std::string("Failed to load image: ") + IMG_GetError());
    }

    //todo: upgrade to OpenGL3 and remove this part.
    //Check if we need to add an alpha channel
    if (surface->format->BitsPerPixel == 32)
    {
        auto texture = IMG_LoadTexture(Renderer::GetInstance().GetSDLRenderer(), fullPath.c_str());
        return std::make_shared<Texture2D>(texture);
    }

    // Create a new surface with an alpha channel
    SDL_Surface* newSurface = SDL_CreateRGBSurfaceWithFormat(0, surface->w, surface->h, 32, SDL_PIXELFORMAT_RGBA32);
    if (newSurface == nullptr)
    {
        throw std::runtime_error(std::string("Failed to create new surface: ") + SDL_GetError());
    }

    // Blit the loaded image onto the new surface
    SDL_BlitSurface(surface, nullptr, newSurface, nullptr);

    // Create an SDL_Texture from the new surface
    SDL_Texture* texture = SDL_CreateTextureFromSurface(Renderer::GetInstance().GetSDLRenderer(), newSurface);
    if (texture == nullptr)
    {
        throw std::runtime_error(std::string("Failed to create texture: ") + SDL_GetError());
    }

    // Free the surfaces
    SDL_FreeSurface(surface);
    SDL_FreeSurface(newSurface);

    return std::make_shared<Texture2D>(texture);
}

std::shared_ptr<ody::Font> ody::ResourceManager::LoadFont(const std::string& file, unsigned int size) const
{
	return std::make_shared<Font>(m_DataPath + file, size);
}

std::shared_ptr<ody::Music> ody::ResourceManager::LoadMusic(const std::string& file) const
{
    return std::make_shared<Music>(m_DataPath + file);
}

std::shared_ptr<ody::Sound> ody::ResourceManager::LoadSoundEffect(const std::string& file, const bool keepLoaded)
{
    // Check if the sound effect is already loaded
    const auto it = m_LoadedAudios.find(file);
    if (it != m_LoadedAudios.end())
        return std::dynamic_pointer_cast<Sound>(it->second);

    auto sound = std::make_shared<Sound>(m_DataPath + file, keepLoaded);

    if (sound->KeepLoaded())
        m_LoadedAudios.insert(std::make_pair(file, sound));
    
    return sound;
}
