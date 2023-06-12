#include "PhysicsManager.h"

#include <utility>
#include <box2d/b2_world.h>
#include <box2d/b2_body.h>


void ody::PhysicsManager::Init(b2World* world)
{
	m_pActivePhysicsWorld = world;
}

b2Body* ody::PhysicsManager::CreateBody(const b2BodyDef& bodyDef) const
{
	return m_pActivePhysicsWorld->CreateBody(&bodyDef);
}

void ody::PhysicsManager::SetPhysicsWorld(b2World* world)
{
	m_pActivePhysicsWorld = world;
}

b2Fixture* ody::PhysicsManager::CreateFixture(b2Body* body, const b2FixtureDef& fixtureDef) const
{
	return body->CreateFixture(&fixtureDef);
}