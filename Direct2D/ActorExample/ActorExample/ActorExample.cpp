#include "ActorExample.h"

void ActorExample::Initialize(HINSTANCE hInstance, LPCWSTR title, UINT width, UINT height)
{
	D2DFramework::Initialize(hInstance, title, width, height);

	mspBackground = std::make_unique<Actor>(this, L"Data/back1.png");
	
	for (int i = 0; i < 100; i++)
	{
		mBugs.push_back(std::make_unique<Actor>(this, L"Data/bug.png", i, i % 30 * 30));
	}
}

void ActorExample::Release()
{
	mBugs.clear();
	mspBackground.reset();

	D2DFramework::Release();
}

void ActorExample::Render()
{
	HRESULT hr;
	mspRenderTarget->BeginDraw();
	mspRenderTarget->Clear(D2D1::ColorF(0.0f, 0.2f, 0.4f));

	mspBackground->Draw();

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
