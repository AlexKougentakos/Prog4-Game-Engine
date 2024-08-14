#pragma once
#include <iostream>
#include <map>
#include <memory>
#include <vector>
#include <glm/glm.hpp>
#include <SDL.h>
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
            OnThumbIdleOnce,
            OnThumbIdleContinuous,
            OnMouseMove,
            OnMouseButtonDown,
            OnMouseButtonUp,
            OnMouseWheel
        };

        void AddKeyboardCommand(unsigned int keyboardKey, InputType type, std::unique_ptr<Command> pCommand);
        void AddMouseCommand(int mouseButton, InputType type, std::unique_ptr<Command> pCommand);

#ifndef __EMSCRIPTEN__
        void AddControllerCommand(XBox360Controller::ControllerButton button, unsigned int controllerID, InputType type, std::unique_ptr<Command> pCommand);
        glm::vec2 GetThumbstickDirection(unsigned int controllerIdx, bool leftThumb = true) const;
        std::pair<glm::vec2*, glm::vec2*> GetThumbstickPositionsRef(unsigned int controllerIdx)
        {
            AddControllerIfNeeded(controllerIdx);
            return m_ControllerPtrs[controllerIdx]->GetThumbStickPositions();
        }
#endif

        glm::ivec2 GetMousePosition() const { return m_MousePosition; }
        glm::ivec2 GetMouseMotion() const { return m_MouseMotion; }
        int GetMouseWheelDelta() const { return m_MouseWheelDelta; }

    private:
#ifndef __EMSCRIPTEN__
        struct InputDataController
        {
            unsigned int controllerID{};
            XBox360Controller::ControllerButton button{};
            InputType type{};
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
            bool operator<(const InputDataKeyboard& other) const
            {
                if (key < other.key) return true;
                if (key > other.key) return false;
                return type < other.type;
            }
        };

        struct InputDataMouse
        {
            int button{};
            InputType type{};
            bool operator<(const InputDataMouse& other) const
            {
                if (button < other.button) return true;
                if (button > other.button) return false;
                return type < other.type;
            }
        };

        friend class Singleton<InputManager>;
        InputManager() = default;

#ifndef __EMSCRIPTEN__
        std::map<InputDataController, std::unique_ptr<Command>> m_ControllerActionMap{};
        std::vector<std::unique_ptr<XBox360Controller>> m_ControllerPtrs{};
        bool m_ExecutedIdleThumbstick{ false };
        void AddControllerIfNeeded(unsigned int controllerID);
#endif

        std::map<InputDataKeyboard, std::unique_ptr<Command>> m_KeyboardActionMap{};
        std::map<InputDataMouse, std::unique_ptr<Command>> m_MouseActionMap{};
        bool m_ExecutedOnDown{ false };

        glm::ivec2 m_MousePosition{};
        glm::ivec2 m_MouseMotion{};
        int m_MouseWheelDelta{};

        void ProcessMouseEvents(const SDL_Event& e);
    };
}