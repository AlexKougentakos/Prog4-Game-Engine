#include "ColliderComponent.h"
#include "GameObject.h"
#include "Utils.h"
#include <Box2D/b2_fixture.h>
#include <box2d/b2_body.h>
#include <Box2D/b2_polygon_shape.h>

ody::ColliderComponent::ColliderComponent(const glm::vec2& dimensions, ColliderSettings settings, b2Body* bodyToAttchTo):
m_Dimensions(dimensions), m_Settings(settings)
{
    m_RuntimeCreate = bodyToAttchTo != nullptr;

    m_pRuntimeBody = bodyToAttchTo;
}

void ody::ColliderComponent::Initialize()
{
    if (!m_RuntimeCreate)
        return;
    const auto center = b2Vec2{ Utils::PixelsToMeters(m_Dimensions.x), Utils::PixelsToMeters(m_Dimensions.y) };

    b2PolygonShape shape{};
    shape.m_centroid = center;

    b2FixtureDef fixtureDef{};
    Utils::ColliderSettingsToB2DFixtureDef(m_Settings, fixtureDef);
fixtureDef.shape = &shape;
    b2FixtureUserData userData{};
    userData.pointer = reinterpret_cast<uintptr_t>(GetOwner());
    

    fixtureDef.userData = userData;
    b2Fixture* pFixture = m_pRuntimeBody->CreateFixture(&fixtureDef);
    m_pRuntimeFixture = pFixture;

    InitializeFilter();
}

void ody::ColliderComponent::InitializeFilter()
{
    b2Filter filter{};
    filter.categoryBits = static_cast<uint16>(m_Settings.collisionGroup); //Set the collision group of the player
    filter.maskBits = 0xFFFF;  // collide with everything
    filter.groupIndex = 0;  // no specific group

    m_pRuntimeFixture->SetFilterData(filter);
}

void ody::ColliderComponent::AddIgnoreGroup(constants::CollisionGroups collisionIgnoreGroup)
{
    b2Filter filter = m_pRuntimeFixture->GetFilterData();

    // Use bitwise AND with the inverse of the category to clear that bit
    filter.maskBits &= ~static_cast<uint16>(collisionIgnoreGroup);
    m_pRuntimeFixture->SetFilterData(filter);
}

void ody::ColliderComponent::RemoveIgnoreGroup(constants::CollisionGroups collisionIgnoreGroup)
{
    b2Filter filter = m_pRuntimeFixture->GetFilterData();

    // Use bitwise OR to set the category bit
    filter.maskBits |= static_cast<uint16>(collisionIgnoreGroup);
    m_pRuntimeFixture->SetFilterData(filter);
}