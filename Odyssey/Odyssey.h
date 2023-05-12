#pragma once
#include <string>
#include <functional>
#include <map>
#include <memory>

namespace ody
{
	class AudioSystem;

	class Minigin final
	{
	public:
		explicit Minigin(const std::string& dataPath, std::map<unsigned int, std::string> SfxLocationMap);
		~Minigin();
		void Run(const std::function<void()>& load);

		Minigin(const Minigin& other) = delete;
		Minigin(Minigin&& other) = delete;
		Minigin& operator=(const Minigin& other) = delete;
		Minigin& operator=(Minigin&& other) = delete;
	private:
		std::unique_ptr<AudioSystem> m_pAudioSystem;
	};
}
