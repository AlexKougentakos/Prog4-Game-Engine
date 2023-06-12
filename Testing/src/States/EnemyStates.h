#pragma once

#include "State.h"

namespace ody
{
	class GameObject;
}

class EnemyWalk : public ody::State
{
public:
	EnemyWalk(ody::GameObject* pGameObject);
	virtual ~EnemyWalk() override = default;

	EnemyWalk(const EnemyWalk& other) = delete;
	EnemyWalk(EnemyWalk&& other) = delete;
	EnemyWalk& operator=(const EnemyWalk& other) = delete;
	EnemyWalk& operator=(EnemyWalk&& other) = delete;

	virtual void Enter() override;
	virtual void Exit() override;
	virtual void Update() override;

	virtual std::string GetStateName() override { return "EnemyWalk"; }
private:
	ody::GameObject* m_pGameObject{};
};

class EnemyJump : public ody::State
{
public:
	EnemyJump(ody::GameObject* pGameObject);
	virtual ~EnemyJump() override = default;

	EnemyJump(const EnemyJump& other) = delete;
	EnemyJump(EnemyJump&& other) = delete;
	EnemyJump& operator=(const EnemyJump& other) = delete;
	EnemyJump& operator=(EnemyJump&& other) = delete;

	virtual void Enter() override;
	virtual void Exit() override;
	virtual void Update() override;

	virtual std::string GetStateName() override { return "EnemyJump"; }
private:
	ody::GameObject* m_pGameObject{};
};

class EnemyBubble : public ody::State
{
public:
	EnemyBubble(ody::GameObject* pGameObject);
	virtual ~EnemyBubble() override = default;

	EnemyBubble(const EnemyBubble& other) = delete;
	EnemyBubble(EnemyBubble&& other) = delete;
	EnemyBubble& operator=(const EnemyBubble& other) = delete;
	EnemyBubble& operator=(EnemyBubble&& other) = delete;

	virtual void Enter() override;
	virtual void Exit() override;
	virtual void Update() override;

	virtual std::string GetStateName() override { return "EnemyBubble"; }
private:
	ody::GameObject* m_pGameObject{};
};

class EnemyRage : public ody::State
{
public:
	EnemyRage(ody::GameObject* pGameObject);
	virtual ~EnemyRage() override = default;

	EnemyRage(const EnemyRage& other) = delete;
	EnemyRage(EnemyRage&& other) = delete;
	EnemyRage& operator=(const EnemyRage& other) = delete;
	EnemyRage& operator=(EnemyRage&& other) = delete;

	virtual void Enter() override;
	virtual void Exit() override;
	virtual void Update() override;

	virtual std::string GetStateName() override { return "EnemyRage"; }
private:
	ody::GameObject* m_pGameObject{};
};