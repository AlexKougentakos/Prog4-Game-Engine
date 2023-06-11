#include "PlayerStates.h"

#include <iostream>

#include "GameObject.h"

#pragma region PlayerIdle
PlayerIdle::PlayerIdle(ody::GameObject* pGameObject):
	m_pGameObject(pGameObject)
{}

void PlayerIdle::Enter()
{
	std::cout << "PlayerIdle::Enter()" << std::endl;
}

void PlayerIdle::Exit()
{
	std::cout << "PlayerIdle::Exit()" << std::endl;
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
	std::cout << "PlayerWalk::Enter()" << std::endl;
}

void PlayerWalk::Exit()
{
	std::cout << "PlayerWalk::Exit()" << std::endl;
}

void PlayerWalk::Update()
{

}
#pragma endregion PlayerWalk