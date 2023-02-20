#include "BuggyHouse.h"

void BuggyHouse::Initialize(HINSTANCE hInstance, LPCWSTR title, UINT width, UINT height)
{
	D2DFramework::Initialize(hInstance, title, width, height);

	mspBackground = std::make_unique<Actor>(this, L"Data/back1_1024.png");
	

	for (int i = 0; i < 100; i++)
	{
		mBugs.push_back(std::make_unique<Bug>(this));
	}
}

void BuggyHouse::Release()
{

	mBugs.clear();
	mspBackground.reset();

	D2DFramework::Release();
}

void BuggyHouse::Render()
{
	HRESULT hr;
	mspRenderTarget->BeginDraw();
	mspRenderTarget->Clear(D2D1::ColorF(0.0f, 0.2f, 0.4f));

	// 이전에 벌레클래스의 Draw에서 회전하고 이동한 정보를 초기화한다.
	// (배경까지 움직이지 않게하기 위해서)
	mspRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
	mspBackground->Draw();

	CheckBugs();	// 클릭된 벌레를 없앤다.

	for (auto& bug : mBugs)
	{
		bug->Draw();
	}

	hr = mspRenderTarget->EndDraw();

	if (hr == D2DERR_RECREATE_TARGET)
	{
		CreateDeviceResources();
	}
}

void BuggyHouse::CheckBugs()
{
	// 1. 마우스 왼쪽버튼이 클릭되었다면
	if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
	{
		// 1.1. 현재 마우스의 위치를 클라이언트 좌표계로 구해온다.
		POINT pt;
		GetCursorPos(&pt);
		ScreenToClient(mHwnd, &pt);

		// 1.2. 벌레의 상태를 확인한 후 없앤다.
		auto it = std::remove_if(mBugs.begin(), mBugs.end(),
			[&](const std::unique_ptr<Bug> &bug) -> bool
			{
				Bug* pBug = bug.get();
				pBug->IsClicked(pt);

				if (pBug->mIsDeleted)
				{
					return true;
				}
				else
				{
					return false;
				}
			}
		);

		mBugs.erase(it, mBugs.end());
	}
}
