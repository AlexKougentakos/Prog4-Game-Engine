#include "PlayerStates.h"

#include <iostream>

#include "GameTime.h"
#include "GameObject.h"
#include "AnimatedTextureComponent.h"

#pragma region PlayerIdle
PlayerIdle::PlayerIdle(ody::GameObject* pGameObject):
	m_pGameObject(pGameObject)
{}

void PlayerIdle::Enter()
{
	m_pGameObject->GetComponent<ody::AnimatedTextureComponent>()->SetTexture("Player_Idle_Anim.png", glm::ivec2{ 2,1 }, 0.5f, 0.7f);
}

void PlayerIdle::Exit()
{
	
}

void PlayerIdle::Update()
{
	
}
#pragma endregion PlayerIdle

#pragma region PlayerWalk
PlayerWalk::PlayerWalk(ody::GameObject* pGameObject) :
	m_pGameObject(pGameObject)
{

}

void PlayerWalk::Enter()
{
	m_pGameObject->GetComponent<ody::AnimatedTextureComponent>()->SetTexture("Player_Run_Anim.png", glm::ivec2{ 4,1 }, 0.2f, 0.7f);
}

void PlayerWalk::Exit()
{
	
}

void PlayerWalk::Update()
{
	
}
#pragma endregion PlayerWalk

#pragma region PlayerShoot
PlayerShoot::PlayerShoot(ody::GameObject* pGameObject) :
	m_pGameObject(pGameObject)
{

}

void PlayerShoot::Enter()
{
	m_ElapsedAnimTime = 0.f;
	m_pGameObject->GetComponent<ody::AnimatedTextureComponent>()->SetTexture("Player_Shoot_Anim.png", glm::ivec2{ 4,1 }, 0.2f, 0.7f);
}

void PlayerShoot::Exit()
{

}

void PlayerShoot::Update()
{
	
}
#pragma endregion PlayerShoot