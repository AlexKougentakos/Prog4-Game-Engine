#include "Component.h"
#include "GameObject.h"

void ody::Component::SetOwner(ody::GameObject* owner)
{
	//assert(m_Owner.expired() && "Object already has an owner");

	m_Owner = owner;
}