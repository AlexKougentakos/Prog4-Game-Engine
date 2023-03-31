#include "GameObject.h"
#include "RotatorComponent.h"
#include "TransformComponent.h"
#include "Time.h"

void ody::RotatorComponent::Update()
{
	const auto pTransform{ m_Owner->GetComponent<TransformComponent>()};

	if (!pTransform) return;

	m_CurAngle += m_RotateSpeed * Time::GetInstance().GetDeltaTime() * static_cast<float>(3.14f) / 180.0f;

	const float x{ cosf(m_CurAngle) * m_DistanceFromPivot };
	const float y{ sinf(m_CurAngle) * m_DistanceFromPivot };

	pTransform->SetPosition(x, y);
}
void ody::RotatorComponent::Render() const
{

}