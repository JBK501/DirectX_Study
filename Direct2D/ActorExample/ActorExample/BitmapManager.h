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
	// �̹����� �ҷ��� �� �������� �� ����ϴ� ���� Ÿ��
	ID2D1HwndRenderTarget* mpRenderTarget{};
	Microsoft::WRL::ComPtr<IWICImagingFactory> mspWICFactory{};

	// �̹��� ���� �����̳�
	std::map < std::wstring, Microsoft::WRL::ComPtr<ID2D1Bitmap>> mBitmapResources;

private:
	// ���� �Լ��� �ڵ��� ����� �������Ͽ� ��Ʈ���� �������ش�.
	HRESULT LoadWICBitmap(std::wstring filename, ID2D1Bitmap * *ppBitmap);

public:
	// �̱��� ���Ͽ����� ������/�Ҹ��ڿ� �����ϱ� �����.
	// ���� ��������� �ʱ�ȭ/���� �Լ��� ������ ����.
	HRESULT Initialize(ID2D1HwndRenderTarget * pRT);
	void Release();

	// �ʿ� �����Ͱ� �ִ��� ������ Ȯ���ϰ� �����Ѵ�.
	ID2D1Bitmap* LoadBitmap(std::wstring filename);
};

