#include "Command.h"

#include <iostream>
#include <Box2D/b2_body.h>

#include "GameObject.h"
#include "TransformComponent.h"
#include "RigidBodyComponent.h"

//Todo: Move the move command to a seperate file
void ody::MoveCommand::Execute()
{
	std::cout << "test";

	const auto b2dBody = m_pActor->GetComponent<ody::RigidBodyComponent>();

	if (m_UseRef)
		__debugbreak();
		//b2dBody->SetLinearVelocity({ m_MoveDirectionRef->x * m_MoveSpeed, m_MoveDirectionRef->y * m_MoveSpeed });

	else
	{
		//float timestep = 1.f / 60.f; // Assuming your physics updates at 60Hz
		float mass = 200;

		b2Vec2 force(m_MoveDirection.x * m_MoveSpeed * mass, 0);
		b2dBody->AddForce({ force.x, force.y });
	}



}