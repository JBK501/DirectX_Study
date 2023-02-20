#include "BitmapManager.h"

#pragma comment(lib,"WindowsCodecs.lib")

using namespace std;
using namespace Microsoft::WRL;

HRESULT BitmapManager::LoadWICBitmap(std::wstring filename, ID2D1Bitmap** ppBitmap)
{
	HRESULT hr;
	// 1. ���ڴ� ���� (Decoder)
	Microsoft::WRL::ComPtr<IWICBitmapDecoder> decoder;
	hr = mspWICFactory->CreateDecoderFromFilename(
		filename.c_str(),
		nullptr,
		GENERIC_READ,
		WICDecodeMetadataCacheOnLoad,
		decoder.GetAddressOf()
	);
	ThrowIfFailed(hr);

	// 2. ���ڴ����� ������ ȹ�� (BMP���� - 0�� ������)
	Microsoft::WRL::ComPtr<IWICBitmapFrameDecode> frame;
	hr = decoder->GetFrame(0, frame.GetAddressOf());
	ThrowIfFailed(hr);

	// 3. �����ͷ� �����ӿ��� �ȼ� �迭�� ���ؿ´�.
	Microsoft::WRL::ComPtr<IWICFormatConverter> converter;
	hr = mspWICFactory->CreateFormatConverter(converter.GetAddressOf());
	ThrowIfFailed(hr);

	hr = converter->Initialize(
		frame.Get(),
		GUID_WICPixelFormat32bppPBGRA,
		WICBitmapDitherTypeNone,
		nullptr,
		0,
		WICBitmapPaletteTypeCustom
	);
	ThrowIfFailed(hr);

	// 4. �ȼ��迭�� ��Ʈ���� �����Ѵ�.
	hr = mpRenderTarget->CreateBitmapFromWicBitmap(
		converter.Get(),
		ppBitmap
	);
	ThrowIfFailed(hr);

	return S_OK;
}

HRESULT BitmapManager::Initialize(ID2D1HwndRenderTarget* pRT)
{
    if (pRT == nullptr)
        return E_FAIL;

    mpRenderTarget = pRT;

    HRESULT hr = CoCreateInstance(
        CLSID_WICImagingFactory,
        nullptr,
        CLSCTX_INPROC_SERVER,
        IID_PPV_ARGS(mspWICFactory.GetAddressOf())
    );
    ThrowIfFailed(hr);

    return hr;
}

void BitmapManager::Release()
{
    for (auto& e : mBitmapResources)
    {
        e.second.Reset();
    }
	mBitmapResources.clear();

    mspWICFactory.Reset();
}

ID2D1Bitmap* BitmapManager::LoadBitmap(std::wstring filename)
{
	// ��� �ڵ�
	if (mspWICFactory == nullptr)
		return nullptr;

    // ó�� �θ��� �����Ѵ�.
	// insert���� �� �ش� ������ ������ �ߺ��� iterator(key)�� false(value)������ ���� �����ش�.
	// insert���� �� �ش� ������ ������ ���Ե� iterator(key)�� true(value)������ ���� �����ش�.
	auto result = mBitmapResources.insert({ filename, nullptr });
	
	// 1. �ش� ������ ������
	if (result.second == true)	
	{
		auto spBitmap = ComPtr<ID2D1Bitmap>();
		LoadWICBitmap(filename, spBitmap.GetAddressOf());	// 1.1. �̹����� �ε��Ѵ�.
		result.first->second = spBitmap;	// 1.2. map�� �߰��Ѵ�.
	}
	
	return result.first->second.Get();
}