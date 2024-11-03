#pragma once
#include "Command.h"

class HumanPlayer;

class CardSelectCommand : public ody::Command
{
public:
	CardSelectCommand(HumanPlayer* pHumanPlayer);

	virtual ~CardSelectCommand() override = default;
	CardSelectCommand(const CardSelectCommand& other) = delete;
	CardSelectCommand(CardSelectCommand&& other) = delete;
	CardSelectCommand& operator=(const CardSelectCommand& other) = delete;
	CardSelectCommand& operator=(CardSelectCommand&& other) = delete;

	virtual void Execute() override;
private:
	HumanPlayer* m_pHumanPlayer{};	
};
