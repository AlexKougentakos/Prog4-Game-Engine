#include <SDL.h>
#include <backends/imgui_impl_sdl2.h>

#include "InputManager.h"
#include "Command.h"
#include "Controller.h"

bool ody::InputManager::ProcessInput()
{
	if (!ReadSDLInput()) return false;

	for (const auto& pController : m_pControllers)
	{
		pController->Update();
	}
	
	for (const auto& analogCommand : m_pBoundAnalogCommands)
	{
		for (const auto& inputAnalog : analogCommand.second)
		{
			const float axisValue{ GetAxis(inputAnalog) };
			if (abs(axisValue) > 0.0f)
			{
				analogCommand.first->Execute();
				break;
			}
		}
	}

	for (const auto& buttonCommand : m_pBoundDigitalCommands)
	{
		for (const auto& inputKey : buttonCommand.second)
		{
			if (IsKeyBound(inputKey))
			{
				buttonCommand.first->Execute();
				break;
			}
		}
	}

	return true;
}

bool ody::InputManager::ReadSDLInput()
{
	m_KeyboardDownInput.clear();
	m_KeyboardUpInput.clear();

	// Check keyboard input
	SDL_Event e;
	while (SDL_PollEvent(&e))
	{
		if (e.type == SDL_QUIT) return false;

		switch (e.key.type)
		{
		case SDL_KEYUP:
		{
			const auto it{ m_KeyboardInput.find(e.key.keysym.sym) };
			if (it != m_KeyboardInput.end())
			{
				m_KeyboardInput.erase(it);
				m_KeyboardUpInput.insert(e.key.keysym.sym);
			}
			break;
		}
		case SDL_KEYDOWN:
		{
			const auto it{ m_KeyboardInput.find(e.key.keysym.sym) };
			if (it == m_KeyboardInput.end())
			{
				m_KeyboardInput.insert(e.key.keysym.sym);
				m_KeyboardDownInput.insert(e.key.keysym.sym);
			}
			break;
		}
		}

		//process event for IMGUI
		ImGui_ImplSDL2_ProcessEvent(&e);
	}

	return true;
}

bool ody::InputManager::IsKeyBound(const ody::InputManager::InputDigital& inputKey)
{
	if (inputKey.keyboard)
	{
		switch (inputKey.inputType)
		{
		case InputType::ONBUTTONDOWN:
			return m_KeyboardDownInput.find(inputKey.button) != m_KeyboardDownInput.end();
		case InputType::ONBUTTONUP:
			return m_KeyboardUpInput.find(inputKey.button) != m_KeyboardUpInput.end();
		case InputType::ONBUTTON:
			return m_KeyboardInput.find(inputKey.button) != m_KeyboardInput.end();
		}
	}
	else
	{
		switch (inputKey.inputType)
		{
		case InputType::ONBUTTONDOWN:
			return m_pControllers[inputKey.controllerIdx]->OnButtonDown(inputKey.button);
		case InputType::ONBUTTONUP:
			return m_pControllers[inputKey.controllerIdx]->OnButtonUp(inputKey.button);
		case InputType::ONBUTTON:
			return m_pControllers[inputKey.controllerIdx]->OnButton(inputKey.button);
		}
	}

	return false;
}

//glm::vec2 ody::InputManager::GetThumbstickPosition(bool leftThumbstick, int playerIndex)
//{
//	glm::vec2 pos;
//	if (leftThumbstick)
//	{
//		pos = glm::vec2(m_CurrGamepadState[int(playerIndex)].Gamepad.sThumbLX, m_CurrGamepadState[int(playerIndex)].Gamepad.sThumbLY);
//
//		if (pos.x > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE && pos.x < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)pos.x = 0;
//		if (pos.y > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE && pos.y < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)pos.y = 0;
//	}
//	else
//	{
//		pos = XMFLOAT2(m_CurrGamepadState[int(playerIndex)].Gamepad.sThumbRX, m_CurrGamepadState[int(playerIndex)].Gamepad.sThumbRY);
//
//		if (pos.x > -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE && pos.x < XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)pos.x = 0;
//		if (pos.y > -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE && pos.y < XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)pos.y = 0;
//	}
//
//	if (pos.x < 0)pos.x /= 32768;
//	else pos.x /= 32767;
//
//	if (pos.y < 0)pos.y /= 32768;
//	else pos.y /= 32767;
//
//	return pos;
//}

