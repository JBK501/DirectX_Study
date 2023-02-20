#pragma once

#include "D2DFramework.h"

class Actor
{
protected:
	D2DFramework* mpFramework;
	ID2D1Bitmap* mpBitmap;
	
	// 회전에 대응하기위해 소수점 형태의 좌표를 사용한다.
	float mX;
	float mY;
	float mOpacity;

public:
	// D2DFramework가 반드시 필요하다.(WICFactory와 RenderTarget을 가져다 써야함.)
	// 따라서 기본 생성자를 없앤다.
	Actor() = delete;	
	Actor(D2DFramework* pFramework, std::wstring filename);
	Actor(D2DFramework* pFramework, std::wstring filename,
		float x, float y, float opacity=1.0f);
	virtual ~Actor();	// 자식이 있을 수 있으므로 virtual사용

protected:
	void Draw(float x, float y, float opacity = 1.0f);

public:
	virtual void Draw();
};

