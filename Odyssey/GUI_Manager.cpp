#include "GUI_Manager.h"
#include <imgui.h>
#include <vector>

#include "imgui.h"
#include "TrashTheCache.h"
#include "backends/imgui_impl_sdl2.h"
#include "../3rdParty/imgui-1.89.4/backends/imgui_impl_opengl2.h"
#include "../ImPlot/implot.h"

void GUI_Manager::DrawGUI()
{
	DrawSceneHierarchy();
}

void GUI_Manager::DrawSceneHierarchy()
{
	ImGui::SetNextWindowSize(ImVec2{ 200, 400 });
	if (ImGui::Begin("SceneHierarchy"))
	{

	}ImGui::End();
}


