#pragma once    

#include "Components/PlayerComponent.h"
#include <future>

class Button;
class ButtonManagerComponent;

class AIPlayer_MCTS final : public PlayerComponent
{
public:
	AIPlayer_MCTS(const int playerID, const CardRenderPackage& renderPackage, const int iterations = 1);

    virtual ~AIPlayer_MCTS() override = default;

    AIPlayer_MCTS(const AIPlayer_MCTS& other) = delete;
    AIPlayer_MCTS(AIPlayer_MCTS&& other) = delete;
    AIPlayer_MCTS& operator=(const AIPlayer_MCTS& other) = delete;
    AIPlayer_MCTS& operator=(AIPlayer_MCTS&& other) = delete;

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

};