#include "Actor.h"

Actor::Actor(D2DFramework* pFramework, std::wstring filename) :
	Actor(pFramework, filename, 0.0f, 0.0f, 1.0f)	// 대리 생성자사용
{
}

Actor::Actor(D2DFramework* pFramework, std::wstring filename, float x, float y, float opacity) :
	mpFramework{pFramework},
	mX{x},mY{y},
	mOpacity{opacity}
{
	mpBitmap = BitmapManger::Instance().LoadBitmap(filename);
}

Actor::~Actor()
{
}

void Actor::Draw(float x, float y, float opacity)
{
	auto pRenderTarget = mpFramework->GetRenderTarget();
	auto size = mpBitmap->GetPixelSize();
	D2D1_RECT_F rect{
		x,y,
		x + size.width,y + size.height
	};

	pRenderTarget->DrawBitmap(mpBitmap, rect, opacity);
}

void Actor::Draw()
{
	Draw(mX, mY, mOpacity);
}

