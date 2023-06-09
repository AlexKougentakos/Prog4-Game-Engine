#include "Commands.h"

#include "GameObject.h"
#include "RigidBodyComponent.h"
#include <Box2D/b2_body.h>

void StopMoveCommand::Execute()
{
	const auto b2dBody = m_pActor->GetComponent<ody::RigidBodyComponent>()->GetRuntimeBody();

	b2dBody->SetLinearVelocity({0,b2dBody->GetLinearVelocity().y});
}
