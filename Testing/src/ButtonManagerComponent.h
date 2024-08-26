#pragma once
#include <functional>
#include <map>
#include <string>
#include <vector>
#include <glm/vec2.hpp>

#include "Component.h"
#include "Texture2D.h"

class Button
{
public:
	void SetEnabled(const bool isEnabled) { m_IsEnabled = isEnabled; }
	void SetVisible(const bool isVisible) { m_IsVisible = isVisible; }
	void SetTexture(const std::shared_ptr<ody::Texture2D>& _texture) { texture = _texture; }

private:
	friend class ButtonManagerComponent;
	Button(std::shared_ptr<ody::Texture2D> texture, std::function<void()>&& callback,
		const glm::vec2& screenPosition, const glm::vec2& dimensions) :
		texture(texture), callback(std::move(callback)),
		screenPosition(screenPosition), dimensions(dimensions)
	{}

	std::shared_ptr<ody::Texture2D> texture{};
	std::function<void()> callback{};
	glm::vec2 screenPosition{};
	glm::vec2 dimensions{};

	glm::vec4 hoveredTint{0.5f, 0.5f, 0.5f, 1.f};
	glm::vec4 pressedTint{0.3f, 0.3f, 0.3f, 1.f};
	glm::vec4 disabledTint{0.4f, 0.4f, 0.4f, 1.f};

	//Interfacing
	bool m_IsEnabled{ true };
	bool m_IsVisible{ true };
};

class ButtonManagerComponent final : public ody::Component
{
public:
	ButtonManagerComponent() = default;

	virtual ~ButtonManagerComponent() override = default;
	ButtonManagerComponent(const ButtonManagerComponent& other) = delete;
	ButtonManagerComponent(ButtonManagerComponent&& other) = delete;
	ButtonManagerComponent& operator=(const ButtonManagerComponent& other) = delete;
	ButtonManagerComponent& operator=(ButtonManagerComponent&& other) = delete;

	virtual void Render() const override;
	virtual void Update() override;

	//The source rect gets defaulted to the size of the texture
	Button* AddButton(const std::string& imagePath, std::function<void()> callback, const glm::vec2& screenPosition, const glm::vec2& sourceRect = {0,0});
	Button* AddButton(const std::shared_ptr<ody::Texture2D>& texture, std::function<void()> callback, const glm::vec2& screenPosition, const glm::vec2& sourceRect = {0,0});

private:
	friend class ButtonPressed;
	friend class ButtonManagerComponent;
	void OnMouseClick(const glm::vec2& mousePos);
	bool IsPointInsideButton(const glm::vec2& point, const Button& pButton) const;

	std::vector<std::unique_ptr<Button>> m_pButtons{};
	std::vector<std::shared_ptr<ody::Texture2D>> m_pTextTextures{};
};
