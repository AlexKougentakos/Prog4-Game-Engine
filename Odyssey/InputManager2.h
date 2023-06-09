#pragma once
#include <iostream>
#include <map>
#include <memory>
#include <vector>

#include "Singleton.h"
#include "XboxController.h"
#include "Command.h"

namespace ody
{

	class InputManager final : public Singleton<InputManager>
	{
	public:
		bool ProcessInput();

		XBox360Controller* GetController(unsigned int controllerIdx);

		enum class InputType
		{
			OnDown,
			Pressed,
			OnRelease,

			OnThumbMove,
			OnThumbIdleOnce, //will only execute once when thumbstick is idle
			OnThumbIdleContinuous //will execute every frame when thumbstick is idle
		};

		void AddControllerCommand(XBox360Controller::ControllerButton button, unsigned int controllerID, InputType type, std::unique_ptr<Command> pCommand);
		void AddKeyboardCommand(unsigned int keyboardKey, InputType type, std::unique_ptr<Command> pCommand);

		glm::vec2 GetThumbstickDirection(unsigned int controllerIdx, bool leftThumb = true) const;

		std::pair<glm::vec2*, glm::vec2*> GetThumbstickPositionsRef(unsigned int controllerIdx)
		{
			AddControllerIfNeeded(controllerIdx);
			return m_ControllerPtrs[controllerIdx]->GetThumbStickPositions();
		}

	private:
		struct InputDataController
		{
			unsigned int controllerID{};
			XBox360Controller::ControllerButton button{};
			InputType type{};

			// Custom less-than operator for std::map
			bool operator<(const InputDataController& other) const
			{
				if (controllerID < other.controllerID) return true;
				if (controllerID > other.controllerID) return false;

				if (button < other.button) return true;
				if (button > other.button) return false;

				return type < other.type;
			}
		};

		struct InputDataKeyboard
		{
			unsigned int key{};
			InputType type{};

			// Custom less-than operator for std::map
			bool operator<(const InputDataKeyboard& other) const
			{
				if (key < other.key) return true;
				if (key > other.key) return false;

				return type < other.type;
			}
		};

		friend class Singleton<InputManager>;
		InputManager() = default;

		std::map<InputDataController, std::unique_ptr<Command>> m_ControllerActionMap{};
		std::vector<std::unique_ptr<XBox360Controller>> m_ControllerPtrs{};

		std::map<InputDataKeyboard, std::unique_ptr<Command>> m_KeyboardActionMap{};

		bool m_ExecutedIdleThumbstick{ false };
		bool m_ExecutedOnDown{ false };

		void AddControllerIfNeeded(unsigned int controllerID);
	};
}