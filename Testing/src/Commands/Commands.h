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

