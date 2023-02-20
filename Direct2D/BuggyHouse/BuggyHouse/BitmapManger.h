#pragma once
#include <d2d1.h>
#include <wincodec.h>
#include <wrl/client.h>
#include <map>
#include <string>
#include "ComException.h"

class BitmapManger
{
public:
	static BitmapManger& Instance()
	{
		static BitmapManger instance;

		return instance;
	}

private:
	BitmapManger() = default;	// default : 아무것도 안한다.
	BitmapManger(const BitmapManger&) = delete;
	void operator=(const BitmapManger&) = delete;

private:
	ID2D1HwndRenderTarget* mpRenderTarget{};
	Microsoft::WRL::ComPtr<IWICImagingFactory> mspWICFactory{};
	std::map<std::wstring, Microsoft::WRL::ComPtr<ID2D1Bitmap>> mBitmaps;

private:
	HRESULT LoadWICBitmap(std::wstring filename, ID2D1Bitmap** ppBitmap);

public:
	HRESULT Initialize(ID2D1HwndRenderTarget* pRT);
	void Release();

	ID2D1Bitmap* LoadBitmap(std::wstring filename);
};

