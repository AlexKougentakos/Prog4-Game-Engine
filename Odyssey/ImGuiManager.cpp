#include "ImGuiManager.h"

#include <utility>
#include "imgui.h"

namespace ody
{
    void ImGuiManager::AddCheckBox(const std::string& id, bool& stateRef)
    {
        // Use emplace to construct the element in place without default construction
        const auto result = m_CheckBoxStates.emplace(std::make_pair(id, std::ref(stateRef)));

        // result.second is true if the insertion took place, false if the id already existed
        // result.first is an iterator to the inserted or existing element
        if (!result.second) {
            // The id already existed, update the reference
            result.first->second = std::ref(stateRef);
        }
    }


    // Implement AddButton
    void ImGuiManager::AddButton(const std::string& id, std::function<void()> onClick)
    {
        m_Buttons[id] = std::move(onClick);
    }

    void ImGuiManager::Render()
    {
        ImGui::Begin("ImGuiManager - Global");

        if (ImGui::CollapsingHeader("Checkboxes"))
        {
            for (auto& checkBox : m_CheckBoxStates) 
            {
                ImGui::Checkbox(checkBox.first.c_str(), &(checkBox.second.get()));
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
