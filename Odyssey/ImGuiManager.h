#pragma once

#include <unordered_map>
#include <string>
#include "imgui.h"
#include "Singleton.h"
namespace ody
{
class ImGuiManager final : public ody::Singleton<ImGuiManager>
{
public:
    void AddCheckBox(const std::string& id, bool defaultState);
    bool GetCheckBoxState(const std::string& id) { return m_CheckBoxStates[id]; }

    void Render();

private:
    // Store the state of checkboxes
    std::unordered_map<std::string, bool> m_CheckBoxStates;

    ImGuiManager() {}
    friend class Singleton<ImGuiManager>;
};

}
