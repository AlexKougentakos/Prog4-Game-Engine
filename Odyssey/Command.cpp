#include "Command.h"

#include <iostream>
#include <Box2D/b2_body.h>

#include "GameObject.h"
#include "TransformComponent.h"
#include "RigidBodyComponent.h"

//Todo: Move the move command to a seperate file
void ody::MoveCommand::Execute()
{
	const auto b2dBody = m_pActor->GetComponent<ody::RigidBodyComponent>();

	if (m_UseRef)
		__debugbreak();
		//b2dBody->SetLinearVelocity({ m_MoveDirectionRef->x * m_MoveSpeed, m_MoveDirectionRef->y * m_MoveSpeed });

	else
	{
		//b2dBody->AddForce({ m_MoveDirection * m_MoveSpeed });
		//return;
		auto vel = b2dBody->GetVelocity();
		vel.x = m_MoveDirection.x * m_MoveSpeed;
		std::cout << vel.y << std::endl;
		b2dBody->SetVelocity({ vel.x, vel.y });
	}
}