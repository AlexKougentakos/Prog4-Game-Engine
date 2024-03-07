#pragma once
#include "Command.h"

class PlaySound : public ody::Command
{
public:
	PlaySound() = default;

	virtual ~PlaySound() override = default;
	PlaySound(const PlaySound& other) = delete;
	PlaySound(PlaySound&& other) = delete;
	PlaySound& operator=(const PlaySound& other) = delete;
	PlaySound& operator=(PlaySound&& other) = delete;

	virtual void Execute() override;
};