#pragma once
namespace ody
{
	namespace constants
	{
		//Window & World(Box2d)
		constexpr int g_ScreenWidth = 640; //todo: Get a setter for this
		constexpr int g_ScreenHeight = 480;
		constexpr float g_WorldWidthInMeters = 10; // todo: Fiddle with this value

		constexpr float g_PixelsPerMeter{ g_ScreenWidth / g_WorldWidthInMeters };
	}
}