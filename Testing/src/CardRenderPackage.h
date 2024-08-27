#pragma once
#include <memory>
#include <string>
#include <vector>


namespace ody
{
	class Texture2D;
}


struct CardRenderPackage
{
	std::vector<std::shared_ptr<ody::Texture2D>> cardTextures{};
	std::vector<std::string> cardTexturePaths{}; //In case you need to create a new texture. They are in the same order as the cardTextures
	std::shared_ptr<ody::Texture2D> cardBack{};
	float cardSpacing{};
	float cardScale{};
	float pointDisplayHeight{75.f};
};