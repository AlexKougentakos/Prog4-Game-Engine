#pragma once    

#include "Components/PlayerComponent.h"

class Button;
class ButtonManagerComponent;


class AIPlayer final : public PlayerComponent
{
public:
	AIPlayer(const int playerID, const CardRenderPackage& renderPackage);

    virtual ~AIPlayer() override = default;

    AIPlayer(const AIPlayer& other) = delete;
    AIPlayer(AIPlayer&& other) = delete;
    AIPlayer& operator=(const AIPlayer& other) = delete;
    AIPlayer& operator=(AIPlayer&& other) = delete;

	void Initialize() override;
	void Update() override;

	virtual void AskForDragon() override;
private:
    void MakeRandomMove();
};