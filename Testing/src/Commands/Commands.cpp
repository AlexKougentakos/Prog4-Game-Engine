#include "Commands.h"

#include "GameObject.h"
#include "RigidBodyComponent.h"
#include <Box2D/b2_body.h>
#include "../Components/PlayerMovementComponent.h"
#include "../Components/PlayerShootingComponent.h"


void StopMoveCommand::Execute()
{
	m_pActor->GetComponent<PlayerMovementComponent>()->StopMoving();
}

void JumpCommand::Execute()
{
	m_pActor->GetComponent<PlayerMovementComponent>()->Jump();
}

void MoveCommand::Execute()
{
	const auto b2dBody = m_pActor->GetComponent<ody::RigidBodyComponent>();

	if (m_UseRef)
		m_pActor->GetComponent<PlayerMovementComponent>()->Move(*m_MoveDirectionRef);
	//b2dBody->SetLinearVelocity({ m_MoveDirectionRef->x * m_MoveSpeed, m_MoveDirectionRef->y * m_MoveSpeed });

	else
	{
		m_pActor->GetComponent<PlayerMovementComponent>()->Move(m_MoveDirection);
	}
}

void ShootBubbleCommand::Execute()
{
	m_pActor->GetComponent<PlayerShootingComponent>()->Shoot();
}