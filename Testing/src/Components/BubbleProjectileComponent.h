#pragma once
#include "Component.h"
#include "glm/vec2.hpp"


class BubbleProjectileComponent : public ody::Component
{
public:
	BubbleProjectileComponent(const glm::vec2& position, const glm::vec2& direction);
	~BubbleProjectileComponent() override = default;
	BubbleProjectileComponent(const BubbleProjectileComponent& other) = delete;
	BubbleProjectileComponent(BubbleProjectileComponent&& other) = delete;
	BubbleProjectileComponent& operator=(const BubbleProjectileComponent& other) = delete;
	BubbleProjectileComponent& operator=(BubbleProjectileComponent&& other) = delete;

	void Initialize() override;
	void Update() override;

	void Destroy();

private:
	float m_MoveSpeed{110.f};
	float m_MoveDistance{100.f};

	glm::vec2 m_StartingPosition{};
	glm::vec2 m_MoveDirection{};

	const float m_MaxTimeAlive{5.f};
	float m_TimeAlive{};
};
