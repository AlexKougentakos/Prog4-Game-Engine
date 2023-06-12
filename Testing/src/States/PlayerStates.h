#pragma once
#include "State.h"

namespace ody
{
	class GameObject;
}

class PlayerIdle : public ody::State
{
public:
	PlayerIdle(ody::GameObject* pGameObject);
	virtual ~PlayerIdle() override = default;

	PlayerIdle(const PlayerIdle& other) = delete;
	PlayerIdle(PlayerIdle&& other) = delete;
	PlayerIdle& operator=(const PlayerIdle& other) = delete;
	PlayerIdle& operator=(PlayerIdle&& other) = delete;

	virtual void Enter() override;
	virtual void Exit() override;
	virtual void Update() override;

	virtual std::string GetStateName() override {return "PlayerIdle"; }
private:
	ody::GameObject* m_pGameObject{};
};

class PlayerWalk : public ody::State
{
public:
	PlayerWalk(ody::GameObject* pGameObject);
	virtual ~PlayerWalk() override = default;

	PlayerWalk(const PlayerWalk& other) = delete;
	PlayerWalk(PlayerWalk&& other) = delete;
	PlayerWalk& operator=(const PlayerWalk& other) = delete;
	PlayerWalk& operator=(PlayerWalk&& other) = delete;

	virtual void Enter() override;
	virtual void Exit() override;
	virtual void Update() override;

	virtual std::string GetStateName() override {return "PlayerWalk"; }
private:
	ody::GameObject* m_pGameObject{};
};

class PlayerShoot : public ody::State
{
public:
	PlayerShoot(ody::GameObject* pGameObject);
	virtual ~PlayerShoot() override = default;

	PlayerShoot(const PlayerShoot& other) = delete;
	PlayerShoot(PlayerShoot&& other) = delete;
	PlayerShoot& operator=(const PlayerShoot& other) = delete;
	PlayerShoot& operator=(PlayerShoot&& other) = delete;

	virtual void Enter() override;
	virtual void Exit() override;
	virtual void Update() override;

	virtual std::string GetStateName() override { return "PlayerShoot"; }
private:
	ody::GameObject* m_pGameObject{};

	const float m_AnimTime{ 0.5f };
	float m_ElapsedAnimTime{0.f};
};

