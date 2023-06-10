#include "PhysicsEngine.h"
#include <Box2D/b2_world.h>
#include "Structs.h"

namespace ody
{
	PhysicsEngine::PhysicsEngine() : m_pPhysicsWorld{}
	{}

	PhysicsEngine::~PhysicsEngine()
	{
		delete m_pPhysicsWorld;
		m_pPhysicsWorld = nullptr;
	}

	void PhysicsEngine::SetPhysicsWorld(const glm::vec2& gravity)
	{
		m_pPhysicsWorld->SetGravity({gravity.x, gravity.y});
	}

	void PhysicsEngine::Initialize()
	{
		m_pPhysicsWorld = new b2World({ 0.f, 0.f });
	}

	void PhysicsEngine::Step(float timeStep, int velocityIterations, int positionIterations)
	{
		m_pPhysicsWorld->Step(timeStep, velocityIterations, positionIterations);
		m_PhysicsEvent.EventTriggered(ody::GameEvent::StepUpdateEvent);
	}
}