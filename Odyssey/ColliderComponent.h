#pragma once
#include <SDL_rect.h>
#include <glm/glm.hpp>

#include "Component.h"

namespace ody
{
struct ColliderSettings
{
	float density = 1.0f;
	float friction = 0.3f;
	float restitution = 0.5f;

	bool isSensor = false;
};

class ColliderComponent : public Component
{
public:
	//ColliderComponent() = default;
	ColliderComponent(const glm::vec2& dimensions, ColliderSettings settings = {});

	const glm::vec2& GetDimensions() const { return m_Dimensions; }

	~ColliderComponent() override = default;
	ColliderComponent(const ColliderComponent& other) = delete;
	ColliderComponent(ColliderComponent&& other) = delete;
	ColliderComponent& operator=(const ColliderComponent& other) = delete;
	ColliderComponent& operator=(ColliderComponent&& other) = delete;

	void Update() override {}
	void Render() const override {}

	void* GetRuntimeFixture() const { return m_pRuntimeFixture; }
	void SetRuntimeFixture(void* pFixture) { m_pRuntimeFixture = pFixture; }

	ColliderSettings GetSettings() const { return m_Settings; }

private:
	glm::vec2 m_Dimensions{};

	ColliderSettings m_Settings{};

	void* m_pRuntimeFixture{};
};

}
