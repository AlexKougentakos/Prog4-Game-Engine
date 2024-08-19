#include <stdexcept>

#ifndef __EMSCRIPTEN__
	#define WIN32_LEAN_AND_MEAN 
	#include <windows.h>
#else
#include "WebAudioSystem.h"
#endif

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

#include "Constants.h"

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

#ifdef __EMSCRIPTEN__
#include "emscripten.h"

void LoopCallback(void* arg)
{
	static_cast<ody::Odyssey*>(arg)->RunOneFrame();
}
#endif

ody::Odyssey::Odyssey(const std::string &dataPath, 
	std::map<unsigned int, std::pair<std::string, bool>> sfxLocationMap)
{

	m_SfxLocationMap = sfxLocationMap;
	PrintSDLVersion();
	
	if (SDL_Init(SDL_INIT_VIDEO) != 0) 
	{
		throw std::runtime_error(std::string("SDL_Init Error: ") + SDL_GetError());
	}

	g_window = SDL_CreateWindow(
		"Odyssey Engine - Tichu Recreation",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		constants::g_ScreenWidth,
		constants::g_ScreenHeight,
		SDL_WINDOW_OPENGL
	);
	if (g_window == nullptr)
	{
		throw std::runtime_error(std::string("SDL_CreateWindow Error: ") + SDL_GetError());
	}
	Renderer::GetInstance().Init(g_window);

	auto& resourceManager = ResourceManager::GetInstance();

	resourceManager.Init(dataPath);

#ifndef __EMSCRIPTEN__
	m_pAudioSystem = std::make_unique<ody::AudioSystem>(sfxLocationMap);
#else
	m_pAudioSystem = std::make_unique<ody::WebAudioSystem>(sfxLocationMap);
#endif
	ody::ServiceLocator::Provide(m_pAudioSystem.get());

	// Setup for resources and initial state
	std::vector<std::string> paths;
	for (const auto& audioFile : m_SfxLocationMap)
		paths.emplace_back(audioFile.second.first);
	ResourceManager::GetInstance().PreLoad(paths);

	m_pRenderer = &Renderer::GetInstance();
	m_pSceneManager = &SceneManager::GetInstance();
	m_pTime = &ody::Time::GetInstance();
	m_pInputManager = &InputManager::GetInstance();
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

	printf("Starting game loop edited string\n");
#ifdef __EMSCRIPTEN__
	// Use Emscripten's main loop mechanism
	emscripten_set_main_loop_arg(&LoopCallback, this, 0, true);
	printf("Emscripten main loop set\n");
#else
	// Traditional loop for non-Emscripten (desktop) builds
	m_LastTime = std::chrono::high_resolution_clock::now();
	constexpr int FPSLimit = 60;
	m_MaxWaitingTimeMs = 1000 / FPSLimit;
	while (m_DoContinue)
	{
		RunOneFrame();

		auto currentTime = std::chrono::high_resolution_clock::now();
		auto frameEndTime = m_LastTime + std::chrono::milliseconds(m_MaxWaitingTimeMs);
		if (currentTime < frameEndTime)
		{
			std::this_thread::sleep_until(frameEndTime);
		}
		m_LastTime = frameEndTime;
	}
#endif
}

void ody::Odyssey::RunOneFrame()
{
	const auto currentTime = std::chrono::high_resolution_clock::now();
	const float deltaTime = std::chrono::duration<float>(currentTime - m_LastTime).count();

	m_pTime->SetDeltaTime(deltaTime);
	m_DoContinue = m_pInputManager->ProcessInput();
	//m_pTime->SetDeltaTime(deltaTime);

	lag += deltaTime;
	while (lag >= m_PhysicsTimeStep)
	{
		// First time, lastTime is 0, which means deltaTime is very high, which messes this loop up
		if (deltaTime >= 1000.0f) lag = m_PhysicsTimeStep;
		m_pSceneManager->FixedUpdate();
		lag -= m_PhysicsTimeStep;
	}

	m_pSceneManager->Update();
	m_pRenderer->Render();

	m_LastTime = currentTime;

	//Reset at the end of the frame
	m_pInputManager->ResetMouseMotion();

	const auto sleepTime{ currentTime + std::chrono::milliseconds(m_MaxWaitingTimeMs) - std::chrono::high_resolution_clock::now() };
	std::this_thread::sleep_for(sleepTime);
}