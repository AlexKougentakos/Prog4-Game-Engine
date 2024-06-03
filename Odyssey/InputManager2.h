#pragma once
#include <iostream>
#include <map>
#include <memory>
#include <vector>

#include "Singleton.h"
#include "Command.h"

#ifndef __EMSCRIPTEN__
#include "XboxController.h"
#endif

namespace ody
{

	class InputManager final : public Singleton<InputManager>
	{
	public:
		bool ProcessInput();

#ifndef __EMSCRIPTEN__
		XBox360Controller* GetController(unsigned int controllerIdx);
#endif
		enum class InputType
		{
			OnDown,
			Pressed,
			OnRelease,

			OnThumbMove,
			OnThumbIdleOnce, //will only execute once when thumbstick is idle
			OnThumbIdleContinuous //will execute every frame when thumbstick is idle
		};

		void AddKeyboardCommand(unsigned int keyboardKey, InputType type, std::unique_ptr<Command> pCommand);

#ifndef __EMSCRIPTEN__
		void AddControllerCommand(XBox360Controller::ControllerButton button, unsigned int controllerID, InputType type, std::unique_ptr<Command> pCommand);

		glm::vec2 GetThumbstickDirection(unsigned int controllerIdx, bool leftThumb = true) const;

		std::pair<glm::vec2*, glm::vec2*> GetThumbstickPositionsRef(unsigned int controllerIdx)
		{
			AddControllerIfNeeded(controllerIdx);
			return m_ControllerPtrs[controllerIdx]->GetThumbStickPositions();
		}
#endif

	private:

#ifndef __EMSCRIPTEN__
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
#endif

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

#ifndef __EMSCRIPTEN__
		//Controller
		std::map<InputDataController, std::unique_ptr<Command>> m_ControllerActionMap{};
		std::vector<std::unique_ptr<XBox360Controller>> m_ControllerPtrs{};
		bool m_ExecutedIdleThumbstick{ false };
		void AddControllerIfNeeded(unsigned int controllerID);

#endif
		std::map<InputDataKeyboard, std::unique_ptr<Command>> m_KeyboardActionMap{};
		bool m_ExecutedOnDown{ false };

	};
}