#pragma once
#include "Command.h"

class StopMoveCommand : public ody::Command
{
public:
	StopMoveCommand(ody::GameObject* pActor) : m_pActor{ pActor } {}

	virtual ~StopMoveCommand() override = default;
	StopMoveCommand(const StopMoveCommand& other) = delete;
	StopMoveCommand(StopMoveCommand&& other) = delete;
	StopMoveCommand& operator=(const StopMoveCommand& other) = delete;
	StopMoveCommand& operator=(StopMoveCommand&& other) = delete;

	virtual void Execute() override;
private:
	ody::GameObject* m_pActor{};

};

class MoveCommand final : public ody::Command
{
public:
	MoveCommand(ody::GameObject* pActor, const glm::vec2* moveDir) : m_pActor{ pActor }, m_MoveDirectionRef{ moveDir }, m_UseRef(true) {}
	MoveCommand(ody::GameObject* pActor, const glm::vec2& moveDir) : m_pActor{ pActor }, m_MoveDirection{ moveDir } {}

	virtual ~MoveCommand() override = default;
	MoveCommand(const MoveCommand& other) = delete;
	MoveCommand(MoveCommand&& other) = delete;
	MoveCommand& operator=(const MoveCommand& other) = delete;
	MoveCommand& operator=(MoveCommand&& other) = delete;

	virtual void Execute() override;
private:
	ody::GameObject* m_pActor{};
	const glm::vec2* m_MoveDirectionRef{};
	const glm::vec2 m_MoveDirection{};

	bool m_UseRef{ false };
};

class JumpCommand : public ody::Command
{
public:
	JumpCommand(ody::GameObject* pActor, float jumpForce) : m_pActor{ pActor }, m_JumpForce{jumpForce} {}

	virtual ~JumpCommand() override = default;
	JumpCommand(const JumpCommand& other) = delete;
	JumpCommand(JumpCommand&& other) = delete;
	JumpCommand& operator=(const JumpCommand& other) = delete;
	JumpCommand& operator=(JumpCommand&& other) = delete;

	virtual void Execute() override;
private:
	ody::GameObject* m_pActor{};
	float m_JumpForce{};
};

class ShootBubbleCommand : public ody::Command
{
public:
	ShootBubbleCommand(ody::GameObject* pActor) : m_pActor{ pActor }{}

	virtual ~ShootBubbleCommand() override = default;
	ShootBubbleCommand(const ShootBubbleCommand& other) = delete;
	ShootBubbleCommand(ShootBubbleCommand&& other) = delete;
	ShootBubbleCommand& operator=(const ShootBubbleCommand& other) = delete;
	ShootBubbleCommand& operator=(ShootBubbleCommand&& other) = delete;

	virtual void Execute() override;
private:
	ody::GameObject* m_pActor{};
};