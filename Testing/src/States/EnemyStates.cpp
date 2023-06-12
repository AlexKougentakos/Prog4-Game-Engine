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
	m_pGameObject->GetComponent<EnemyComponent>()->BecomeBubble();
	m_pGameObject->GetComponent<EnemyComponent>()->ResetTimer();
}

void EnemyBubble::Exit()
{
	
}

void EnemyBubble::Update()
{
	m_pGameObject->GetComponent<EnemyComponent>()->UpdateBubble();
}
#pragma endregion EnemyBubble

#pragma region EnemyRage
EnemyRage::EnemyRage(ody::GameObject* pGameObject) :
	m_pGameObject(pGameObject)
{}

void EnemyRage::Enter()
{
	m_pGameObject->GetComponent<EnemyComponent>()->BecomeEnraged();
}

void EnemyRage::Exit()
{

}

void EnemyRage::Update()
{
	m_pGameObject->GetComponent<EnemyComponent>()->UpdateRage();
}
#pragma endregion EnemyRage
