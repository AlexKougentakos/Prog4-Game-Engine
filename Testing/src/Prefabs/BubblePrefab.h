#pragma once
#include "IPrefab.h"
#include "glm/vec2.hpp"

class BubblePrefab final : public ody::IPrefab
{
public:

	BubblePrefab(const glm::vec2& position, const glm::vec2& direction);
	virtual ~BubblePrefab() override = default;

	BubblePrefab(const BubblePrefab& other) = delete;
	BubblePrefab(BubblePrefab&& other) = delete;
	BubblePrefab& operator=(const BubblePrefab& other) = delete;
	BubblePrefab& operator=(BubblePrefab&& other) = delete;

	virtual void Configure(ody::GameObject* gameObject) const override;
private:
	glm::vec2 m_MoveDirection{};
	glm::vec2 m_SpawnPosition{};
};

