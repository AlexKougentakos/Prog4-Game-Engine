#include "PlayerMovementComponent.h"

#include <iostream>
#include <Box2D/b2_edge_shape.h>

#include "GameObject.h"
#include "RigidBodyComponent.h"
#include <box2d/b2_world_callbacks.h>
#include <box2d/b2_math.h>

#include "CircleColliderComponent.h"
#include "DebugRenderer.h"
#include "GameScene.h"
#include "Utils.h"

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


PlayerMovementComponent::PlayerMovementComponent(const ody::RigidBodyComponent* rigidBody)
{
	m_pRigidBodyComponent = rigidBody;
}

void PlayerMovementComponent::Update()
{
    //Change collision ignore groups based on velocity
    bool isGoingUp = m_pRigidBodyComponent->GetVelocity().y < 0.f;

    auto colliderComponent = GetOwner()->GetComponent<ody::CircleColliderComponent>();
    if (isGoingUp)
    {
        colliderComponent->AddIgnoreGroup(ody::constants::CollisionGroups::Group1);
    }
    else
    {
        colliderComponent->RemoveIgnoreGroup(ody::constants::CollisionGroups::Group1);
    }

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

void PlayerMovementComponent::Jump()
{
	if (m_HasJumped || !m_IsGrounded) return;

	m_HasJumped = true;
	m_pRigidBodyComponent->AddForce({0, -33.f });
}