#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <Xinput.h>

#include "Keyboard.h"

class ody::Keyboard::KeyboardImpl
{
private:
	XINPUT_STATE previousState{};
	XINPUT_STATE currentState{};

	WORD buttonsPressedThisFrame{};
	WORD buttonsReleasedThisFrame{};

	int m_KeyboardIndex{};

public:
	KeyboardImpl(int keyboardIndex)
	{
		ZeroMemory(&previousState, sizeof(XINPUT_STATE));
		ZeroMemory(&currentState, sizeof(XINPUT_STATE));

		m_KeyboardIndex = keyboardIndex;
	}

	void Update()
	{
		CopyMemory(&previousState, &currentState, sizeof(XINPUT_STATE));
		ZeroMemory(&currentState, sizeof(XINPUT_STATE));
		XInputGetState(m_KeyboardIndex, &currentState);

		const auto buttonChanges = currentState.Gamepad.wButtons ^ previousState.Gamepad.wButtons;
		buttonsPressedThisFrame = buttonChanges & currentState.Gamepad.wButtons;
		buttonsReleasedThisFrame = buttonChanges & (~currentState.Gamepad.wButtons);
	}

	bool IsDownThisFrame(unsigned int key) const { return buttonsPressedThisFrame & key; }
	bool IsUpThisFrame(unsigned int key) const { return buttonsReleasedThisFrame & key; }
	bool IsPressed(unsigned int key) const { return currentState.Gamepad.wButtons & key; }
};

ody::Keyboard::Keyboard(int keyboardIndex)
{
	m_pImpl = std::make_unique<KeyboardImpl>(keyboardIndex);
}




void ody::Keyboard::Update()
{
	m_pImpl->Update();
}

bool ody::Keyboard::IsDown(KeyboardKey key) const
{
	return m_pImpl->IsDownThisFrame(static_cast<unsigned int>(key));
}

bool ody::Keyboard::IsUp(KeyboardKey key) const
{
	return m_pImpl->IsUpThisFrame(static_cast<unsigned int>(key));
}


bool ody::Keyboard::IsPressed(KeyboardKey key) const
{
	return m_pImpl->IsPressed(static_cast<unsigned int>(key));
}
