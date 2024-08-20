#pragma once
#include <string>
#include <memory>
#include <stdexcept>
#include <SDL_ttf.h>

#include "Component.h"
#include "TransformComponent.h"
#include "Font.h"
#include "Texture2D.h"
#include "glm/glm.hpp"


namespace ody
{
	class TextComponent final : public Component
	{
	public:
		TextComponent(const std::string& text, const std::string& fontPath, unsigned int size);

		TextComponent(const std::string& text, std::shared_ptr<ody::Font> font);

		virtual ~TextComponent() override = default;
		TextComponent(const TextComponent& other) = delete;
		TextComponent(TextComponent&& other) = delete;
		TextComponent& operator=(const TextComponent& other) = delete;
		TextComponent& operator=(TextComponent&& other) = delete;

		void SetPosition(glm::vec2 position)
		{
			m_Position = position;
		}

		void SetPosition(float x, float y)
		{
			m_Position.x = x;
			m_Position.y = y;

		}

		void SetText(const std::string& text)
		{
			m_Text = text;
			m_NeedsUpdate = true;
		}

		glm::vec2 GetTextSize() const
		{
			if (m_TextTexture == nullptr)
			{
				throw std::runtime_error("Text texture is nullptr");
			}
			return m_TextTexture->GetSize();
		}

		void SetVisible(const bool isVisible) { m_IsVisible = isVisible; }

		virtual void Update() override;
		virtual void Render() const override;

	private:
		glm::vec2 m_Position{};
		std::string m_Text{};

		void UpdateText();

		bool m_IsVisible{ true };

		std::shared_ptr<ody::Font> m_Font;
		std::shared_ptr<ody::Texture2D> m_TextTexture;

		bool m_NeedsUpdate{true};

	};
}
