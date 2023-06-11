#include "ImGuiManager.h"
namespace ody
{
void ImGuiManager::AddCheckBox(const std::string& id, bool defaultState)
{
	if (m_CheckBoxStates.find(id) == m_CheckBoxStates.end())
		m_CheckBoxStates[id] = defaultState;
}

void ImGuiManager::Render()
{
	ImGui::Begin("ImGuiManager - Global");

    for (auto& checkBox : m_CheckBoxStates)
        ImGui::Checkbox(checkBox.first.c_str(), &checkBox.second);

	ImGui::End();
}


}
