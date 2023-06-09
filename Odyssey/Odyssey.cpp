#include <stdexcept>
#define WIN32_LEAN_AND_MEAN 
#include <windows.h>
#include <chrono>
#include <iostream>
#include <thread>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include "Odyssey.h"

#include "InputManager2.h"
#include "SceneManager.h"
#include "Renderer.h"
#include "ResourceManager.h"
#include "GameTime.h"
#include "AudioSystem.h"
#include "ServiceLocator.h"
#include "DebugRenderer.h"

SDL_Window* g_window{};

void PrintSDLVersion()
{
	SDL_version version{};
	SDL_VERSION(&version);
	printf("We compiled against SDL version %u.%u.%u ...\n",
		version.major, version.minor, version.patch);

	SDL_GetVersion(&version);
	printf("We are linking against SDL version %u.%u.%u.\n",
		version.major, version.minor, version.patch);

	SDL_IMAGE_VERSION(&version);
	printf("We compiled against SDL_image version %u.%u.%u ...\n",
		version.major, version.minor, version.patch);

	version = *IMG_Linked_Version();
	printf("We are linking against SDL_image version %u.%u.%u.\n",
		version.major, version.minor, version.patch);

	SDL_TTF_VERSION(&version);
	printf("We compiled against SDL_ttf version %u.%u.%u ...\n",
		version.major, version.minor, version.patch);

	version = *TTF_Linked_Version();
	printf("We are linking against SDL_ttf version %u.%u.%u.\n",
		version.major, version.minor, version.patch);

	SDL_MIXER_VERSION(&version);
	printf("We compiled against SDL_mixer version %u.%u.%u ...\n",
		version.major, version.minor, version.patch);

	version = *Mix_Linked_Version();
	printf("We are linking against SDL_mixer version %u.%u.%u.\n",
		version.major, version.minor, version.patch);
}

ody::Odyssey::Odyssey(const std::string &dataPath, 
	std::map<unsigned int, std::pair<std::string, bool>> SfxLocationMap)
{

	m_SfxLocationMap = SfxLocationMap;
	PrintSDLVersion();
	
	if (SDL_Init(SDL_INIT_VIDEO) != 0) 
	{
		throw std::runtime_error(std::string("SDL_Init Error: ") + SDL_GetError());
	}

	g_window = SDL_CreateWindow(
		"Odyssey Engine",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		640,
		480,
		SDL_WINDOW_OPENGL
	);
	if (g_window == nullptr) 
	{
		throw std::runtime_error(std::string("SDL_CreateWindow Error: ") + SDL_GetError());
	}

	Renderer::GetInstance().Init(g_window);

	auto& resourceManager = ResourceManager::GetInstance();

	resourceManager.Init(dataPath);

	m_pAudioSystem = std::make_unique<ody::AudioSystem>(SfxLocationMap);

	ody::ServiceLocator::Provide(m_pAudioSystem.get());

}

ody::Odyssey::~Odyssey()
{
	Renderer::GetInstance().Destroy();
	SDL_DestroyWindow(g_window);
	g_window = nullptr;
	SDL_Quit();
}

void ody::Odyssey::Run(const std::function<void()>& load)
{
	load();

	std::vector<std::string> paths{};

	for (const auto& audioFile : m_SfxLocationMap)
		paths.emplace_back(audioFile.second.first);

	ResourceManager::GetInstance().PreLoad(paths);

	const auto& renderer = Renderer::GetInstance();
	auto& sceneManager = SceneManager::GetInstance();
	auto& input = ody::InputManager::GetInstance();
	auto& time = ody::Time::GetInstance();

	auto& debugRenderer = ody::DebugRenderer::GetInstance();
	debugRenderer.SetRenderer(renderer.GetSDLRenderer());

	bool doContinue = true;
	std::chrono::steady_clock::time_point lastTime{ std::chrono::high_resolution_clock::now() };
	constexpr int FPSLimit{ 60 };
	constexpr int maxWaitingTimeMs{ static_cast<int>(1000 / FPSLimit) };
	while (doContinue)
	{
		auto currentTime = std::chrono::high_resolution_clock::now();
		const float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();

		time.SetDeltaTime(deltaTime);
		doContinue = input.ProcessInput();
		sceneManager.Update();
		renderer.Render();

		lastTime = currentTime;

		const auto sleepTime{ currentTime + std::chrono::milliseconds(maxWaitingTimeMs) - std::chrono::high_resolution_clock::now() };
		std::this_thread::sleep_for(sleepTime);
	}
}