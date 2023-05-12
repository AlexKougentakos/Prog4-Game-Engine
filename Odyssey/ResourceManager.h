#pragma once
#include <map>
#include <string>
#include <memory>
#include <vector>

#include "Singleton.h"

namespace ody
{
	class Texture2D;
	class Font;
	class Sound;
	class Music;

	class ResourceManager final : public Singleton<ResourceManager>
	{
	public:
		void Init(const std::string& data);
		[[nodiscard]] std::shared_ptr<Texture2D> LoadTexture(const std::string& file) const;
		[[nodiscard]] std::shared_ptr<Font> LoadFont(const std::string& file, unsigned int size) const;
		[[nodiscard]] std::shared_ptr<Music> LoadMusic(const std::string& file) const;
		[[nodiscard]] std::shared_ptr<Sound> LoadSoundEffect(const std::string& file);

	private:
		friend class Singleton<ResourceManager>;
		ResourceManager() = default;
		std::string m_DataPath;
		std::map<std::string, std::shared_ptr<Sound>> m_LoadedAudios{};
	};
}
