#include "PlayerShootingComponent.h"
#include "glm/vec2.hpp"
#include "GameObject.h"
#include "GameScene.h"
#include "../Prefabs/BubblePrefab.h"
#include "GameTime.h"
#include "PlayerMovementComponent.h"
#include "StateManager.h"
#include "../States/PlayerStates.h"

PlayerShootingComponent::PlayerShootingComponent(ody::StateManager* pStateManager)
{
	m_pStateManager = pStateManager;
}

void PlayerShootingComponent::Shoot()
{
	//Here we would get the look dir of the player
	glm::vec2 lookDir = GetOwner()->GetComponent<PlayerMovementComponent>()->GetLookDir();

	if (m_TimeSinceLastShot >= m_ShootingCooldown)
	{
		m_pStateManager->ChangeState(std::make_unique<PlayerShoot>(GetOwner()));
		BubblePrefab bubblePrefab{ GetOwner()->GetTransform()->GetWorldPosition(),lookDir };
		GetOwner()->GetScene()->CreateGameObjectFromPrefab(bubblePrefab);
		m_TimeSinceLastShot = 0.f;

		m_SwitchedAnim = false;
	}
}

void PlayerShootingComponent::Update()
{
	m_TimeSinceLastShot += ody::Time::GetInstance().GetDeltaTime();

	if (m_TimeSinceLastShot >= m_ShootingCooldown / 3.f && !m_SwitchedAnim)
	{
		m_SwitchedAnim = true;
		m_pStateManager->ChangeState(std::make_unique<PlayerIdle>(GetOwner()));
	}
}