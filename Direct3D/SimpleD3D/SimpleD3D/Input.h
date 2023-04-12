#pragma once
#include <array>

namespace MyUtil
{
	class Input
	{
	private:
		std::array<bool, 256> mKeys;	// Ű �Է� ���� �迭
		int mCursorX;
		int mCursorY;

	public:
		void Initialize();

		void SetKeyDown(unsigned int key);
		void SetKeyUp(unsigned int key);
		void SetCursor(int x, int y);

		bool IsKeyDown(unsigned int key);
		void GetCursorPosition(int& x, int& y);
	};
}

