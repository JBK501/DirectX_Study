#pragma once

#include "D2DFramework.h"

class Actor
{
protected:
	D2DFramework* mpFramework;
	ID2D1Bitmap* mpBitmap;
	
	// 화면에 그릴 위치와 투명도
	// 파생클래스가 사용할 수 있도록 protected로 지정함.
	// 회전에 대응하기위해 소수점 형태의 좌표를 사용한다.
	float mX;
	float mY;
	float mOpacity;

public:
	// Actor클래스는 D2DFramework정보가 없으면 아무런 처리도 할 수 없다. 
	// WICFactory와 RenderTarget을 가져다 써야함.
	// 따라서 기본 생성자를 없앤다.
	Actor() = delete;	
	Actor(D2DFramework* pFramework, std::wstring filename);
	Actor(D2DFramework* pFramework, std::wstring filename,
		float x, float y, float opacity=1.0f);
	virtual ~Actor();	// 파생클래스가 있을 수 있으므로 virtual사용

protected:
	void Draw(float x, float y, float opacity = 1.0f);

public:
	virtual void Draw();
};

