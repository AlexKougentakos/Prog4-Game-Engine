#include "PlayerMovementComponent.h"

#include <iostream>
#include <Box2D/b2_edge_shape.h>

#include "GameObject.h"
#include "RigidBodyComponent.h"
#include <box2d/b2_world_callbacks.h>
#include <box2d/b2_math.h>

#include "AnimatedTextureComponent.h"
#include "CircleColliderComponent.h"
#include "DebugRenderer.h"
#include "GameScene.h"
#include "StateManager.h"
#include "Utils.h"
#include "../States/PlayerStates.h"

class MyRayCastCallback : public b2RayCastCallback
{
public:
    MyRayCastCallback()
    {
        m_fixture = nullptr;
    }

    float ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float fraction) override
    {
        m_fixture = fixture;
        m_point = point;
        m_normal = normal;
        m_fraction = fraction;
        return fraction;
    }

    b2Fixture* m_fixture;
    b2Vec2 m_point;
    b2Vec2 m_normal;
    float m_fraction;
};

PlayerMovementComponent::PlayerMovementComponent(const ody::RigidBodyComponent* rigidBody, ody::StateManager* pStateManager)
{
	m_pRigidBodyComponent = rigidBody;
    m_pPlayerStateManager = pStateManager;
}

void PlayerMovementComponent::Update()
{
    //Change collision ignore groups based on velocity
    const bool isGoingUp = m_pRigidBodyComponent->GetVelocity().y < 0.f;

    const auto colliderComponent = GetOwner()->GetComponent<ody::CircleColliderComponent>();
    if (isGoingUp)
        colliderComponent->AddIgnoreGroup(ody::constants::CollisionGroups::Group1);
    else
        colliderComponent->RemoveIgnoreGroup(ody::constants::CollisionGroups::Group1);

    HandleGroundChecking();
}

void PlayerMovementComponent::HandleGroundChecking()
{
    const auto radius = GetOwner()->GetComponent<ody::CircleColliderComponent>()->GetRadius();
        
    MyRayCastCallback callback;
    const b2Vec2 currentPos{
        ody::Utils::PixelsToMeters(GetOwner()->GetTransform()->GetWorldPosition().x + radius / 2.f),
        ody::Utils::PixelsToMeters(GetOwner()->GetTransform()->GetWorldPosition().y + radius / 2.f)
    };
    const b2Vec2 endPoint{ currentPos - b2Vec2{ 0, -0.5f } };

    ody::DebugRenderer::GetInstance().DrawSegment(
        currentPos, endPoint,
        { 1.f, 0, 0, 1.f }
    );

    ody::SceneManager::GetInstance().GetActiveScene()->GetPhysicsWorld()->RayCast(&callback, currentPos, endPoint);

    if (callback.m_fixture != nullptr)
    {
        if (m_HasJumped) m_HasJumped = false;
        m_IsGrounded = true;
    }
    else
    {
        m_IsGrounded = false;
    }
}

void PlayerMovementComponent::Move(const glm::vec2& direction)
{
    if (direction.x < 0.f && m_IsSpriteLookingRight)
    {
        GetOwner()->GetComponent<ody::AnimatedTextureComponent>()->Flip();
        m_IsSpriteLookingRight = false;
    }
    else if (direction.x > 0.f && !m_IsSpriteLookingRight)
    {
    	GetOwner()->GetComponent<ody::AnimatedTextureComponent>()->Flip();
		m_IsSpriteLookingRight = true;
	}

    m_pPlayerStateManager->ChangeState(std::make_unique<PlayerWalk>(GetOwner()));

    auto vel = m_pRigidBodyComponent->GetVelocity();
    vel.x = direction.x * m_MoveSpeed;
    m_pRigidBodyComponent->SetVelocity({ vel.x, vel.y });
}

void PlayerMovementComponent::StopMoving() const
{
	m_pPlayerStateManager->ChangeState(std::make_unique<PlayerIdle>(GetOwner()));
	auto vel = m_pRigidBodyComponent->GetVelocity();
	m_pRigidBodyComponent->SetVelocity({ 0.f, vel.y });
}


void PlayerMovementComponent::Jump()
{
	if (m_HasJumped || !m_IsGrounded) return;

	m_HasJumped = true;
	m_pRigidBodyComponent->AddForce({0, -m_JumpForce });
}

glm::vec2 PlayerMovementComponent::GetLookDir() const
{
    if (m_IsSpriteLookingRight)
        return { 1.f, 0.f };
    
    return { -1.f, 0.f };
}