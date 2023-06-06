#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <XInput.h>
#pragma comment(lib, "xinput.lib")

#include "XboxController.h"

#include <iostream>
#include <memory>
#include <glm/detail/func_geometric.inl>

using namespace ody;

class XBox360Controller::XBox360ControllerImpl final
{
	XINPUT_STATE previousState{};
	XINPUT_STATE currentState{};

	WORD buttonsPressedThisFrame;
	WORD buttonsReleasedThisFrame;

	int _controllerIndex;
	std::unique_ptr<glm::vec2> m_pLastThumbstickPosLeft{};
	std::unique_ptr<glm::vec2> m_pLastThumbstickPosRight{};

public:
	XBox360ControllerImpl(int controllerIndex)
		:_controllerIndex{ controllerIndex }
	{
		ZeroMemory(&previousState, sizeof(XINPUT_STATE));
		ZeroMemory(&currentState, sizeof(XINPUT_STATE));

		m_pLastThumbstickPosLeft = std::make_unique<glm::vec2>();
		m_pLastThumbstickPosRight = std::make_unique<glm::vec2>();
	}

	void Update()
	{
		CopyMemory(&previousState, &currentState, sizeof(XINPUT_STATE));
		ZeroMemory(&currentState, sizeof(XINPUT_STATE));
		XInputGetState(_controllerIndex, &currentState);

		const auto buttonChanges = currentState.Gamepad.wButtons ^ previousState.Gamepad.wButtons;
		buttonsPressedThisFrame = buttonChanges & currentState.Gamepad.wButtons;
		buttonsReleasedThisFrame = buttonChanges & (~currentState.Gamepad.wButtons);

		*m_pLastThumbstickPosLeft = GetLeftThumbStickPos();
		*m_pLastThumbstickPosRight = GetRightThumbStickPos();
	}

	bool IsDownThisFrame(unsigned int button) const { return buttonsPressedThisFrame & button; }
	bool IsUpThisFrame(unsigned int button) const { return buttonsReleasedThisFrame & button; }
	bool IsPressed(unsigned int button) const { return currentState.Gamepad.wButtons & button; }
	bool IsThumbMoved(unsigned int button) const
	{
		if (button & static_cast<int>(ControllerButton::LeftThumbStick))
			return glm::length(GetLeftThumbStickPos()) > 0.2f;

		if (button & static_cast<int>(ControllerButton::RightThumbStick))
			return glm::length(GetRightThumbStickPos()) > 0.2f;

		return false;
	}

	glm::vec2* const GetLeftThumbStickPosRef() const
	{
		return m_pLastThumbstickPosLeft.get();
	}

	glm::vec2* const GetRightThumbStickPosRef() const
	{
		return m_pLastThumbstickPosRight.get();
	}

	glm::vec2 GetLeftThumbStickPos() const
	{
		return glm::vec2{ currentState.Gamepad.sThumbLX / m_ThumbRange, -currentState.Gamepad.sThumbLY / m_ThumbRange };
	}

	glm::vec2 GetRightThumbStickPos() const
	{
		return glm::vec2{ currentState.Gamepad.sThumbRX / m_ThumbRange, -currentState.Gamepad.sThumbRY / m_ThumbRange };
	}

};

std::pair<glm::vec2*, glm::vec2*> XBox360Controller::GetThumbStickPositions() const
{
	return std::make_pair(pImpl->GetLeftThumbStickPosRef(), pImpl->GetRightThumbStickPosRef());
}

XBox360Controller::XBox360Controller(unsigned int controllerIndex)
	: m_ControllerIndex{ controllerIndex }
{
	pImpl = new XBox360ControllerImpl(controllerIndex);
}

XBox360Controller::~XBox360Controller()
{
	delete pImpl;
}

void XBox360Controller::Update()
{
	pImpl->Update();
}

bool XBox360Controller::IsDown(ControllerButton button) const
{
	return pImpl->IsDownThisFrame(static_cast<unsigned int>(button));
}

bool XBox360Controller::IsUp(ControllerButton button) const
{
	return pImpl->IsUpThisFrame(static_cast<unsigned int>(button));
}

bool XBox360Controller::IsPressed(ControllerButton button) const
{
	return pImpl->IsPressed(static_cast<unsigned int>(button));
}

bool XBox360Controller::IsThumbMoved(ControllerButton button) const
{
	return pImpl->IsThumbMoved(static_cast<unsigned int>(button));
}

glm::vec2 XBox360Controller::GetThumbStickPos(bool leftThumb) const
{
	if (leftThumb)
		return pImpl->GetLeftThumbStickPos();
	
	return pImpl->GetRightThumbStickPos();
}
