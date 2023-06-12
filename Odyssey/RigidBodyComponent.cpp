#include "RigidBodyComponent.h"
#include "GameObject.h"

#include <Box2D/b2_body.h>
#include <Box2D/b2_world.h>

#include "PhysicsManager.h"
#include "Utils.h"

ody::RigidBodyComponent::RigidBodyComponent(RigidBodySettings settings, bool runtimeCreate)
	:m_Settings(settings)
{
	m_RuntimeCreate = runtimeCreate;
}

void ody::RigidBodyComponent::Initialize()
{
	if (!m_RuntimeCreate)
		return;

	b2BodyDef bodyDef{};
	Utils::RigidbodySettingsToB2DBodyDef(m_Settings, bodyDef);

	b2Body* pBody = PhysicsManager::GetInstance().CreateBody(bodyDef);
	m_pRuntimeBody = pBody;
	pBody->SetFixedRotation(bodyDef.fixedRotation);
}


void ody::RigidBodyComponent::AddForce(const glm::vec2& force) const
{
	//m_pRuntimeBody->ApplyLinearImpulseToCenter({ force.x, force.y }, true);
	m_pRuntimeBody->ApplyForce(b2Vec2{ force.x, force.y}, m_pRuntimeBody->GetWorldCenter(), true);
}

void ody::RigidBodyComponent::SetVelocity(const glm::vec2& velocity) const
{
	m_pRuntimeBody->SetLinearVelocity({ Utils::PixelsToMeters(velocity.x), Utils::PixelsToMeters(velocity.y) });
}

glm::vec2 ody::RigidBodyComponent::GetVelocity() const
{
	const auto vel = m_pRuntimeBody->GetLinearVelocity();
	return { Utils::MetersToPixels(vel.x), Utils::MetersToPixels(vel.y) };
}

