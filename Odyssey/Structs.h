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

	struct ColliderSettings
	{
		float density = 1.0f;
		float friction = 0.3f;
		float restitution = 0.5f;

		constants::CollisionGroups collisionGroup = constants::CollisionGroups::None;

		bool isSensor = false;
	};

#pragma region SCENE_STRUCTS
	
	enum class GameEvent
	{};

#pragma endregion
}