//If there is more than one controller this function should be used to access a specific controller
void ody::InputManager::BindControllerButtonCommand(unsigned int controller, GamepadButton button, InputType inputType, std::unique_ptr<ody::Command> pCommand)
{
	// If no controller is found then we create a new one
	AddControllersIfNeeded(controller);

	m_pBoundDigitalCommands.emplace_back(std::make_pair(std::move(pCommand), std::vector<InputDigital>{ InputDigital{ false, controller, static_cast<unsigned int>(button), inputType } }));
}

void ody::InputManager::BindKeyboardCommand(unsigned int keyboardKey, InputType inputType, std::unique_ptr<ody::Command> pCommand)
{
	m_pBoundDigitalCommands.emplace_back(std::make_pair(std::move(pCommand), std::vector<InputDigital>{ InputDigital{ true, 0, keyboardKey, inputType } }));
}

void ody::InputManager::BindControllerAxisCommand(unsigned int controller, GamepadAxis button, std::unique_ptr<ody::Command> pCommand)
{
	// If no controller is found then we create a new one
	AddControllersIfNeeded(controller);

	m_pBoundAnalogCommands.emplace_back(std::make_pair(std::move(pCommand), std::vector<InputAnalog>{ InputAnalog{ controller, static_cast<unsigned int>(button) } }));
}

void ody::InputManager::Clear()
{
	m_pBoundAnalogCommands.clear();
	m_pBoundDigitalCommands.clear();
}

void ody::InputManager::AddControllersIfNeeded(unsigned int controller)
{
	// Add controllers until there is a controller with the id that is needed
	if (controller >= m_pControllers.size())
	{
		const unsigned int nrControllersNeeded{ (controller + 1) - static_cast<unsigned int>(m_pControllers.size()) };
		for (unsigned int i{}; i < nrControllersNeeded; ++i)
		{
			m_pControllers.emplace_back(std::make_unique<ody::Controller>(static_cast<unsigned int>(m_pControllers.size())));
		}
	}
}

float ody::InputManager::GetAxis(ody::Command* pCommand) const
{
	// Get the command/digital input pair with the given command ptr
	const auto& pBindedCommand{ std::find_if(begin(m_pBoundDigitalCommands), end(m_pBoundDigitalCommands), [pCommand](const auto& command)
		{
			return command.first.get() == pCommand;
		}) };

	// If a digital command is found, return the axis of the digital input
	if (pBindedCommand != m_pBoundDigitalCommands.end())
	{
		return GetAxis(pBindedCommand->second);
	}

	// Get the command/digital input pair with the given command ptr
	const auto& pBindedAnalogCommand{ std::find_if(begin(m_pBoundAnalogCommands), end(m_pBoundAnalogCommands), [pCommand](const auto& command)
		{
			return command.first.get() == pCommand;
		}) };


	// If an analog command is found, return the biggest axis binded to this command
	if (pBindedAnalogCommand != m_pBoundAnalogCommands.end())
	{
		float maxInput{};
		for (const InputAnalog& input : pBindedAnalogCommand->second)
		{
			const float curInput{ GetAxis(input) };
			maxInput = abs(curInput) > abs(maxInput) ? curInput : maxInput;
		}
		return maxInput;
	}

	return 0.0f;
}

glm::vec2 ody::InputManager::GetTwoDirectionalAxis(ody::Command* pCommand) const
{
	// Get the command/digital input pair with the given command ptr
	const auto& pBindedCommand{ std::find_if(begin(m_pBoundDigitalCommands), end(m_pBoundDigitalCommands), [pCommand](const auto& command)
		{
			return command.first.get() == pCommand;
		}) };

	// If a digital command is found, return the 2D axis of the digital input
	if (pBindedCommand != m_pBoundDigitalCommands.end())
	{
		return GetTwoDirectionalAxis(pBindedCommand->second);
	}

	// Get the command/digital input pair with the given command ptr
	const auto& pBindedAnalogCommand{ std::find_if(begin(m_pBoundAnalogCommands), end(m_pBoundAnalogCommands), [pCommand](const auto& command)
		{
			return command.first.get() == pCommand;
		}) };


	// If an analog command is found, return the 2D axis of the analog input
	if (pBindedAnalogCommand != m_pBoundAnalogCommands.end())
	{
		return GetTwoDirectionalAxis(pBindedAnalogCommand->second);
	}

	return glm::vec2{};
}

