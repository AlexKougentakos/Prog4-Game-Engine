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
    const std::map<unsigned int, std::pair<std::string, bool>> SfxLocationMap = 
    {
        {0, {"Sounds/ButtonClick.mp3", true}},
        {1, {"Sounds/CardThrow1.mp3", true}},
        {2, {"Sounds/CardThrow2.mp3", true}},
        {3, {"Sounds/CardThrow3.mp3", true}},
        {4, {"Sounds/CardThrow4.mp3", true}},
        {5, {"Sounds/CardThrow5.mp3", true}},
        {6, {"Sounds/CardThrow6.mp3", true}},
        {7, {"Sounds/Dragon.mp3", true}},
        {8, {"Sounds/Pass.mp3", true}},
        {9, {"Sounds/Phoenix.mp3", true}},
        {10, {"Sounds/RoundStart.mp3", true}},
        {11, {"Sounds/Tichu.mp3", true}},
        {12, {"Sounds/YourTurn.mp3", true}}
    };

    ody::Odyssey engine(data_location.string(), SfxLocationMap);
    engine.Run(load);
    return 0;
}