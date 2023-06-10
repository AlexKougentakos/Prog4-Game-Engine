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