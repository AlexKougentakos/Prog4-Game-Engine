#include <SDL.h>
#include "InputManager2.h"

#include <imgui_impl_sdl2.h>

#include "GameScene.h"
#include "IAudio.h"
#include "TextureComponent.h"

#include "ServiceLocator.h"
#ifdef _DEBUG
#include "SceneManager.h"
#endif

using namespace ody;

bool InputManager::ProcessInput()
{
	//Keyboard part
	SDL_Event e;
	while (SDL_PollEvent(&e)) 
	{
		ImGui_ImplSDL2_ProcessEvent(&e);

		if (e.type == SDL_QUIT) 
		{
			return false;
		}

		//Keyboard OnDown and OnRelease
		for (const auto& mapPair : m_KeyboardActionMap)
		{
			if (static_cast<unsigned int>(e.key.keysym.sym) == mapPair.first.key)
			{
				if (mapPair.first.type == InputType::OnDown && e.type == SDL_KEYDOWN && e.key.repeat == 0)
				{
					mapPair.second->Execute();
				}

				else if (mapPair.first.type == InputType::OnRelease && e.type == SDL_KEYUP)
				{
					mapPair.second->Execute();
				}
			}
		}
#ifdef __EMSCRIPTEN__
		switch (e.key.type)
		{
			case SDL_KEYDOWN:
			{
				if (e.key.keysym.scancode == SDL_SCANCODE_D)
				{
					printf("D is pressed\n");
					ody::ServiceLocator::GetSoundSystem().PlaySound(1);
				}
			}
		}
#endif


#ifdef _DEBUG //These are only used for debugging
	switch (e.key.type)
	{
		case SDL_KEYDOWN:
		{
			if (e.key.keysym.scancode == SDL_SCANCODE_D)
			{
				auto pos = SceneManager::GetInstance().GetActiveScene()->GetCamera()->GetPosition();

				SceneManager::GetInstance().GetActiveScene()->GetCamera()->SetPosition({ pos.x + 10, pos.y + 10} );
			}

			if (e.key.keysym.scancode == SDL_SCANCODE_H)
			{
				SceneManager::GetInstance().GetActiveScene()->GetCamera()->SetZoom(0.5f);
			}

			break;
		}
		case SDL_KEYUP:
		{
			break;
		}
	}
#endif
	}

	//Keyboard Pressed continuously
	const Uint8* state = SDL_GetKeyboardState(nullptr);
	for (const auto& mapPair : m_KeyboardActionMap)
	{
		if (mapPair.first.type == InputType::Pressed)
		{
			if (state[SDL_GetScancodeFromKey(mapPair.first.key)])
				mapPair.second->Execute();
		}
	}

#ifndef __EMSCRIPTEN__
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

				//Thumbstick Part
				if (mapPair.first.type == InputType::OnThumbMove && controller->IsThumbMoved(mapPair.first.button))
				{
					m_ExecutedIdleThumbstick = false;
					mapPair.second->Execute();
				}
				if (mapPair.first.type == InputType::OnThumbIdleContinuous && controller->IsThumbIdle(mapPair.first.button))
					mapPair.second->Execute();
				if (mapPair.first.type == InputType::OnThumbIdleOnce && controller->IsThumbIdle(mapPair.first.button)
					&& !m_ExecutedIdleThumbstick)
				{
					m_ExecutedIdleThumbstick = true;
					mapPair.second->Execute();
				}
			}
		}
	}
#endif

	return true;
}

#ifndef __EMSCRIPTEN__

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

	const InputDataController inputData{ controllerID, button, type };
	m_ControllerActionMap[inputData] = std::move(pCommand);
}

glm::vec2 InputManager::GetThumbstickDirection(unsigned int controllerIdx, bool leftThumb) const
{
	return m_ControllerPtrs[controllerIdx]->GetThumbStickPos(leftThumb);
}
#endif

void InputManager::AddKeyboardCommand(unsigned int keyboardKey, InputType type, std::unique_ptr<Command> pCommand)
{
	const InputDataKeyboard inputData{ keyboardKey, type };
	m_KeyboardActionMap[inputData] = std::move(pCommand);
}