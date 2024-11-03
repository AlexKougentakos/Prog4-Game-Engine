#pragma once
#include "Constants.h"
namespace ody
{
#pragma region SCENE_STRUCTS
	class InputManager;
	class GameTime;


	struct SceneSettings
	{
		bool drawSceneHierarchy{ true };
	};

#pragma endregion

#pragma region SCENE_STRUCTS
	
	enum class GameEvent
	{
		PlayCards,
		Pass,
		DeclareTichu
	};

#pragma endregion
}