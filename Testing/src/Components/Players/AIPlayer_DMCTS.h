#pragma once    

#include "Components/PlayerComponent.h"
#include <future>

class Button;
class ButtonManagerComponent;

class AIPlayer_DMCTS final : public PlayerComponent
{
public:
	AIPlayer_DMCTS(const int playerID, const CardRenderPackage& renderPackage, const int iterations = 1, const int determinizations = 1);

	virtual ~AIPlayer_DMCTS() override = default;

	AIPlayer_DMCTS(const AIPlayer_DMCTS& other) = delete;
	AIPlayer_DMCTS(AIPlayer_DMCTS&& other) = delete;
	AIPlayer_DMCTS& operator=(const AIPlayer_DMCTS& other) = delete;
	AIPlayer_DMCTS& operator=(AIPlayer_DMCTS&& other) = delete;

	void Initialize() override;
	void Update() override;
	void OnGuiMCTS() override;

	virtual void AskForDragon() override;

	bool IsAIPlayer() const override { return true; }
	float GetDesiredGuiHeight() const override { return 200.f; }  // Taller for AI debug info

private:
	void StartMoveCalculation();
	void ExecuteMove(const std::vector<Card>& cardsToPlay);

	std::future<std::vector<Card>> m_MoveFuture;
	bool m_IsCalculatingMove{ false };

	const int m_Iterations{};
	const int m_Determinizations{};

};