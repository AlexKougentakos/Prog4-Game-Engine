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
		TextureComponent(const std::string& filename) :
			Component()
		{
			m_pTexture = dae::ResourceManager::GetInstance().LoadTexture(filename);
		}

		~TextureComponent() = default;
		TextureComponent(const TextureComponent& other) = delete;
		TextureComponent(TextureComponent&& other) = delete;
		TextureComponent& operator=(const TextureComponent& other) = delete;
		TextureComponent& operator=(TextureComponent&& other) = delete;

		void SetTexture(std::shared_ptr<dae::Texture2D> pTexture)
		{
			m_pTexture = pTexture;
		}

		void SetTexture(const std::string& filename)
		{
			m_pTexture = dae::ResourceManager::GetInstance().LoadTexture(filename);
		}

		virtual void Update() override
		{

		}
		virtual void Render() const override
		{
			const auto pTransformComponent{m_Owner->GetComponent<TransformComponent>()};
			glm::vec3 renderPosition{ pTransformComponent->GetWorldPosition() };

			dae::Renderer::GetInstance().RenderTexture(*m_pTexture, renderPosition.x, renderPosition.y);
		}

	private:
		std::shared_ptr<dae::Texture2D> m_pTexture{};

	};
}