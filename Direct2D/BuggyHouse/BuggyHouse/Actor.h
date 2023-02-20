#pragma once

#include "D2DFramework.h"

class Actor
{
protected:
	D2DFramework* mpFramework;
	ID2D1Bitmap* mpBitmap;
	
	// ȸ���� �����ϱ����� �Ҽ��� ������ ��ǥ�� ����Ѵ�.
	float mX;
	float mY;
	float mOpacity;

public:
	// D2DFramework�� �ݵ�� �ʿ��ϴ�.(WICFactory�� RenderTarget�� ������ �����.)
	// ���� �⺻ �����ڸ� ���ش�.
	Actor() = delete;	
	Actor(D2DFramework* pFramework, std::wstring filename);
	Actor(D2DFramework* pFramework, std::wstring filename,
		float x, float y, float opacity=1.0f);
	virtual ~Actor();	// �ڽ��� ���� �� �����Ƿ� virtual���

protected:
	void Draw(float x, float y, float opacity = 1.0f);

public:
	virtual void Draw();
};

