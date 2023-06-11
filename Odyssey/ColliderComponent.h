#pragma once
#include <SDL_rect.h>
#include <glm/glm.hpp>
#include "Constants.h"
#include "Component.h"
#include "Structs.h"

class b2Body;
class b2Fixture;
struct b2Filter;

namespace ody
{

class ColliderComponent : public Component
{
public:
	//ColliderComponent() = default;
	ColliderComponent(const glm::vec2& dimensions, ColliderSettings settings = {}, b2Body* bodyToAttchTo = nullptr);

	const glm::vec2& GetDimensions() const { return m_Dimensions; }

	~ColliderComponent() override = default;
	ColliderComponent(const ColliderComponent& other) = delete;
	ColliderComponent(ColliderComponent&& other) = delete;
	ColliderComponent& operator=(const ColliderComponent& other) = delete;
	ColliderComponent& operator=(ColliderComponent&& other) = delete;

	void Initialize() override;
	void Update() override {}
	void Render() const override {}

	b2Fixture* GetRuntimeFixture() const { return m_pRuntimeFixture; }
	void SetRuntimeFixture(b2Fixture* pFixture) { m_pRuntimeFixture = pFixture; }

	void AddIgnoreGroup(constants::CollisionGroups );
	void RemoveIgnoreGroup(constants::CollisionGroups collisionIgnoreGroup);

	ColliderSettings GetSettings() const { return m_Settings; }

private:
	friend class GameScene;
	void InitializeFilter();

	glm::vec2 m_Dimensions{};
	glm::vec2 m_Offset{};

	bool m_RuntimeCreate{ false };
	ColliderSettings m_Settings{};

	b2Fixture* m_pRuntimeFixture{};
	b2Body* m_pRuntimeBody{};
};

}