#pragma once
#include <array>

namespace MyUtil
{
	class Input
	{
	private:
		std::array<bool, 256> mKeys;	// 키 입력 상태 배열
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


