#include "ImGuiManager.h"
#include "imgui.h"

namespace ody
{
    void ImGuiManager::AddCheckBox(const std::string& id, bool defaultState)
    {
        if (m_CheckBoxStates.find(id) == m_CheckBoxStates.end())
            m_CheckBoxStates[id] = defaultState;
    }

    // Implement AddButton
    void ImGuiManager::AddButton(const std::string& id, std::function<void()> onClick)
    {
        m_Buttons[id] = onClick;
    }

    void ImGuiManager::Render()
    {
        ImGui::Begin("ImGuiManager - Global");

        if (ImGui::CollapsingHeader("Checkboxes"))
        {
            for (auto& checkBox : m_CheckBoxStates)
            {
                ImGui::Checkbox(checkBox.first.c_str(), &checkBox.second);
            }
        }

        if (ImGui::CollapsingHeader("Buttons"))
        {
            for (auto& button : m_Buttons)
            {
                if (ImGui::Button(button.first.c_str()))
                {
                    button.second();
                }
            }
        }

        ImGui::End();
    }



}
