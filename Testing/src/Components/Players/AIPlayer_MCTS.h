#pragma once    

#include "PlayerComponent.h"

class Button;
class ButtonManagerComponent;


class AIPlayer_MCTS final : public PlayerComponent
{
public:
	AIPlayer_MCTS(const int playerID, const CardRenderPackage& renderPackage);

    virtual ~AIPlayer_MCTS() override = default;

    AIPlayer_MCTS(const AIPlayer_MCTS& other) = delete;
    AIPlayer_MCTS(AIPlayer_MCTS&& other) = delete;
    AIPlayer_MCTS& operator=(const AIPlayer_MCTS& other) = delete;
    AIPlayer_MCTS& operator=(AIPlayer_MCTS&& other) = delete;

	void Initialize() override;
	void Update() override;

	virtual void AskForDragon() override;
private:
    void MakeMove();
};