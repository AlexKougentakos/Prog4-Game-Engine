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
#include "InputManager.h"
#include "../src/Scenes/TestScene.h"

void load()
{
	auto& sceneManager = ody::SceneManager::GetInstance();

	auto scene = new TestScene();

	sceneManager.AddScene(scene);
}


int main(int, char* []) {
	ody::Minigin engine("../Data/");
	engine.Run(load);
	return 0;
}