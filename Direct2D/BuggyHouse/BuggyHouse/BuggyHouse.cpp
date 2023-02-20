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

	// ������ ����Ŭ������ Draw���� ȸ���ϰ� �̵��� ������ �ʱ�ȭ�Ѵ�.
	// (������ �������� �ʰ��ϱ� ���ؼ�)
	mspRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
	mspBackground->Draw();

	CheckBugs();	// Ŭ���� ������ ���ش�.

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
	// 1. ���콺 ���ʹ�ư�� Ŭ���Ǿ��ٸ�
	if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
	{
		// 1.1. ���� ���콺�� ��ġ�� Ŭ���̾�Ʈ ��ǥ��� ���ؿ´�.
		POINT pt;
		GetCursorPos(&pt);
		ScreenToClient(mHwnd, &pt);

		// 1.2. ������ ���¸� Ȯ���� �� ���ش�.
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
