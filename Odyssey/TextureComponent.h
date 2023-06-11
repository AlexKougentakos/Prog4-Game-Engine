#pragma once
#include "Component.h"
#include "Renderer.h"
#include "TransformComponent.h"
#include "glm/glm.hpp"
#include "Texture2D.h"
#include "ResourceManager.h"
#include "GameObject.h"
#include <memory>
#include <string>

namespace ody
{
	class TextureComponent final : public Component
	{
	public:
		TextureComponent() = default;
		TextureComponent(const std::string& filename, float scale = 1.f) :
			Component(), m_Scale(scale)
		{
			m_pTexture = ody::ResourceManager::GetInstance().LoadTexture(filename);

		}
		TextureComponent(const std::string& filename, const glm::vec2& destinationRect) :
			Component()
		{
			m_UseDestinationRect = true;
			m_pTexture = ody::ResourceManager::GetInstance().LoadTexture(filename);
			m_DestinationRect = destinationRect;
		}

		~TextureComponent() override = default;
		TextureComponent(const TextureComponent& other) = delete;
		TextureComponent(TextureComponent&& other) = delete;
		TextureComponent& operator=(const TextureComponent& other) = delete;
		TextureComponent& operator=(TextureComponent&& other) = delete;

		glm::vec2 GetTextureSize() const
		{
			return m_pTexture->GetSize();
		}

		void SetTexture(std::shared_ptr<ody::Texture2D> pTexture)
		{
			m_pTexture = pTexture;
		}

		void SetTexture(const std::string& filename)
		{
			m_pTexture = ody::ResourceManager::GetInstance().LoadTexture(filename);
		}

		virtual void Render() const override
		{
			const auto pTransformComponent{ GetOwner()->GetComponent<TransformComponent>()};
			const glm::vec3 renderPosition{ pTransformComponent->GetWorldPosition() };
			if (!m_UseDestinationRect)
				ody::Renderer::GetInstance().RenderTexture(*m_pTexture, renderPosition.x, renderPosition.y, m_Scale);
			else
				ody::Renderer::GetInstance().RenderTexture(*m_pTexture, renderPosition.x, renderPosition.y, m_DestinationRect.x, m_DestinationRect.y, m_Scale);
		}

	private:
		std::shared_ptr<ody::Texture2D> m_pTexture{};
		glm::vec2 m_DestinationRect{};
		bool m_UseDestinationRect{ false };

		const float m_Scale{ 1.f };

	};
}