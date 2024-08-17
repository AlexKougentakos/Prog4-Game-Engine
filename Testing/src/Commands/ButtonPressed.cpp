#include "ButtonPressed.h"

#include "ButtonManagerComponent.h"
#include "InputManager2.h"

ButtonPressed::ButtonPressed(ButtonManagerComponent* pButtonManagerComponent) :
	m_pButtonManagerComponent(pButtonManagerComponent)
{
}

void ButtonPressed::Execute()
{
	m_pButtonManagerComponent->OnMouseClick(ody::InputManager::GetInstance().GetMousePosition());
}
