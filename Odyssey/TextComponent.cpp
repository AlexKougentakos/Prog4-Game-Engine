#include "TextComponent.h"

#include "Renderer.h"
#include "GameObject.h"
#include "ResourceManager.h"

ody::TextComponent::TextComponent(const std::string& text, const std::string& fontPath, unsigned size) :
	Component()
{
	m_Font = ody::ResourceManager::GetInstance().LoadFont(fontPath, size);
	m_Text = text;

	UpdateText();
}

ody::TextComponent::TextComponent(const std::string& text, std::shared_ptr<ody::Font> font) :
	Component()
{
	m_Font = font;
	m_Text = text;

	UpdateText();
}

void ody::TextComponent::Update() 
{
	if (m_NeedsUpdate)
	{
		UpdateText();
	}
}

void ody::TextComponent::Render() const 
{
	if (m_TextTexture != nullptr)
	{
		const auto pTransformComponent{ GetOwner()->GetComponent<TransformComponent>() };
		glm::vec3 renderPosition{ pTransformComponent->GetWorldPosition() };
		renderPosition += glm::vec3(m_Position, 0);

		ody::Renderer::GetInstance().RenderTexture(*m_TextTexture, renderPosition.x, renderPosition.y);
	}
}

void ody::TextComponent::UpdateText()
{
	constexpr SDL_Color color = { 255,255,255, 255 }; // only white text is supported now
	const auto surf = TTF_RenderText_Blended(m_Font->GetFont(), m_Text.c_str(), color);
	if (surf == nullptr)
	{
		throw std::runtime_error(std::string("Render text failed: ") + SDL_GetError());
	}
	auto texture = SDL_CreateTextureFromSurface(ody::Renderer::GetInstance().GetSDLRenderer(), surf);
	if (texture == nullptr)
	{
		throw std::runtime_error(std::string("Create text texture from surface failed: ") + SDL_GetError());
	}
	SDL_FreeSurface(surf);
	m_TextTexture = std::make_shared<ody::Texture2D>(texture);
	m_NeedsUpdate = false;
}
