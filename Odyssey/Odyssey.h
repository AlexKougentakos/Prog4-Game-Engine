#pragma once
#include <string>
#include <functional>
#include <map>
#include <memory>
#include <chrono>

namespace ody
{
	class IAudio;
	class InputManager;
	class Time;
	class SceneManager;
	class Renderer;

	class Odyssey final
	{
	public:
		/**
		 * \brief 
		 * \param dataPath Location for the data folder
		 * \param SfxLocationMap Map which contains the ID, location and if the sound should be pre-loaded and kept in memory
		 */
		explicit Odyssey(const std::string& dataPath, 
		                 std::map<unsigned int, std::pair<std::string, bool>> SfxLocationMap, const std::string& windowIconName = "");
		~Odyssey();
		void Run(const std::function<void()>& load);
		void RunOneFrame();

		Odyssey(const Odyssey& other) = delete;
		Odyssey(Odyssey&& other) = delete;
		Odyssey& operator=(const Odyssey& other) = delete;
		Odyssey& operator=(Odyssey&& other) = delete;
	private:

		//Services for ServiceProvider
		std::map<unsigned int, std::pair<std::string, bool>> m_SfxLocationMap;
		//std::unique_ptr<AudioSystem> m_pAudioSystem;

		//Update Loop Variables
		Renderer* m_pRenderer{};
		SceneManager* m_pSceneManager{};
		Time* m_pTime{};
		InputManager* m_pInputManager{};
		std::unique_ptr<IAudio> m_pAudioSystem{};

		int m_MaxWaitingTimeMs{};
		float lag{};
		const float m_PhysicsTimeStep{ 1.0f / 60.0f };
		std::chrono::steady_clock::time_point m_LastTime{};

		bool m_DoContinue{ true };
	};
}
