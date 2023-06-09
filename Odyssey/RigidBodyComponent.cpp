#include "RigidBodyComponent.h"

#include <Box2D/b2_body.h>

ody::RigidBodyComponent::RigidBodyComponent(RigidBodySettings settings)
	:m_Settings(settings)
{
}

void ody::RigidBodyComponent::AddForce(const glm::vec2& force) const
{
	m_pRuntimeBody->ApplyLinearImpulseToCenter({ force.x, force.y }, true);
	//m_pRuntimeBody->ApplyLinearImpulseToCenter({ force.x, force.y }, true);
}
