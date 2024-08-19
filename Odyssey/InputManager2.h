#pragma once
#include <iostream>
#include <map>
#include <vector>
#include <memory>
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

        // Template implementations
        template<typename T, typename... Args>
        void AddKeyboardCommand(unsigned int keyboardKey, InputType type, Args&&... args)
        {
            const InputDataKeyboard inputData{ keyboardKey, type };
            m_KeyboardActionMap[inputData].emplace_back(std::make_unique<T>(std::forward<Args>(args)...));
        }

        template<typename T, typename... Args>
        void AddMouseCommand(int mouseButton, InputType type, Args&&... args)
        {
            const InputDataMouse inputData{ mouseButton, type };
            m_MouseActionMap[inputData].emplace_back(std::make_unique<T>(std::forward<Args>(args)...));
        }

#ifndef __EMSCRIPTEN__
        template<typename T, typename... Args>
        void AddControllerCommand(XBox360Controller::ControllerButton button, unsigned int controllerID, InputType type, Args&&... args)
        {
            const InputDataController inputData{ controllerID, button, type };
            m_ControllerActionMap[inputData].emplace_back(std::make_unique<T>(std::forward<Args>(args)...));
        }

        glm::vec2 GetThumbstickDirection(unsigned int controllerIdx, bool leftThumb = true) const;
        std::pair<glm::vec2*, glm::vec2*> GetThumbstickPositionsRef(unsigned int controllerIdx);
#endif

        glm::ivec2 GetMousePosition() const { return m_MousePosition; }
        glm::ivec2 GetMouseMotion() const { return m_MouseMotion; }
        void ResetMouseMotion() { m_MouseMotion = glm::vec2{}; }
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
        std::map<InputDataController, std::vector<std::unique_ptr<Command>>> m_ControllerActionMap{};
        std::vector<std::unique_ptr<XBox360Controller>> m_ControllerPtrs{};
        bool m_ExecutedIdleThumbstick{ false };
        void AddControllerIfNeeded(unsigned int controllerID);
#endif

        std::map<InputDataKeyboard, std::vector<std::unique_ptr<Command>>> m_KeyboardActionMap{};
        std::map<InputDataMouse, std::vector<std::unique_ptr<Command>>> m_MouseActionMap{};
        bool m_ExecutedOnDown{ false };
        glm::ivec2 m_MousePosition{};
        glm::ivec2 m_MouseMotion{};
        int m_MouseWheelDelta{};
        void ProcessMouseEvents(const SDL_Event& e);
    };
}