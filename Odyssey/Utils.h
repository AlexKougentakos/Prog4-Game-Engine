	#pragma once
#include "Box2D/b2_body.h"

#pragma region Forward Declarations External
//Rigidbody stuff

struct b2BodyDef;
struct b2FixtureDef;

//enum b2BodyType : int;
#pragma endregion Forward Declarations External

namespace ody
{

#pragma region Forward Declarations
//Rigidbody stuff
struct RigidBodySettings;
struct ColliderSettings;

enum class BodyType;
#pragma endregion Forward Declarations

/**
 * \brief Here you can place any helper functions you might need
 */
class Utils final
{
public:
	static void RigidbodySettingsToB2DBodyDef(RigidBodySettings bodySettingsIn, b2BodyDef& bodyOut);
	static b2BodyType RigidbodyTypeToB2Type(BodyType typeIn);
	static void ColliderSettingsToB2DFixtureDef(ColliderSettings colliderSettingsIn, b2FixtureDef& fixtureOut);	
};

}
