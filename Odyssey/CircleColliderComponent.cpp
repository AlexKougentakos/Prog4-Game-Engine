#include "CircleColliderComponent.h"
#include <Box2D/b2_fixture.h>
#include <Box2D/b2_circle_shape.h>

#include "Utils.h"

ody::CircleColliderComponent::CircleColliderComponent(float radius, ColliderSettings settings, b2Body* bodyToAttchTo) :
	m_Radius(radius), m_Settings(settings)
{
    m_RuntimeCreate = bodyToAttchTo != nullptr;

	m_pRuntimeBody = bodyToAttchTo;
}

void ody::CircleColliderComponent::Initialize()
{
	if (!m_RuntimeCreate)
		return;
    auto center = b2Vec2{ Utils::PixelsToMeters(m_Radius), Utils::PixelsToMeters(m_Radius) };

	b2CircleShape shape{};
	shape.m_radius = Utils::PixelsToMeters(m_Radius);
	shape.m_p = center;
	b2FixtureDef fixtureDef{};
    Utils::ColliderSettingsToB2DFixtureDef(m_Settings, fixtureDef);

    b2FixtureUserData userData{};
    userData.pointer = reinterpret_cast<uintptr_t>(GetOwner());
    fixtureDef.userData = userData;

    fixtureDef.shape = &shape;
	b2Fixture* pFixture = m_pRuntimeBody->CreateFixture(&fixtureDef);
	m_pRuntimeFixture = pFixture;
	InitializeFilter();
}


void ody::CircleColliderComponent::InitializeFilter()
{
    b2Filter filter{};
    filter.categoryBits = static_cast<uint16>(m_Settings.collisionGroup); //Set the collision group
    filter.maskBits = 0xFFFF;  // collide with everything
    filter.groupIndex = 0;  // no specific group

    m_pRuntimeFixture->SetFilterData(filter);
}

void ody::CircleColliderComponent::AddIgnoreGroup(constants::CollisionGroups collisionIgnoreGroup) const
{
    b2Filter filter = m_pRuntimeFixture->GetFilterData();

    // Use bitwise AND with the inverse of the category to clear that bit
    filter.maskBits &= ~static_cast<uint16>(collisionIgnoreGroup);
    m_pRuntimeFixture->SetFilterData(filter);
}

void ody::CircleColliderComponent::RemoveIgnoreGroup(constants::CollisionGroups collisionIgnoreGroup) const
{
    b2Filter filter = m_pRuntimeFixture->GetFilterData();

    // Use bitwise OR to set the category bit
    filter.maskBits |= static_cast<uint16>(collisionIgnoreGroup);
    m_pRuntimeFixture->SetFilterData(filter);
}
