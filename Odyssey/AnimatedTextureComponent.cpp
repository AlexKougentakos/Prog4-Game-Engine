#include "AnimatedTextureComponent.h"
#include "GameTime.h"

namespace ody
{
	void AnimatedTextureComponent::Update()
	{
		{
			m_AnimationTime += Time::GetInstance().GetDeltaTime();

			if (m_AnimationTime > m_AnimationSpeed)
			{
				m_CurrentFrame.x++;
				if (m_CurrentFrame.x >= m_SpriteCounts.x)
				{
					m_CurrentFrame.x = 0;
					m_CurrentFrame.y++;
					if (m_CurrentFrame.y >= m_SpriteCounts.y)
					{
						m_CurrentFrame.y = 0;
					}
				}
				m_AnimationTime = 0.0f;
			}
		}
	}

}