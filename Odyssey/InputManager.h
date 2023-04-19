#pragma once
#include "Singleton.h"
#include "Command.h"
#include "Controller.h"

#include <memory>
#include <vector>
#include <glm/glm.hpp>
#include <set>


namespace dae
{
	class GameObject;
}

namespace ody
{
	class InputManager final : public dae::Singleton<InputManager>
	{
	public:
		enum class InputType
		{
			ONBUTTONDOWN,
			ONBUTTONUP,
			ONBUTTON
		};
		enum class GamepadButton
		{
			//todo: dpad for controller input and try
			DPAD_UP = 0x0001,
			DPAD_DOWN = 0x0002,
			DPAD_LEFT = 0x0004,
			DPAD_RIGHT = 0x0008,
			START = 0x0010,
			BACK = 0x0020,
			LEFT_SHOULDER = 0x0100,
			RIGHT_SHOULDER = 0x0200,
			A = 0x1000,
			B = 0x2000,
			X = 0x4000,
			Y = 0x8000
		};
		enum class GamepadAxis
		{
			LEFT_THUMB = 0x0040,
			RIGHT_THUMB = 0x0080,
			LEFT_SHOULDER = 0x0100,
			RIGHT_SHOULDER = 0x0200
		};

		bool ProcessInput();

		void BindControllerButtonCommand(unsigned int controller, GamepadButton button, InputType inputType, std::unique_ptr<ody::Command> pCommand);
		
		void BindKeyboardCommand(unsigned int keyboardKey, InputType inputType, std::unique_ptr<ody::Command> pCommand);

		void BindControllerAxisCommand(unsigned int controller, GamepadAxis button, std::unique_ptr<ody::Command> pCommand);

		float GetAxis(ody::Command* pCommand) const;
		glm::vec2 GetTwoDirectionalAxis(ody::Command* pCommand) const;

		void Clear();
	private:
		struct InputDigital
		{
			bool keyboard{};
			unsigned int controllerIdx{};
			unsigned int button{};
			InputType inputType{};
		};
		struct InputAnalog
		{
			unsigned int controllerIdx{};
			unsigned int button{};
			bool x{};
		};

		bool ReadSDLInput();
		bool IsKeyBound(const ody::InputManager::InputDigital& inputKey);

		void AddControllersIfNeeded(unsigned int controller);

		glm::vec2 GetTwoDirectionalAxis(const std::vector<InputDigital>& inputVector) const;
		glm::vec2 GetTwoDirectionalAxis(const std::vector<InputAnalog>& inputVector) const;

		float GetAxis(const std::vector<InputDigital>& inputVector) const;
		float GetAxis(const InputAnalog& input) const;

		std::set<unsigned int> m_KeyboardDownInput{};
		std::set<unsigned int> m_KeyboardUpInput{};
		std::set<unsigned int> m_KeyboardInput{};

		std::vector<std::pair<std::unique_ptr<ody::Command>, std::vector<InputAnalog>>> m_pBoundAnalogCommands{};
		std::vector<std::pair<std::unique_ptr<ody::Command>, std::vector<InputDigital>>> m_pBoundDigitalCommands{};
		std::vector<std::unique_ptr<ody::Controller>> m_pControllers{};
	};
}