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

		//starting at one to match the default value of Box2D
		enum class CollisionGroups : unsigned short
		{
			None = 0x0001, 
			Group1 = 0x0002,
			Group2 = 0x0003,
			Group3 = 0x0004,
			Group4 = 0x0005,
			Group5 = 0x0006,
			Group6 = 0x0007,
			Group7 = 0x0008,
			Group8 = 0x0009,
			Group9 = 0x00010,
			Group10 = 0x0011,
			Group11 = 0x0012,
			Group12 = 0x0013,
			Group13 = 0x0014,
			Group14 = 0x0015,
			Group15 = 0x0016,
			Group16 = 0x0017,
			Group17 = 0x0018,
			Group18 = 0x0019,
			Group19 = 0x0020,
			Group20 = 0x0021
		};
	}
}