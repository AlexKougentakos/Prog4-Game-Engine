#pragma once
#include "Singleton.h"

class GUI_Manager : public ody::Singleton<GUI_Manager>
{
public:
	void DrawGUI();

private:
	void DrawSceneHierarchy();
};
