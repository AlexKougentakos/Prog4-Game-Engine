#pragma once

#include <unordered_map>
#include <string>
#include <functional>
#include "Singleton.h"

namespace ody
{
    class ImGuiManager final : public ody::Singleton<ImGuiManager>
    {
    public:
        void AddCheckBox(const std::string& id, bool& defaultState);
        //bool GetCheckBoxState(const std::string& id) { return m_CheckBoxStates[id]; }

        void AddButton(const std::string& id, std::function<void()> onClick);

        void Render();

    private:
        std::unordered_map<std::string, std::reference_wrapper<bool>> m_CheckBoxStates;
        std::unordered_map<std::string, std::function<void()>> m_Buttons;

        ImGuiManager() {}
        friend class Singleton<ImGuiManager>;
    };
}
