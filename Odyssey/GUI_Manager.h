#pragma once
#include "Singleton.h"

class GUI_Manager : public dae::Singleton<GUI_Manager>
{
public:
	void DrawGUI();

private:
	void DrawSceneHierarchy();
};
