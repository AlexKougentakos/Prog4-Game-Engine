#include "ButtonManagerComponent.h"

#include "InputManager2.h"
#include "Renderer.h"
#include "ResourceManager.h"

void ButtonManagerComponent::Render() const 
{ 
	for (const auto& button : m_pButtons)
	{
        ody::Renderer::GetInstance().RenderTexture(
            *button->texture,
            button->screenPosition.x,
            button->screenPosition.y
        );
	}
}

void ButtonManagerComponent::Update() 
{
	const auto& inputManager = ody::InputManager::GetInstance();

    //Only check this if the mouse has moved
	if (inputManager.GetMouseMotion().x == 0 || inputManager.GetMouseMotion().y == 0) return;

    const glm::vec2 mousePos = inputManager.GetMousePosition();
    for (const auto& button : m_pButtons)
    {
        if (IsPointInsideButton(mousePos, *button))
        {
            button->texture->Tint({ 0.5f, 0.5f, 0.5f, 1.f });
        }
        else 
        {
            button->texture->Tint({ 1.f, 1.f, 1.f, 1.f });
        }
    }
}

void ButtonManagerComponent::OnMouseClick(const glm::vec2& mousePos)
{
    for (const auto& button : m_pButtons)
    {
        if (IsPointInsideButton(mousePos, *button))
        {
            button->callback();
            return;
        }
    }
    
}

void ButtonManagerComponent::AddButton(const std::string& imagePath, std::function<void()> callback, glm::vec2 screenPosition)
{
	std::shared_ptr<ody::Texture2D> texture = ody::ResourceManager::GetInstance().LoadTexture(imagePath);

	m_pButtons.emplace_back(std::make_unique<Button>(texture, std::move(callback), screenPosition, texture->GetSize()));
}

bool ButtonManagerComponent::IsPointInsideButton(const glm::vec2& point, const Button& pButton) const
{
    const glm::vec2 buttonPos = pButton.screenPosition;
    const glm::vec2 buttonDimensions = pButton.dimensions;

    return (point.x >= buttonPos.x && point.x < buttonPos.x + buttonDimensions.x &&
        point.y >= buttonPos.y && point.y < buttonPos.y + buttonDimensions.y);
}
