#pragma once    

#include "Components/PlayerComponent.h"

class Button;
class ButtonManagerComponent;


class HumanPlayer final : public PlayerComponent
{
public:
	HumanPlayer(const int playerID, const CardRenderPackage& renderPackage);

    virtual ~HumanPlayer() override = default;

    HumanPlayer(const HumanPlayer& other) = delete;
    HumanPlayer(HumanPlayer&& other) = delete;
    HumanPlayer& operator=(const HumanPlayer& other) = delete;
    HumanPlayer& operator=(HumanPlayer&& other) = delete;

	void Initialize() override;
	void Update() override;


	void SelectCardAtMousePosition(const glm::vec2& mousePosition);

	void AskForDragon() override;
private:
	void CalculateHitBoxes();
	void CreateActionButtons();

	void GiveDragonToPlayer(const int playerID);
	CardHitbox CalculateRotatedHitbox(float centerX, float centerY, float width, float height, float rotation, bool manualCorrection);

	ButtonManagerComponent* m_pButtonManager{};
	Button* m_pPlayButton{};
	Button* m_pPassButton{};
	Button* m_pTichuButton{};
	Button* m_pGrandTichuButton{};
	Button* m_pDealCardsButton{};
	Button* m_pConfirmTradesButton{};
	Button* m_pGiveDragonButtonLeft{};
	Button* m_pGiveDragonButtonRight{};
};