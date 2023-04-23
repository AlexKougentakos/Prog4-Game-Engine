#pragma once

#include "Observer.h"
#include "GameObject.h"
#include "HealthComponent.h"

class PointExercise final : public ody::IObserver	
{
public:
	virtual void OnNotify(const ody::GameObject& gameObject, ody::GameEvent /*event*/) override
	{
		gameObject.GetComponent<ody::HealthComponent>()->Damage(10.f);
	}
};
