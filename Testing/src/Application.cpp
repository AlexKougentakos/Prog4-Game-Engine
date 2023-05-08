#if _DEBUG
#if __has_include(<vld.h>)
#include <vld.h>
#endif
#endif

#include "Odyssey.h"
#include "SceneManager.h"
#include "ResourceManager.h"
#include "TransformComponent.h"
#include "TextureComponent.h"
#include "TextComponent.h"
#include "FPSCounter.h"
#include "Command.h"

#include "../src/Scenes/TestScene.h"
#include "../src/Scenes/TestScene2.h"
#include "../src/Scenes/TestScene3.h"



void load()
{
	auto& sceneManager = ody::SceneManager::GetInstance();

	//auto scene = new TestScene();

	//sceneManager.AddScene(scene);

	//auto scene2 = new TestScene2();

	//sceneManager.AddScene(scene2);

	auto scene3 = new TestScene3();

	sceneManager.AddScene(scene3);
}

#include <Windows.h>
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

int main(int, char* []) 
{
	std::string relativeDataPath{};

	if (IsDebuggerPresent())
		relativeDataPath = "../Data/";
	else relativeDataPath = "../../../Data/";

	ody::Minigin engine(relativeDataPath);
	engine.Run(load);
	return 0;
}