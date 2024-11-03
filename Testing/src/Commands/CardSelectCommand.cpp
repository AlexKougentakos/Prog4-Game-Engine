#include "CardSelectCommand.h"
#include "InputManager2.h"
#include "Components/Players/HumanPlayer.h"

CardSelectCommand::CardSelectCommand(HumanPlayer* pHumanPlayer) :
	m_pHumanPlayer{ pHumanPlayer }
{
	
}

void CardSelectCommand::Execute()
{
	m_pHumanPlayer->SelectCardAtMousePosition(ody::InputManager::GetInstance().GetMousePosition());
}