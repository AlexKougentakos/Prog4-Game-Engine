#include "PlayerShootingComponent.h"
#include "glm/vec2.hpp"
#include "GameObject.h"
#include "GameScene.h"
#include "../Prefabs/BubblePrefab.h"
#include <iostream>

void PlayerShootingComponent::Shoot() const
{
	//Here we would get the look dir of the player
	glm::vec2 lookDir{-1, 0};
	std::cout << "Bubble Shot" << std::endl;
	BubblePrefab bubblePrefab{ GetOwner()->GetTransform()->GetWorldPosition(),lookDir};
	GetOwner()->GetScene()->CreateGameObjectFromPrefab(bubblePrefab);
}