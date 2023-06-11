#include "EnemyStates.h"

#include <iostream>

#include "GameObject.h"

#include "../Components/EnemyComponent.h"


#pragma region EnemyWalk
EnemyWalk::EnemyWalk(ody::GameObject* pGameObject) :
	m_pGameObject(pGameObject)
{
}

void EnemyWalk::Enter()
{
	
}

void EnemyWalk::Exit()
{
	m_pGameObject->GetComponent<EnemyComponent>()->ResetTimer();
}

void EnemyWalk::Update()
{
	m_pGameObject->GetComponent<EnemyComponent>()->Wander();
}
#pragma endregion EnemyWalk

#pragma region EnemyBubble
EnemyBubble::EnemyBubble(ody::GameObject* pGameObject) :
	m_pGameObject(pGameObject)
{}

void EnemyBubble::Enter()
{
	std::cout << "EnemyBubble::Enter()" << std::endl;
	m_pGameObject->GetComponent<EnemyComponent>()->BecomeBubble();
}

void EnemyBubble::Exit()
{
	
}

void EnemyBubble::Update()
{
	m_pGameObject->GetComponent<EnemyComponent>()->UpdateBubble();
}
#pragma endregion EnemyBubble
