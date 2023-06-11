#include "Utils.h"
#include "RigidBodyComponent.h"
#include "ColliderComponent.h"

#include "Box2D/b2_fixture.h"
#include "Constants.h"

#include <random>

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
		bodyOut.position.Set(PixelsToMeters(bodySettingsIn.startingPosition.x), PixelsToMeters(bodySettingsIn.startingPosition.y));
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

	float Utils::RandomFloat(float min, float max)
	{
		std::random_device rd;  // Random number from hardware
		std::mt19937 gen(rd()); // Initialize Mersenne Twister random number generator
		std::uniform_real_distribution<> distr(min, max); // Define the distribution

		return static_cast<float>( distr(gen));
	}

	bool Utils::CheckChance(int percentage)
	{
		if (percentage < 0 || percentage > 100) 
		{
			throw std::invalid_argument("percentage must be between 0 and 100");
		}

		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<> distr(1, 100);

		const int roll = distr(gen);

		return roll <= percentage;
	}

}
