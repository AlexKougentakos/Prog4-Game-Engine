#pragma once
#include "Command.h"

class ButtonManagerComponent;

class ButtonPressed : public ody::Command
{
public:
	explicit ButtonPressed(ButtonManagerComponent* pButtonManagerComponent);

	virtual ~ButtonPressed() override = default;
	ButtonPressed(const ButtonPressed& other) = delete;
	ButtonPressed(ButtonPressed&& other) = delete;
	ButtonPressed& operator=(const ButtonPressed& other) = delete;
	ButtonPressed& operator=(ButtonPressed&& other) = delete;

	virtual void Execute() override;
private:
	ButtonManagerComponent* m_pButtonManagerComponent{};
};
