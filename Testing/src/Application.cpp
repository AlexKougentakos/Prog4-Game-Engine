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
#include "Command.h"

#include "Scenes/TichuScene.h"

#include <filesystem>
namespace fs = std::filesystem;


void load()
{
	auto& sceneManager = ody::SceneManager::GetInstance();

	const auto pTichuScene = new TichuScene();

	sceneManager.AddScene(pTichuScene);
}

int main(int, char* [])
{
#ifdef __EMSCRIPTEN__
    fs::path data_location = "";
#else
    fs::path data_location = "./Data/";
#endif
    //Here we define the sound effects that we want to use in our game
    const std::map<unsigned int, std::pair<std::string, bool>> SfxLocationMap = {};
    //{
    //  {1, {"test.wav", true}},
    //  {2, {"test2.wav", true}},
    //  {4, {"test3.wav", true}},
    //  {5, {"test4.wav", true}},
    //  {6, {"test5.wav", true}},
    //  {7, {"test6.wav", true}},
    //  {8, {"test7.wav", true}},
    //  {9, {"test8.wav", true}},
    //  {3, {"test9.wav", true}},
    //  {10, {"test10.wav", true}}
    //};

    ody::Odyssey engine(data_location.string(), SfxLocationMap);
    engine.Run(load);
    return 0;
}