#include "CardSelectCommand.h"
#include "InputManager2.h"

CardSelectCommand::CardSelectCommand(const std::vector<PlayerComponent*>& pPlayerComponents, const int& currentPlayerIndex) :
	m_pPlayerComponents{ pPlayerComponents },
	m_CurrentPlayerIndex{currentPlayerIndex}
{
	
}

void CardSelectCommand::Execute()
{

	m_pPlayerComponents[m_CurrentPlayerIndex]->SelectCardAtMousePosition(ody::InputManager::GetInstance().GetMousePosition());
}