#pragma once
#include <glm/vec2.hpp>

#include "IPrefab.h"


class EnemyPrefab final : public ody::IPrefab
{
public:

	EnemyPrefab(const glm::vec2& position, const glm::vec2& direction);
	virtual ~EnemyPrefab() override = default;

	EnemyPrefab(const EnemyPrefab& other) = delete;
	EnemyPrefab(EnemyPrefab&& other) = delete;
	EnemyPrefab& operator=(const EnemyPrefab& other) = delete;
	EnemyPrefab& operator=(EnemyPrefab&& other) = delete;

	virtual void Configure(ody::GameObject* gameObject) const override;
private:
	glm::vec2 m_MoveDirection{};
	glm::vec2 m_SpawnPosition{};
};

