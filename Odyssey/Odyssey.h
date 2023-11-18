#pragma once
#include <string>
#include <functional>
#include <map>
#include <memory>

namespace ody
{
	class AudioSystem;

	class Odyssey final
	{
	public:
		/**
		 * \brief 
		 * \param dataPath Location for the data folder
		 * \param SfxLocationMap Map which contains the ID, location and if the sound should be pre-loaded and kept in memory
		 */
		explicit Odyssey(const std::string& dataPath, 
		                 std::map<unsigned int, std::pair<std::string, bool>> SfxLocationMap);
		~Odyssey();
		void Run(const std::function<void()>& load);

		Odyssey(const Odyssey& other) = delete;
		Odyssey(Odyssey&& other) = delete;
		Odyssey& operator=(const Odyssey& other) = delete;
		Odyssey& operator=(Odyssey&& other) = delete;
	private:

		//Services for ServiceProvider
		std::map<unsigned int, std::pair<std::string, bool>> m_SfxLocationMap;
		std::unique_ptr<AudioSystem> m_pAudioSystem;
	};
}
