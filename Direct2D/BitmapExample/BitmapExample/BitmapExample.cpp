#include "BitmapExample.h"
#include <cmath>

void BitmapExample::Initialize(HINSTANCE hInstance, LPCWSTR title, UINT width, UINT height)
{
	// 1. ���� �����ӿ�ũ�� �ʱ�ȭ�Ѵ�.
	D2DFramework::Initialize(hInstance, title, width, height);

	// 2. ����۸� �����.
	mspBackBuffer = std::make_unique<UINT8[]>(
		BITMAP_WIDTH * BITMAP_HEIGHT * BITMAP_BYTECOUNT
	);

	// 3. ��Ʈ�� �������̽��� �����.
	HRESULT hr = mspRenderTarget->CreateBitmap(
		D2D1::SizeU(BITMAP_WIDTH, BITMAP_HEIGHT),
		D2D1::BitmapProperties(
			D2D1::PixelFormat(
				DXGI_FORMAT_R8G8B8A8_UNORM,
				D2D1_ALPHA_MODE_IGNORE
			)
		),
		mspFramebitmap.GetAddressOf()
	);

	ThrowIfFailed(hr);
}

void BitmapExample::Render()
{
	mspRenderTarget->BeginDraw();
	mspRenderTarget->Clear(D2D1::ColorF(0.0f, 0.2f, 0.4f, 1.0f));

	ClearBuffer(D2D1::ColorF(D2D1::ColorF::LightPink));
	//FillRectToBuffer(0, 0, 100, 100, D2D1::ColorF::Red);
	//FillRectToBuffer(50, 50, 100, 100, D2D1::ColorF(D2D1::ColorF::Green,0.5f));
	//DrawCircleToBuffer(200, 200, 50, D2D1::ColorF::DeepSkyBlue);
	DrawLineToBuffer(100, 200, 200, 400, D2D1::ColorF::Red);
	PresentBuffer();

	mspRenderTarget->DrawBitmap(mspFramebitmap.Get());

	mspRenderTarget->EndDraw();
}

void BitmapExample::ClearBuffer(D2D1::ColorF color)
{
	// 1. ����۸� �����´�.
	UINT8* pCurrent = mspBackBuffer.get();

	// 2. �ʺ�ŭ �ݺ��Ѵ�.
	for (int count = 0; count < BITMAP_WIDTH * BITMAP_HEIGHT; ++count)
	{
		// 2.1 �ȼ��� ������ ä���.
		*pCurrent = static_cast<UINT8>(color.r * 255);
		*(pCurrent + 1) = static_cast<UINT8>(color.g * 255);
		*(pCurrent + 2) = static_cast<UINT8>(color.b * 255);
		*(pCurrent + 3) = static_cast<UINT8>(color.a * 255);

		// 2.2 �ȼ�ũŰ��ŭ �̵��Ѵ�.
		pCurrent += BITMAP_BYTECOUNT;
	}
}

void BitmapExample::PresentBuffer()
{
	mspFramebitmap->CopyFromMemory(
		nullptr,
		mspBackBuffer.get(),
		BITMAP_WIDTH * BITMAP_BYTECOUNT
	);
}

// alpha blending (�� ������ ����)
// ���� ���� * (1 - a) + �׸����� * a
void BitmapExample::DrawPixelToBuffer(int x, int y, D2D1::ColorF color)
{
	// 1. ��ġ�� ���Ѵ�.
	int pitch = BITMAP_WIDTH * BITMAP_BYTECOUNT;	

	// 2. �ε����� ���Ѵ�.
	int index = x * BITMAP_BYTECOUNT + pitch * y;

	// 3. ������� �ش� �ε����� ���� ��´�.
	float inverse = 1.0f - color.a;

	mspBackBuffer[index] = static_cast<UINT8>(
		mspBackBuffer[index] * inverse + color.r * 255 * color.a
		);
	mspBackBuffer[index + 1] = static_cast<UINT8>(
		mspBackBuffer[index + 1] * inverse + color.g * 255 * color.a
		);
	mspBackBuffer[index + 2] = static_cast<UINT8>(
		mspBackBuffer[index + 2] * inverse + color.b * 255 * color.a
		);
	mspBackBuffer[index + 3] = static_cast<UINT8>(
		color.a * 255
		);
}

void BitmapExample::FillRectToBuffer(int left, int top, int width, int height, D2D1::ColorF color)
{
	// 1. ���� ��ŭ �ݺ��Ѵ�.
	for (int h = 0; h < height; h++)
	{
		// 1.1. �ʺ� ��ŭ �ݺ��Ѵ�.
		for (int w = 0; w < width; w++)
		{
			// 1.1.1. ���ۿ� �����Ѵ�.
			DrawPixelToBuffer(left + w, top + h, color);
		}
	}
}

void BitmapExample::DrawCircleToBuffer(int x, int y, int radius, D2D1::ColorF color)
{
	const float pi{ 3.14f };
	const float piMultiplyTwo{ pi * 2 };
	float theta{};

	// 1. ��Ÿ�� 2���̰� �ɶ����� �ݺ��Ѵ�.
	while(theta < piMultiplyTwo)
	{
		// 1.1. ���� ��´�.
		DrawPixelToBuffer(
			static_cast<int>(x + cos(theta) * radius),
			static_cast<int>(y + sin(theta) * radius),
			color
		);
		// 1.2. ������ �ø���.
		theta += 0.02;
	}
}

void BitmapExample::DrawLineToBuffer(int x1, int y1, int x2, int y2, D2D1::ColorF color)
{
	// 1. ��ȭ���� ���Ѵ�.
	int dx{ x2 - x1 };
	int dy{ y2 - y1 };

	int x;
	int y;
	int op;
	// 2. x��ȭ���� 0�̸�
	if (dx == 0)
	{
		x = x1;	// x����
		y = y1;
		op = (y2 > y1) ?  1 : -1;
		while (y != y2)
		{
			DrawPixelToBuffer(x, y, color);
			y += op;
		}
		return;
	}

	// 3. y��ȭ���� 0�̸�
	if (dy == 0)
	{
		x = x1;
		y = y1; // y����
		op = (x2 > x1) ? 1 : -1;
		while (x != x2)
		{
			DrawPixelToBuffer(x, y, color);
			x += op;
		}
		return;
	}

	// 4. ���⸦ ���Ѵ�.
	float m = dy / (dx * 1.0f);

	// 5. ������ ���Ѵ�.
	op = (x1 < x2) ? 1 : -1;
	x = x1;
	while (x != x2)
	{
		y = static_cast<int>(m * (x - x1) + y1);
		DrawPixelToBuffer(x, y, color);
		x += op;
	}
}
