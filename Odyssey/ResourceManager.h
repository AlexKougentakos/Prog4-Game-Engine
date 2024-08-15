#pragma once
#include <future>
#include <map>
#include <string>
#include <memory>
#include <mutex>
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
		~ResourceManager();

		//apply rule of 5
		ResourceManager(const ResourceManager& other) = delete;
		ResourceManager(ResourceManager&& other) = delete;
		ResourceManager& operator=(const ResourceManager& other) = delete;
		ResourceManager& operator=(ResourceManager&& other) = delete;

		void Init(const std::string& data);

		std::shared_ptr<Texture2D> LoadTexture(const std::string& file) const;
		std::shared_ptr<Font> LoadFont(const std::string& file, unsigned int size) const;
		std::shared_ptr<Music> LoadMusic(const std::string& file) const;
		std::shared_ptr<Sound> LoadSoundEffect(const std::string& file);

		std::string GetDatPath() const { return m_DataPath; }

		//Should only be called once, in the run function of the engine
		void PreLoad(const std::vector<std::string>& paths);

	private:
		friend class Singleton<ResourceManager>;
		ResourceManager() = default;

		static std::mutex m_MapMutex;
		std::vector<std::future<void>> m_Futures{};

		static void AsyncLoadSound(const std::string file, std::map<std::string, std::shared_ptr<Sound>>* map);

		std::string m_DataPath;

		std::map<std::string, std::shared_ptr<Sound>> m_CachedAudios{};
		std::map<std::string, std::shared_ptr<Sound>> m_PreLoadedAudios{};

		//These will be available to the user of the engine, right now it's proof of concept
		unsigned int m_MaxCachedAudioSize{10}; //In Megabytes
		unsigned int m_AudioClearSize{ 3 }; //The amount of audio files to clear when the max size is reached (In Megabytes)

		unsigned int GetLoadedAudiosSize() const;
	};
}
