#include "RigidBodyComponent.h"
#include "Utils.h"
#include "PhysicsEngine.h"
#include "GameObject.h"

#include <Box2D/b2_world.h>
#include <Box2D/b2_body.h>
#include <Box2D/b2_polygon_shape.h>
#include <Box2D/b2_fixture.h>

ody::RigidBodyComponent::RigidBodyComponent(RigidBodySettings settings, ColliderSettings colliderSettings)
	:m_Settings(settings)
{
	b2BodyDef bodyDef{};
	Utils::RigidbodySettingsToB2DBodyDef(settings, bodyDef);

	m_pRuntimeBody = PhysicsEngine::GetInstance().GetWorld()->CreateBody(&bodyDef);

	b2PolygonShape boxShape{};
	//Add a way to set this
	boxShape.SetAsBox(16.f, 16.f);

	b2FixtureDef fixtureDef{};
	Utils::ColliderSettingsToB2DFixtureDef(colliderSettings, fixtureDef);
	fixtureDef.shape = &boxShape;

	m_pRuntimeBody->SetFixedRotation(bodyDef.fixedRotation);

	m_pRuntimeBody->CreateFixture(&fixtureDef);

	PhysicsEngine::GetInstance().AddObserver(this);
}

void ody::RigidBodyComponent::Initialize()
{
	//GetOwner()->GetTransform()->Translate(m_pRuntimeBody->GetPosition().x, m_pRuntimeBody->GetPosition().y);
	auto pos = GetOwner()->GetTransform()->GetWorldPosition();
	m_pRuntimeBody->SetTransform({ pos.x, pos.y }, 0.f);
}

void ody::RigidBodyComponent::AddForce(const glm::vec2& force) const
{
	m_pRuntimeBody->ApplyLinearImpulseToCenter({ force.x, force.y }, true);
	//m_pRuntimeBody->ApplyLinearImpulseToCenter({ force.x, force.y }, true);
}

void ody::RigidBodyComponent::OnNotify(GameEvent /*gameEvent*/)
{
	if (m_pRuntimeBody->GetType() == b2_staticBody) return;

	GetOwner()->GetTransform()->SetPosition(m_pRuntimeBody->GetPosition().x, m_pRuntimeBody->GetPosition().y);
}