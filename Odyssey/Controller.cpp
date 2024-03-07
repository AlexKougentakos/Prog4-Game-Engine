#ifndef __EMSCRIPTEN__
#include "Controller.h"
#include <cstdlib>
#include <algorithm>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <XInput.h>
#pragma comment(lib, "xinput.lib")
namespace ody
{
    class Controller::ControllerImplementation
    {
    public:
        ControllerImplementation(unsigned int controllerIdx) : m_ControllerIdx{ controllerIdx } {}

        void Update()
        {
            m_PreviousState = m_CurrentState;
            XInputGetState(m_ControllerIdx, &m_CurrentState);

            const auto buttonChanges{ m_CurrentState.Gamepad.wButtons ^ m_PreviousState.Gamepad.wButtons };
            m_ButtonsDown = buttonChanges & m_CurrentState.Gamepad.wButtons;
            m_ButtonsUp = buttonChanges & (~m_CurrentState.Gamepad.wButtons);
        }
        void SetDeadZone(float percentage) { m_DeadZone = percentage; }

        bool OnButtonDown(unsigned int button) const
        {
            return m_ButtonsDown & button;
        }
        bool OnButtonUp(unsigned int button) const
        {
            return m_ButtonsUp & button;
        }
        bool OnButton(unsigned int button) const
        {
            return m_CurrentState.Gamepad.wButtons & button;
        }
        float GetAxis(bool left, bool x) const
        {
            constexpr float ceilPercentage{ 0.9f };
            constexpr SHORT ceilValue{ static_cast<SHORT>(ceilPercentage * MAXSHORT) };

            const auto axisValue
            {
                x
                ? (left ? m_CurrentState.Gamepad.sThumbLX : m_CurrentState.Gamepad.sThumbRX)
                : (left ? m_CurrentState.Gamepad.sThumbLY : m_CurrentState.Gamepad.sThumbRY)
            };

            const SHORT sign{ axisValue == 0 ? 0 : static_cast<SHORT>(abs(axisValue) / axisValue) };
            const SHORT deadZoneValue{ static_cast<SHORT>(m_DeadZone * MAXSHORT) };

            const SHORT clamped{ std::clamp(static_cast<SHORT>(abs(axisValue - sign)), deadZoneValue, ceilValue) };
            return static_cast<float>(clamped - deadZoneValue) / (ceilValue - deadZoneValue) * sign;
        }
        float GetAxis(unsigned int button)
        {
            switch (button)
            {
            case XINPUT_GAMEPAD_LEFT_SHOULDER:
                return static_cast<float>(m_CurrentState.Gamepad.bLeftTrigger) / MAXBYTE;
                break;
            case XINPUT_GAMEPAD_RIGHT_SHOULDER:
                return static_cast<float>(m_CurrentState.Gamepad.bRightTrigger) / MAXBYTE;
                break;
            }
            return 0.0f;
        }
    private:
        unsigned int m_ControllerIdx{};

        XINPUT_STATE m_PreviousState{};
        XINPUT_STATE m_CurrentState{};

        unsigned int m_ButtonsDown{};
        unsigned int m_ButtonsUp{};

        float m_DeadZone{ 0.25f };
    };
}

ody::Controller::Controller(unsigned int controllerIdx)
    : m_pImpl{ new ControllerImplementation{ controllerIdx } }
{
}

ody::Controller::~Controller()
{
}

void ody::Controller::Update()
{
    m_pImpl->Update();
}

void ody::Controller::SetDeadZone(float percentage)
{
    m_pImpl->SetDeadZone(percentage);
}

bool ody::Controller::OnButtonDown(unsigned int button) const
{
    return m_pImpl->OnButtonDown(button);
}

bool ody::Controller::OnButtonUp(unsigned int button) const
{
    return m_pImpl->OnButtonUp(button);
}

bool ody::Controller::OnButton(unsigned int button) const
{
    return m_pImpl->OnButton(button);
}

float ody::Controller::GetAxis(unsigned int thumbStick, bool x) const
{
    return m_pImpl->GetAxis(thumbStick == XINPUT_GAMEPAD_LEFT_THUMB, x);
}

float ody::Controller::GetAxis(unsigned int button) const
{
    return m_pImpl->GetAxis(button);
}
#endif