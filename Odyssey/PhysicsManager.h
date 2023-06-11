#pragma once
#include <Box2D/b2_world.h>

#include "Singleton.h"

struct b2FixtureDef;
struct b2BodyDef;
class b2Body;
class b2World;
namespace ody
{
	class PhysicsManager : public Singleton<PhysicsManager>
	{
	public:
		virtual ~PhysicsManager() override = default;

		//apply rule of 5
		PhysicsManager(const PhysicsManager& other) = delete;
		PhysicsManager(PhysicsManager&& other) = delete;
		PhysicsManager& operator=(const PhysicsManager& other) = delete;
		PhysicsManager& operator=(PhysicsManager&& other) = delete;

		void Init(b2World* world);

		b2Body* CreateBody(const b2BodyDef& bodyDef) const;
		b2Fixture* CreateFixture(b2Body* body, const b2FixtureDef& fixtureDef) const;

		void SetPhysicsWorld(b2World* world) { m_pActivePhysicsWorld = world; }
	private:
		friend class Singleton<PhysicsManager>;
		PhysicsManager() = default;

		b2World* m_pActivePhysicsWorld{};
	};
}
