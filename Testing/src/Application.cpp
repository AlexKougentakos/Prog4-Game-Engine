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
        {0, {"Sounds/ButtonClick.wav", false}},
        {1, {"Sounds/CardThrow1.wav", false}},
        {2, {"Sounds/CardThrow2.wav", false}},
        {3, {"Sounds/CardThrow3.wav", false}},
        {4, {"Sounds/CardThrow4.wav", false}},
        {5, {"Sounds/CardThrow5.wav", false}},
        {6, {"Sounds/CardThrow6.wav", false}},
        {7, {"Sounds/Dragon.wav", false}},
        {8, {"Sounds/Pass.wav", false}},
        {9, {"Sounds/Phoenix.wav", false}},
        {10, {"Sounds/RoundStart.wav", false}},
        {11, {"Sounds/Tichu.wav", false}},
        {12, {"Sounds/YourTurn.wav", true}}
    };

    ody::Odyssey engine(data_location.string(), SfxLocationMap, "Icon.png");
    engine.Run(load);
    return 0;
}