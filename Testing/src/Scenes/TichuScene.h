#pragma once
#include "GameScene.h"

#include <iostream>

#include "Tichu.h"
#include "CardRenderPackage.h"

namespace ody
{
	class TextComponent;
}

class Button;
class ButtonManagerComponent;
class CardComponent;

class TichuScene : public ody::GameScene
{
public:
	TichuScene() : ody::GameScene(L"Tichu Scene") { Initialize(); }

	TichuScene(const TichuScene& other) = delete;
	TichuScene(TichuScene&& other) noexcept = delete;
	TichuScene& operator=(const TichuScene& other) = delete;
	TichuScene& operator=(TichuScene&& other) noexcept = delete;

protected:
	void CreateMahjongSelectionTable();
	void Initialize() override;
	void PostRender() override;
	void Update() override;
	void OnGUI() override;

	virtual void OnSceneActivated() override
	{
		std::cout << "Scene Activated" << std::endl;
	}
	virtual void OnSceneDeactivated() override
	{
		std::cout << "Scene Deactivated" << std::endl;
	}
private:
	void CreateDeck();
	void CreatePlayers();
	void DealCards();
	void CreateCardRenderPackage();

	void CheckSubmittedHand();
	void Pass();
	
	void UpdateLights() const;

	std::vector<PlayerComponent*> m_pPlayers{};
	std::vector<Card> m_Cards{};
	std::vector<Card> m_CurrentCards{};
	CardRenderPackage m_RenderPackage{};
	std::unique_ptr<Tichu> m_pTichuGame{};

	std::vector<ody::TextComponent*> m_pTextComponents{};
	std::vector<Button*> m_pMahjongButtons{};

	void CreateButtonTextAtPosition(const std::string& text, const glm::vec2& position, const glm::vec2& buttonSize);

	bool m_ShouldShowMahjongSelectionTable{ false };
	bool m_IsMahjongSelectionTableVisible{ false };

	ButtonManagerComponent* m_pButtonManager{};
	Button* m_pPassButton{};

	//ImGui
	bool m_ShowCardHitboxes{false};
	bool m_ShowCardBacks{true};
};
