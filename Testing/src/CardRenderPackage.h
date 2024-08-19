#pragma once
#include <memory>
#include <vector>


namespace ody
{
	class Texture2D;
}


struct CardRenderPackage
{
	std::vector<std::shared_ptr<ody::Texture2D>> cardTextures{};
	std::shared_ptr<ody::Texture2D> cardBack{};
	float cardSpacing{};
	float cardScale{};
	float pointDisplayHeight{75.f};
};