glm::vec2 ody::InputManager::GetTwoDirectionalAxis(const std::vector<InputDigital>& inputVector) const
{
	// If there are not exact 4 inputs binded to this command, log a warning and return nothing
	if (inputVector.size() != 4)
	{
		return glm::vec2{ 0.0f, 0.0f };
	}

	glm::vec2 input{};

	// For each button
	for (int i{}; i < static_cast<int>(inputVector.size()); ++i)
	{
		const auto& inputKey{ inputVector[i] };

		// Check if input requirement is met
		bool hasInput{};
		if (inputKey.keyboard)
		{
			if (m_KeyboardInput.find(inputKey.button) != m_KeyboardInput.end()) hasInput = true;
		}
		else
		{
			switch (inputKey.inputType)
			{
			case InputType::ONBUTTONDOWN:
				hasInput = m_pControllers[inputKey.controllerIdx]->OnButtonDown(inputKey.button);
				break;
			case InputType::ONBUTTONUP:
				hasInput = m_pControllers[inputKey.controllerIdx]->OnButtonUp(inputKey.button);
				break;
			case InputType::ONBUTTON:
				hasInput = m_pControllers[inputKey.controllerIdx]->OnButton(inputKey.button);
				break;
			}
		}

		// If this button's input is not triggered, continue to the next button
		if (!hasInput) continue;

		// Calculate the input vector for this button
		switch (i)
		{
		case 0:
			input.x += 1.0f;
			break;
		case 1:
			input.x -= 1.0f;
			break;
		case 2:
			input.y += 1.0f;
			break;
		case 3:
			input.y -= 1.0f;
			break;
		}
	}

	// Return the normalized 2D axis
	return glm::dot(input, input) > 0.0f ? glm::normalize(input) : input;
}

glm::vec2 ody::InputManager::GetTwoDirectionalAxis(const std::vector<InputAnalog>& inputVector) const
{
	// If there are not exact 4 inputs binded to this command, log a warning and return nothing
	if (inputVector.size() != 2)
	{
		return glm::vec2{ 0.0f, 0.0f };
	}

	// Retrieve the two input data
	const auto& horizontalInput{ inputVector[0] };
	const auto& verticalInput{ inputVector[1] };

	// Calculate the input vector for this button
	const glm::vec2 input
	{
		m_pControllers[horizontalInput.controllerIdx]->GetAxis(horizontalInput.button, horizontalInput.x),
		m_pControllers[verticalInput.controllerIdx]->GetAxis(verticalInput.button, verticalInput.x)
	};

	return input;
}

float ody::InputManager::GetAxis(const std::vector<InputDigital>& inputVector) const
{
	float input{};

	// For each button
	for (int i{}; i < static_cast<int>(inputVector.size()); ++i)
	{
		const auto& inputKey{ inputVector[i] };

		// Check if input requirement is met
		bool hasInput{};
		if (inputKey.keyboard)
		{
			if (m_KeyboardInput.find(inputKey.button) != m_KeyboardInput.end()) hasInput = true;
		}
		else
		{
			switch (inputKey.inputType)
			{
			case InputType::ONBUTTONDOWN:
				hasInput = m_pControllers[inputKey.controllerIdx]->OnButtonDown(inputKey.button);
				break;
			case InputType::ONBUTTONUP:
				hasInput = m_pControllers[inputKey.controllerIdx]->OnButtonUp(inputKey.button);
				break;
			case InputType::ONBUTTON:
				hasInput = m_pControllers[inputKey.controllerIdx]->OnButton(inputKey.button);
				break;
			}
		}

		// If this button's input is not triggered, continue to the next button
		if (!hasInput) continue;

		// Calculate the input value for this button
		switch (i)
		{
		case 0:
			input += 1.0f;
			break;
		case 1:
			input -= 1.0f;
			break;
		}
	}

	// Return the axis
	return input;
}

float ody::InputManager::GetAxis(const InputAnalog& input) const
{
	switch (static_cast<GamepadAxis>(input.button))
	{
	case GamepadAxis::LEFT_THUMB:
	case GamepadAxis::RIGHT_THUMB:
		return m_pControllers[input.controllerIdx]->GetAxis(input.button, input.x);
	case GamepadAxis::RIGHT_SHOULDER:
	case GamepadAxis::LEFT_SHOULDER:
		return m_pControllers[input.controllerIdx]->GetAxis(input.button);
	}

	return 0.0f;
}