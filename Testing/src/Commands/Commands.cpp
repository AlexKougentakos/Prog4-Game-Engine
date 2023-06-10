#include "Commands.h"

#include "GameObject.h"
#include "RigidBodyComponent.h"
#include <Box2D/b2_body.h>
#include "../Components/PlayerMovementComponent.h"


void StopMoveCommand::Execute()
{
	const auto b2dBody = m_pActor->GetComponent<ody::RigidBodyComponent>();

	auto vel = b2dBody->GetVelocity();
	vel.x = 0;
	b2dBody->SetVelocity(vel);
}

void JumpCommand::Execute()
{
	m_pActor->GetComponent<PlayerMovementComponent>()->Jump();
}
