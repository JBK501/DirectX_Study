#pragma once
#include <d2d1.h>
#include <wincodec.h>
#include <wrl/client.h>
#include <map>
#include <string>
#include "ComException.h"

class BitmapManager
{
public:
	static BitmapManager& Instance()
	{
		static BitmapManager instance;
		return instance;
	}

private:
	BitmapManager() = default;
	BitmapManager(const BitmapManager&) = delete;
	BitmapManager& operator= (const BitmapManager&) = delete;

public:
	~BitmapManager() = default;

private:
	// 이미지를 불러온 후 컨버팅할 때 사용하는 렌더 타겟
	ID2D1HwndRenderTarget* mpRenderTarget{};
	Microsoft::WRL::ComPtr<IWICImagingFactory> mspWICFactory{};

	// 이미지 관리 컨테이너
	std::map < std::wstring, Microsoft::WRL::ComPtr<ID2D1Bitmap>> mBitmapResources;

private:
	// 내부 함수로 코덱을 만들고 컨버팅하여 비트맵을 생성해준다.
	HRESULT LoadWICBitmap(std::wstring filename, ID2D1Bitmap * *ppBitmap);

public:
	// 싱글턴 패턴에서는 생성자/소멸자에 의지하기 어려움.
	// 따라서 명시적으로 초기화/해제 함수가 있으면 좋다.
	HRESULT Initialize(ID2D1HwndRenderTarget * pRT);
	void Release();

	// 맵에 데이터가 있는지 없는지 확인하고 관리한다.
	ID2D1Bitmap* LoadBitmap(std::wstring filename);
};

