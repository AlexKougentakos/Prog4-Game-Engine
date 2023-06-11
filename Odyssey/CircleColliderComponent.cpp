#include "CircleColliderComponent.h"
#include <Box2D/b2_fixture.h>

ody::CircleColliderComponent::CircleColliderComponent(float radius, ColliderSettings settings) :
    m_Radius(radius), m_Settings(settings)
{
    // Perform any additional initializations here if needed
}

void ody::CircleColliderComponent::InitializeFilter()
{
    b2Filter filter{};
    filter.categoryBits = static_cast<uint16>(m_Settings.collisionGroup); //Set the collision group
    filter.maskBits = 0xFFFF;  // collide with everything
    filter.groupIndex = 0;  // no specific group

    m_pRuntimeFixture->SetFilterData(filter);
}

void ody::CircleColliderComponent::AddIgnoreGroup(constants::CollisionGroups category)
{
    b2Filter filter = m_pRuntimeFixture->GetFilterData();

    // Use bitwise AND with the inverse of the category to clear that bit
    filter.maskBits &= ~static_cast<uint16>(category);
    m_pRuntimeFixture->SetFilterData(filter);
}

void ody::CircleColliderComponent::RemoveIgnoreGroup(constants::CollisionGroups category)
{
    b2Filter filter = m_pRuntimeFixture->GetFilterData();

    // Use bitwise OR to set the category bit
    filter.maskBits |= static_cast<uint16>(category);
    m_pRuntimeFixture->SetFilterData(filter);
}
