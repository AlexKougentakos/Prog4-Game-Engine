#pragma once
#include <functional>
#include <string>
#include <vector>
#include <glm/vec2.hpp>

#include "Component.h"
#include "Texture2D.h"

class ConstructedButton final
{
public:
	~ConstructedButton() = default;
	ConstructedButton(const ConstructedButton& other) = delete;
	ConstructedButton(ConstructedButton&& other) = delete;
	ConstructedButton& operator=(const ConstructedButton& other) = delete;
	ConstructedButton& operator=(ConstructedButton&& other) = delete;
private:
	friend class ButtonManagerComponent;
	ConstructedButton();
};

class ButtonManagerComponent final : public ody::Component
{
private:

	struct Button
	{
		std::shared_ptr<ody::Texture2D> texture{};
		std::function<void()> callback{};
		glm::vec2 screenPosition{};
		glm::vec2 dimensions{};

		glm::vec4 hoveredTint{};
		glm::vec4 pressedTint{};
	};

public:
	ButtonManagerComponent() = default;

	virtual ~ButtonManagerComponent() override = default;
	ButtonManagerComponent(const ButtonManagerComponent& other) = delete;
	ButtonManagerComponent(ButtonManagerComponent&& other) = delete;
	ButtonManagerComponent& operator=(const ButtonManagerComponent& other) = delete;
	ButtonManagerComponent& operator=(ButtonManagerComponent&& other) = delete;

	virtual void Render() const override;
	virtual void Update() override;


	void AddButton(const std::string& imagePath, std::function<void()> callback, glm::vec2 screenPosition);

private:
	friend class ButtonPressed;
	void OnMouseClick(const glm::vec2& mousePos);
	bool IsPointInsideButton(const glm::vec2& point, const Button& pButton) const;

	std::vector<std::unique_ptr<Button>> m_pButtons{};
};
