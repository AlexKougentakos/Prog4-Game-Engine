#pragma once
#include "Component.h"

class PlayerShootingComponent : public ody::Component
{
public:
	PlayerShootingComponent() = default;
	~PlayerShootingComponent() override = default;
	PlayerShootingComponent(const PlayerShootingComponent& other) = delete;
	PlayerShootingComponent(PlayerShootingComponent&& other) = delete;
	PlayerShootingComponent& operator=(const PlayerShootingComponent& other) = delete;
	PlayerShootingComponent& operator=(PlayerShootingComponent&& other) = delete;

	//void Update() override;
	void Shoot() const;

private:
	float m_ShootingCooldown{1.f};
	float m_TimeSinceLastShot{ 0.f };
};
