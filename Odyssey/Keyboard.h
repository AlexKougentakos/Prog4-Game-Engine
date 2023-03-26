#pragma once
#include <memory>

namespace ody
{
	class Keyboard final
	{
	public:
		enum class KeyboardKey
		{
			Key_A = 1,
			Key_B = 2,
			Key_C = 3,
			Key_D = 4,
			Key_E = 5,
			Key_F = 6,
			Key_G = 7,
			Key_H = 8,
			Key_I = 9,
			Key_J = 10,
			Key_K = 11,
			Key_L = 12,
			Key_M = 13,
			Key_N = 14,
			Key_O = 15,
			Key_P = 16,
			Key_Q = 17,
			Key_R = 18,
			Key_S = 19,
			Key_T = 20,
			Key_U = 21,
			Key_V = 22,
			Key_W = 23,
			Key_X = 24,
			Key_Y = 25,
			Key_Z = 26,
		};

		void Update();

		bool IsDown(KeyboardKey key) const;
		bool IsUp(KeyboardKey key) const;
		bool IsPressed(KeyboardKey key) const;

		explicit Keyboard(int keyboardIndex);
		~Keyboard() = default;
		Keyboard(const Keyboard& other) = delete;
		Keyboard(Keyboard&& other) = delete;
		Keyboard& operator=(const Keyboard& other) = delete;
		Keyboard& operator=(Keyboard&& other) = delete;
	private:

		class KeyboardImpl;
		std::unique_ptr<KeyboardImpl> m_pImpl{};
	};
}
