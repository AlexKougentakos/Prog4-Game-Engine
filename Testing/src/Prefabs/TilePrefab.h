#pragma once
#include "IPrefab.h"

class TilePrefab final : public ody::IPrefab
{
public:

	TilePrefab() = default;
	virtual ~TilePrefab() override = default;
	TilePrefab(const TilePrefab& other) = delete;
	TilePrefab(TilePrefab&& other) = delete;
	TilePrefab& operator=(const TilePrefab& other) = delete;
	TilePrefab& operator=(TilePrefab&& other) = delete;

	virtual void Configure(ody::GameObject* gameObject) const override;
};

