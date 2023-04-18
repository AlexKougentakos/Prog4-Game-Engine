#pragma once

#include "Component.h"
namespace ody
{
	class HealthComponent final : public ody::Component
	{
	public:
		HealthComponent() = default;
		HealthComponent(float health)
		{
			m_Health = health;
		}

		bool IsAlive() const { return m_IsAlive; }
		float GetHealth() const { return m_Health; }

		void AddHealth(float healthToAdd)
		{
			if (healthToAdd < 0) return;
			m_Health += healthToAdd;
		}

		void Damage(float damage)
		{
			if (damage > 0) return;
			m_Health -= damage;

			if (m_Health < 0) 
				m_IsAlive = false;
		}

		~HealthComponent() override = default;
		HealthComponent(const HealthComponent& other) = delete;
		HealthComponent(HealthComponent&& other) = delete;
		HealthComponent& operator=(const HealthComponent& other) = delete;
		HealthComponent& operator=(HealthComponent&& other) = delete;

	private:
		float m_Health{};
		bool m_IsAlive{true};
	};
}
