#include "InputManager2.h"
#include <SDL.h>
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
    SDL_Event e;
    while (SDL_PollEvent(&e))
    {
        ImGui_ImplSDL2_ProcessEvent(&e);

        if (e.type == SDL_QUIT)
        {
            return false;
        }

        ProcessMouseEvents(e);

        // Keyboard OnDown and OnRelease
        for (const auto& [inputData, commands] : m_KeyboardActionMap)
        {
            if (static_cast<unsigned int>(e.key.keysym.sym) == inputData.key)
            {
                if (inputData.type == InputType::OnDown && e.type == SDL_KEYDOWN && e.key.repeat == 0)
                {
                    for (const auto& command : commands)
                    {
                        command->Execute();
                    }
                }
                else if (inputData.type == InputType::OnRelease && e.type == SDL_KEYUP)
                {
                    for (const auto& command : commands)
                    {
                        command->Execute();
                    }
                }
            }
        }

        // ... (rest of the event handling code)
    }

    // Keyboard Pressed continuously
    const Uint8* state = SDL_GetKeyboardState(nullptr);
    for (const auto& [inputData, commands] : m_KeyboardActionMap)
    {
        if (inputData.type == InputType::Pressed)
        {
            if (state[SDL_GetScancodeFromKey(inputData.key)])
            {
                for (const auto& command : commands)
                {
                    command->Execute();
                }
            }
        }
    }

#ifndef __EMSCRIPTEN__
    // Controller part
    for (auto& controller : m_ControllerPtrs)
    {
        controller->Update();

        for (const auto& [inputData, commands] : m_ControllerActionMap)
        {
            if (inputData.controllerID == controller->GetIdx())
            {
                if (inputData.type == InputType::OnDown && controller->IsDown(inputData.button))
                {
                    for (const auto& command : commands)
                    {
                        command->Execute();
                    }
                }
                else if (inputData.type == InputType::Pressed && controller->IsPressed(inputData.button))
                {
                    for (const auto& command : commands)
                    {
                        command->Execute();
                    }
                }
                else if (inputData.type == InputType::OnRelease && controller->IsUp(inputData.button))
                {
                    for (const auto& command : commands)
                    {
                        command->Execute();
                    }
                }

                // Thumbstick Part
                if (inputData.type == InputType::OnThumbMove && controller->IsThumbMoved(inputData.button))
                {
                    m_ExecutedIdleThumbstick = false;
                    for (const auto& command : commands)
                    {
                        command->Execute();
                    }
                }
                if (inputData.type == InputType::OnThumbIdleContinuous && controller->IsThumbIdle(inputData.button))
                {
                    for (const auto& command : commands)
                    {
                        command->Execute();
                    }
                }
                if (inputData.type == InputType::OnThumbIdleOnce && controller->IsThumbIdle(inputData.button)
                    && !m_ExecutedIdleThumbstick)
                {
                    m_ExecutedIdleThumbstick = true;
                    for (const auto& command : commands)
                    {
                        command->Execute();
                    }
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
    if (std::none_of(m_ControllerPtrs.begin(), m_ControllerPtrs.end(),
        [controllerID](const auto& controller) { return controller->GetIdx() == controllerID; }))
    {
        m_ControllerPtrs.emplace_back(std::make_unique<XBox360Controller>(controllerID));
    }
}

glm::vec2 InputManager::GetThumbstickDirection(unsigned int controllerIdx, bool leftThumb) const
{
    if (controllerIdx < m_ControllerPtrs.size())
    {
        return m_ControllerPtrs[controllerIdx]->GetThumbStickPos(leftThumb);
    }
    return glm::vec2(0, 0);
}

std::pair<glm::vec2*, glm::vec2*> InputManager::GetThumbstickPositionsRef(unsigned int controllerIdx)
{
    AddControllerIfNeeded(controllerIdx);
    return m_ControllerPtrs[controllerIdx]->GetThumbStickPositions();
}
#endif

void InputManager::ProcessMouseEvents(const SDL_Event& e)
{
    switch (e.type)
    {
    case SDL_MOUSEMOTION:
        m_MousePosition = glm::ivec2(e.motion.x, e.motion.y);
        m_MouseMotion += glm::ivec2(e.motion.xrel, e.motion.yrel);
        for (const auto& [inputData, commands] : m_MouseActionMap)
        {
            if (inputData.type == InputType::OnMouseMove)
            {
                for (const auto& command : commands)
                {
                    command->Execute();
                }
            }
        }
        break;

    case SDL_MOUSEBUTTONDOWN:
    case SDL_MOUSEBUTTONUP:
        m_MousePosition = glm::ivec2(e.button.x, e.button.y);
        for (const auto& [inputData, commands] : m_MouseActionMap)
        {
            if (inputData.button == e.button.button)
            {
                if ((inputData.type == InputType::OnMouseButtonDown && e.type == SDL_MOUSEBUTTONDOWN) ||
                    (inputData.type == InputType::OnMouseButtonUp && e.type == SDL_MOUSEBUTTONUP))
                {
                    for (const auto& command : commands)
                    {
                        command->Execute();
                    }
                }
            }
        }
        break;

    case SDL_MOUSEWHEEL:
        m_MouseWheelDelta = e.wheel.y;
        for (const auto& [inputData, commands] : m_MouseActionMap)
        {
            if (inputData.type == InputType::OnMouseWheel)
            {
                for (const auto& command : commands)
                {
                    command->Execute();
                }
            }
        }
        break;
    }
}

bool InputManager::InputDataController::operator<(const InputDataController& other) const
{
    if (controllerID < other.controllerID) return true;
    if (controllerID > other.controllerID) return false;
    if (button < other.button) return true;
    if (button > other.button) return false;
    return type < other.type;
}

bool InputManager::InputDataKeyboard::operator<(const InputDataKeyboard& other) const
{
    if (key < other.key) return true;
    if (key > other.key) return false;
    return type < other.type;
}

bool InputManager::InputDataMouse::operator<(const InputDataMouse& other) const
{
    if (button < other.button) return true;
    if (button > other.button) return false;
    return type < other.type;
}