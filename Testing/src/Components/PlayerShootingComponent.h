#pragma once
#include "Component.h"

namespace ody
{
	class StateManager;
}

class PlayerShootingComponent : public ody::Component
{
public:
	PlayerShootingComponent(ody::StateManager* pStateManager);
	~PlayerShootingComponent() override = default;
	PlayerShootingComponent(const PlayerShootingComponent& other) = delete;
	PlayerShootingComponent(PlayerShootingComponent&& other) = delete;
	PlayerShootingComponent& operator=(const PlayerShootingComponent& other) = delete;
	PlayerShootingComponent& operator=(PlayerShootingComponent&& other) = delete;

	void Update() override;
	void Shoot();

private:
	float m_ShootingCooldown{1.f};
	float m_TimeSinceLastShot{ 0.f };

	bool m_SwitchedAnim{ false };

	ody::StateManager* m_pStateManager;
};
