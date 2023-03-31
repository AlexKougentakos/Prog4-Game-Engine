#pragma once
#include "Singleton.h"
#include "Command.h"
#include "Controller.h"

#include <memory>
#include <vector>
#include <glm/glm.hpp>
#include <set>

namespace ody
{
	class GameObject;

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

		template <class T>
		void BindDigitalCommand(unsigned int controller, GamepadButton button, InputType inputType, GameObject* pGameObject);
		void BindDigitalCommand(unsigned int controller, GamepadButton button, InputType inputType, std::unique_ptr<ody::Command> pCommand);
		template <class T>
		void BindDigitalCommand(unsigned int keyboardKey, InputType inputType, GameObject* pGameObject);
		void BindDigitalCommand(unsigned int keyboardKey, InputType inputType, std::unique_ptr<ody::Command> pCommand);

		template <class T>
		void BindAxisCommand(unsigned int controller, GamepadAxis button, bool isHorizontalAxis, GameObject* pGameObject);
		void BindAxisCommand(unsigned int controller, GamepadAxis button, bool isHorizontalAxis, std::unique_ptr<ody::Command> pCommand);
		template <class T>
		void BindAxisCommand(unsigned int controller, GamepadAxis button, GameObject* pGameObject);
		void BindAxisCommand(unsigned int controller, GamepadAxis button, std::unique_ptr<ody::Command> pCommand);

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

	template<class T>
	inline void InputManager::BindDigitalCommand(unsigned int controller, GamepadButton button, InputType inputType, GameObject* pGameObject)
	{
		static_assert(std::is_base_of<ody::Command, T>(), "T needs to be derived from Command");

		// Add controllers if the controllerIdx is higher then the amount of controllers available
		AddControllersIfNeeded(controller);

		// Try finding a command/input pair of type T
		const auto it{ std::find_if(begin(m_pBoundDigitalCommands), end(m_pBoundDigitalCommands), [](const auto& pBindedCommand)
			{
				return typeid(*pBindedCommand.first.get()) == typeid(T);
			}) };


		// If a command/input pair has been found, bind the new input to this command
		if (it != m_pBoundDigitalCommands.end())
		{
			it->second.push_back(InputDigital{ false, controller, static_cast<unsigned int>(button), inputType });
			return;
		}

		// Create a new command
		m_pBoundDigitalCommands.push_back(std::make_pair(std::make_unique<T>(pGameObject), std::vector<InputDigital>{ InputDigital{ false, controller, static_cast<unsigned int>(button), inputType } }));
	}

	template<class T>
	inline void InputManager::BindDigitalCommand(unsigned int keyboardKey, InputType inputType, GameObject* pGameObject)
	{
		static_assert(std::is_base_of<ody::Command, T>(), "T needs to be derived from Command");

		// Try finding a command/input pair of type T
		const auto it{ std::find_if(begin(m_pBoundDigitalCommands), end(m_pBoundDigitalCommands), [](const auto& pBindedCommand)
			{
				return typeid(*pBindedCommand.first.get()) == typeid(T);
			}) };


		// If a command/input pair has been found, bind the new input to this command
		if (it != m_pBoundDigitalCommands.end())
		{
			it->second.push_back(InputDigital{ true, 0, keyboardKey, inputType });
			return;
		}

		// Create a new command
		m_pBoundDigitalCommands.push_back(std::make_pair(std::make_unique<T>(pGameObject), std::vector<InputDigital>{ InputDigital{ true, 0, keyboardKey, inputType } }));
	}

	template<class T>
	inline void InputManager::BindAxisCommand(unsigned int controller, GamepadAxis button, bool isHorizontalAxis, GameObject* pGameObject)
	{
		static_assert(std::is_base_of<ody::Command, T>(), "T needs to be derived from Command");

		// Add controllers if the controllerIdx is higher then the amount of controllers available
		AddControllersIfNeeded(controller);

		// Try finding a command/input pair of type T
		const auto it{ std::find_if(begin(m_pBoundAnalogCommands), end(m_pBoundAnalogCommands), [](const auto& pBindedCommand)
			{
				return std::is_same<T*, decltype(pBindedCommand.first.get())>();
			}) };

		// If a command/input pair has been found, bind the new input to this command
		if (it != m_pBoundAnalogCommands.end())
		{
			it->second.push_back(InputAnalog{ controller, static_cast<unsigned int>(button), isHorizontalAxis });
			return;
		}

		// Create a new command
		m_pBoundAnalogCommands.push_back(std::make_pair(std::make_unique<T>(pGameObject), std::vector<InputAnalog>{ InputAnalog{ controller, static_cast<unsigned int>(button), isHorizontalAxis } }));
	}
	template<class T>
	inline void InputManager::BindAxisCommand(unsigned int controller, GamepadAxis button, GameObject* pGameObject)
	{
		static_assert(std::is_base_of<ody::Command, T>(), "T needs to be derived from Command");

		// Add controllers if the controllerIdx is higher then the amount of controllers available
		AddControllersIfNeeded(controller);

		// Try finding a command/input pair of type T
		const auto it{ std::find_if(begin(m_pBoundAnalogCommands), end(m_pBoundAnalogCommands), [](const auto& pBindedCommand)
			{
				return std::is_same<T*, decltype(pBindedCommand.first.get())>();
			}) };

		// If a command/input pair has been found, bind the new input to this command
		if (it != m_pBoundAnalogCommands.end())
		{
			it->second.push_back(InputAnalog{ controller, static_cast<unsigned int>(button) });
			return;
		}

		// Create a new command
		m_pBoundAnalogCommands.push_back(std::make_pair(std::make_unique<T>(pGameObject), std::vector<InputAnalog>{ InputAnalog{ controller, static_cast<unsigned int>(button) } }));
	}
}