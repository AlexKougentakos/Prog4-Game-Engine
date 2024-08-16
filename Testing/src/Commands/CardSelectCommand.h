#pragma once
#include "Command.h"
#include "Components/PlayerComponent.h"

class CardSelectCommand : public ody::Command
{
public:
	CardSelectCommand(const std::vector<PlayerComponent*>& pPlayerComponents, const int& currentPlayerIndex);

	virtual ~CardSelectCommand() override = default;
	CardSelectCommand(const CardSelectCommand& other) = delete;
	CardSelectCommand(CardSelectCommand&& other) = delete;
	CardSelectCommand& operator=(const CardSelectCommand& other) = delete;
	CardSelectCommand& operator=(CardSelectCommand&& other) = delete;

	virtual void Execute() override;
private:
	std::vector<PlayerComponent*> m_pPlayerComponents{};	
	const int& m_CurrentPlayerIndex;
};
