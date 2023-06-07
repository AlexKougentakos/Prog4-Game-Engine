#include <SDL.h>
#include "InputManager2.h"

#include "IAudio.h"

#ifdef _DEBUG
#include "SceneManager.h"
#include "ServiceLocator.h"
#endif

using namespace ody;

bool InputManager::ProcessInput()
{
	//Keyboard part
	SDL_Event e;
	while (SDL_PollEvent(&e)) 
	{
		if (e.type == SDL_QUIT) {
			return false;
		}
		//Keyboard OnDown and OnRelease
		for (auto& mapPair : m_KeyboardActionMap)
		{
			if (unsigned int(e.key.keysym.sym) == mapPair.first.key)
			{
				if (mapPair.first.type == InputType::OnDown && e.type == SDL_KEYDOWN)
					mapPair.second->Execute();

				else if (mapPair.first.type == InputType::OnRelease && e.type == SDL_KEYUP)
					mapPair.second->Execute();
			}
		}

#ifdef _DEBUG //These are only used for debbuging
	switch (e.key.type)
	{
		case SDL_KEYDOWN:
		{
			if (e.key.keysym.sym == SDLK_j)
				SceneManager::GetInstance().PreviousScene();
			else if (e.key.keysym.sym == SDLK_k)
				SceneManager::GetInstance().NextScene();
			else if (e.key.keysym.sym == SDLK_1)
			{
				ody::ServiceLocator::GetSoundSystem().PlaySound(1);
			}
			else if (e.key.keysym.sym == SDLK_2)
			{
				ody::ServiceLocator::GetSoundSystem().PlaySound(2);
			}
			else if (e.key.keysym.sym == SDLK_3)
			{
				ody::ServiceLocator::GetSoundSystem().PlaySound(3);
			}
			else if (e.key.keysym.sym == SDLK_4)
			{
				ody::ServiceLocator::GetSoundSystem().PlaySound(4);
			}
			else if (e.key.keysym.sym == SDLK_5)
			{
				ody::ServiceLocator::GetSoundSystem().PlaySound(5);
			}
			else if (e.key.keysym.sym == SDLK_6)
			{
				ody::ServiceLocator::GetSoundSystem().PlaySound(6);
			}
			else if (e.key.keysym.sym == SDLK_7)
			{
				ody::ServiceLocator::GetSoundSystem().PlaySound(7);
			}
			else if (e.key.keysym.sym == SDLK_8)
			{
				ody::ServiceLocator::GetSoundSystem().PlaySound(8);
			}

			break;
		}
		case SDL_KEYUP:
		{
			break;
		}
	}


#endif
	}//While loop end

	//Keyboard Pressed continuously
	const Uint8* state = SDL_GetKeyboardState(nullptr);
	for (auto& mapPair : m_KeyboardActionMap)
	{
		if (mapPair.first.type == InputType::Pressed)
		{
			if (state[SDL_GetScancodeFromKey(mapPair.first.key)])
				mapPair.second->Execute();
		}
	}

	//Controller part
	for (auto& controller : m_ControllerPtrs)
	{
		controller->Update();

		for (auto& mapPair : m_ControllerActionMap)
		{
			if (mapPair.first.controllerID == controller->GetIdx())
			{
				
				if (mapPair.first.type == InputType::OnDown && controller->IsDown(mapPair.first.button))
					mapPair.second->Execute();

				else if (mapPair.first.type == InputType::Pressed && controller->IsPressed(mapPair.first.button))
					mapPair.second->Execute();

				else if (mapPair.first.type == InputType::OnRelease && controller->IsUp(mapPair.first.button))
					mapPair.second->Execute();

				if (mapPair.first.type == InputType::OnThumbMove && controller->IsThumbMoved(mapPair.first.button))
					mapPair.second->Execute();
				if (mapPair.first.type == InputType::OnThumbMove && controller->IsThumbMoved(mapPair.first.button))
					mapPair.second->Execute();
				
			}

		}
	}

	return true;
}

XBox360Controller* InputManager::GetController(unsigned int controllerIdx)
{
	for (auto& controller : m_ControllerPtrs)
	{
		if (controller->GetIdx() == controllerIdx)
		{
			return controller.get();
		}
	}
	return nullptr;
}


void InputManager::AddControllerIfNeeded(unsigned int controllerID)
{
	bool doesControllerExist{ false };
	for (const auto& controller : m_ControllerPtrs)
	{
		if (controller->GetIdx() == controllerID)
		{
			doesControllerExist = true;
			break;
		}
	}

	if (doesControllerExist == false)
		m_ControllerPtrs.emplace_back(std::make_unique<XBox360Controller>(controllerID));
}


void InputManager::AddControllerCommand(XBox360Controller::ControllerButton button, unsigned int controllerID, InputType type, std::unique_ptr<Command> pCommand)
{
	AddControllerIfNeeded(controllerID);

	InputDataController inputData{ controllerID, button, type };
	m_ControllerActionMap[inputData] = std::move(pCommand);
}

void InputManager::AddKeyboardCommand(unsigned int keyboardKey, InputType type, std::unique_ptr<Command> pCommand)
{
	InputDataKeyboard inputData{ keyboardKey, type };
	m_KeyboardActionMap[inputData] = std::move(pCommand);
}

glm::vec2 InputManager::GetThumbstickDirection(unsigned controllerIdx, bool leftThumb) const
{
	return m_ControllerPtrs[controllerIdx]->GetThumbStickPos(leftThumb);
}