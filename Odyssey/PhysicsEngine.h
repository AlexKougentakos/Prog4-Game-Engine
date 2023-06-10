#pragma once
#include "Singleton.h"
#include "Subject.h"
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <memory>

class b2World;

namespace ody
{

class PhysicsEngine : public Singleton<PhysicsEngine>
{
public:
	virtual ~PhysicsEngine();

	void Initialize();
	void SetPhysicsWorld(const glm::vec2& gravity);

	void Step(float timeStep, int velocityIterations, int positionIterations);
	b2World* GetWorld() const { return m_pPhysicsWorld; }

	void AddObserver(IObserver* observer) { m_PhysicsEvent.AddObserver(observer); }

protected:
	friend class Singleton<PhysicsEngine>;
	PhysicsEngine();

private:
	b2World* m_pPhysicsWorld;
	float m_PixelsPerMeter{16.f};
	Subject m_PhysicsEvent{};
};

}



