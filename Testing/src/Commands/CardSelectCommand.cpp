#include "CardSelectCommand.h"
#include "InputManager2.h"

CardSelectCommand::CardSelectCommand(PlayerComponent* pPlayerComponent) : 
	m_pPlayerComponent{ pPlayerComponent }
{
	
}

void CardSelectCommand::Execute()
{
	m_pPlayerComponent->SelectCardAtMousePosition(ody::InputManager::GetInstance().GetMousePosition());
}
