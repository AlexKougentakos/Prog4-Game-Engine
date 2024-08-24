#include "ButtonManagerComponent.h"

#include "InputManager2.h"
#include "Renderer.h"
#include "ResourceManager.h"

void ButtonManagerComponent::Render() const 
{ 
	for (const auto& button : m_pButtons)
	{
		if (!button->m_IsVisible)
			continue;

        ody::Renderer::GetInstance().RenderTexture(
            *button->texture,
            button->screenPosition.x,
            button->screenPosition.y,
			button->dimensions.x,
			button->dimensions.y
        );
	}
}

void ButtonManagerComponent::Update() 
{
	const auto& inputManager = ody::InputManager::GetInstance();

    const glm::vec2 mousePos = inputManager.GetMousePosition();
    for (const auto& button : m_pButtons)
    {
		if (!button->m_IsVisible)
			continue;

        if (!button->m_IsEnabled)
        {
            button->texture->Tint(button->disabledTint);
            continue;
        }

        if (IsPointInsideButton(mousePos, *button))
        {
            button->texture->Tint(button->hoveredTint);
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
		if (!button->m_IsEnabled)
			continue;

        if (IsPointInsideButton(mousePos, *button))
        {
            //todo: add clicked tint
            button->callback();
            return;
        }
    }
    
}

Button* ButtonManagerComponent::AddButton(const std::string& imagePath, std::function<void()> callback, const glm::vec2& screenPosition, const glm::vec2& sourceRect)
{
	const std::shared_ptr<ody::Texture2D> texture = ody::ResourceManager::GetInstance().LoadTexture(imagePath);

	glm::vec2 dimensions = sourceRect;

    if (sourceRect == glm::vec2{0.f, 0.f})
    {
        dimensions = texture->GetSize();
    }

    //Using new here instead of make_unique because the constructor of Button is private
    const auto pButton = new Button(texture, std::move(callback), screenPosition, dimensions);
	m_pButtons.emplace_back(std::unique_ptr<Button>(pButton));

    return pButton;
}

bool ButtonManagerComponent::IsPointInsideButton(const glm::vec2& point, const Button& pButton) const
{
	if (pButton.m_IsVisible == false)
		return false;

    const glm::vec2 buttonPos = pButton.screenPosition;
    const glm::vec2 buttonDimensions = pButton.dimensions;

    return (point.x >= buttonPos.x && point.x < buttonPos.x + buttonDimensions.x &&
        point.y >= buttonPos.y && point.y < buttonPos.y + buttonDimensions.y);
}
