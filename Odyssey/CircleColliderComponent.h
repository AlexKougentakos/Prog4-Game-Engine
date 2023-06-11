#pragma once
#include <SDL_rect.h>
#include <glm/glm.hpp>
#include "Constants.h"
#include "Component.h"
#include "Structs.h"

class b2Fixture;
struct b2Filter;

namespace ody
{

    class CircleColliderComponent : public Component
    {
    public:
        CircleColliderComponent(float radius, ColliderSettings settings = {});

        float GetRadius() const { return m_Radius; }
        ColliderSettings GetSettings() const { return m_Settings; }

        ~CircleColliderComponent() override = default;
        CircleColliderComponent(const CircleColliderComponent& other) = delete;
        CircleColliderComponent(CircleColliderComponent&& other) = delete;
        CircleColliderComponent& operator=(const CircleColliderComponent& other) = delete;
        CircleColliderComponent& operator=(CircleColliderComponent&& other) = delete;

        void Update() override {}
        void Render() const override {}

        b2Fixture* GetRuntimeFixture() const { return m_pRuntimeFixture; }
        void SetRuntimeFixture(b2Fixture* pFixture) { m_pRuntimeFixture = pFixture; }

        void AddIgnoreGroup(constants::CollisionGroups collisionIgnoreGroup);
        void RemoveIgnoreGroup(constants::CollisionGroups collisionIgnoreGroup);

    private:
        void InitializeFilter();
        friend class GameScene;

        float m_Radius;
        ColliderSettings m_Settings;

        b2Fixture* m_pRuntimeFixture{};
    };
}
