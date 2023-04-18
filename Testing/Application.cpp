#if _DEBUG
#if __has_include(<vld.h>)
#include <vld.h>
#endif
#endif

#include "Odyssey.h"
#include "SceneManager.h"
#include "ResourceManager.h"
#include "Scene.h"
#include "TransformComponent.h"
#include "TextureComponent.h"
#include "TextComponent.h"
#include "FPSCounter.h"
#include "Command.h"
#include "InputManager.h"

void load()
{
	auto& scene = dae::SceneManager::GetInstance().CreateScene("Demo");

	//auto go = new dae::GameObject{};


	auto gameObject = scene.CreateGameObject();
	gameObject->GetTransform()->SetPosition(0.0f, 0.0f);
	gameObject->AddComponent<ody::TextureComponent>("background.tga");

	gameObject = new dae::GameObject{};
	gameObject->AddComponent<ody::TransformComponent>(216.f, 180.f);
	gameObject->AddComponent<ody::TextureComponent>("logo.tga");
	scene.AddGameObject(gameObject);

	auto font = dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 36);
	auto to = new dae::GameObject{};
	to->AddComponent<ody::TransformComponent>(80.f, 20.f);
	to->AddComponent<ody::TextComponent>("Programming 4 Assignment", font);
	scene.AddGameObject(to);

	gameObject = new dae::GameObject{};
	gameObject->AddComponent<ody::TextComponent>("0", font);
	gameObject->AddComponent<ody::TransformComponent>(10.f, 10.f);
	gameObject->AddComponent<ody::FPSCounter>();
	scene.AddGameObject(gameObject);

	auto pacman = new dae::GameObject{};
	pacman->AddComponent<ody::TransformComponent>(0.f, 0.f);
	pacman->AddComponent<ody::TextureComponent>("pacman.tga");

	auto ghost = new dae::GameObject{};
	ghost->AddComponent<ody::TransformComponent>(100.f, 100.f);
	ghost->AddComponent<ody::TextureComponent>("ghost.tga");

	ghost->SetParent(pacman);

	scene.AddGameObject(pacman);
	scene.AddGameObject(ghost);

	auto& input{ ody::InputManager::GetInstance() };

	const std::vector<unsigned int> character1Input{ 'd', 'a', 'w', 's' };
	input.BindKeyboardCommand('w', ody::InputManager::InputType::ONBUTTON
		, std::make_unique<ody::MoveCommand>(pacman, 100.f,
			glm::vec2{ 0, -1 }));
}


int main(int, char* []) {
	dae::Minigin engine("../Data/");
	engine.Run(load);
	return 0;
}