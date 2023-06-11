#include "Utils.h"
#include "RigidBodyComponent.h"
#include "ColliderComponent.h"

#include "Box2D/b2_fixture.h"
#include "Constants.h"

namespace ody
{
	void Utils::RigidbodySettingsToB2DBodyDef(RigidBodySettings bodySettingsIn, b2BodyDef& bodyOut)
	{
		bodyOut.type = RigidbodyTypeToB2Type(bodySettingsIn.bodyType);
		bodyOut.enabled = bodySettingsIn.enabled;
		bodyOut.awake = bodySettingsIn.awake;
		bodyOut.bullet = bodySettingsIn.bullet;
		bodyOut.fixedRotation = bodySettingsIn.fixedRotation;
		bodyOut.gravityScale = bodySettingsIn.gravityScale;
	}

	b2BodyType Utils::RigidbodyTypeToB2Type(BodyType typeIn)
	{
		switch (typeIn)
		{
			case BodyType::Static:
				return b2_staticBody;
			case BodyType::Dynamic:
				return b2_dynamicBody;
			case BodyType::Kinematic:
				return b2_kinematicBody;
			default:
				return b2_staticBody;
		}
	}

	void Utils::ColliderSettingsToB2DFixtureDef(ColliderSettings colliderSettingsIn, b2FixtureDef& fixtureOut)
	{
		fixtureOut.density = colliderSettingsIn.density;
		fixtureOut.friction = colliderSettingsIn.friction;
		fixtureOut.restitution = colliderSettingsIn.restitution;
		fixtureOut.isSensor = colliderSettingsIn.isSensor;

		fixtureOut.filter.categoryBits = static_cast<uint16>(colliderSettingsIn.collisionGroup);
	}

	float Utils::MetersToPixels(float meters)
	{
		return meters * constants::g_PixelsPerMeter;
	}

	float Utils::PixelsToMeters(float pixels)
	{
		return pixels / constants::g_PixelsPerMeter;
	}
}
