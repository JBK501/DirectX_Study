#include "BitmapManger.h"

#pragma comment(lib,"WindowsCodecs.lib")

using namespace std;
using namespace Microsoft::WRL;

HRESULT BitmapManger::LoadWICBitmap(std::wstring filename, ID2D1Bitmap** ppBitmap)
{
	HRESULT hr;
	// 1. 디코더 생성 (Decoder)
	Microsoft::WRL::ComPtr<IWICBitmapDecoder> decoder;
	hr = mspWICFactory->CreateDecoderFromFilename(
		filename.c_str(),
		nullptr,
		GENERIC_READ,
		WICDecodeMetadataCacheOnLoad,
		decoder.GetAddressOf()
	);
	ThrowIfFailed(hr);

	// 2. 디코더에서 프레임 획득 (BMP파일 - 0번 프레임)
	Microsoft::WRL::ComPtr<IWICBitmapFrameDecode> frame;
	hr = decoder->GetFrame(0, frame.GetAddressOf());
	ThrowIfFailed(hr);

	// 3. 컨버터로 프레임에서 픽셀 배열을 구해온다.
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

	// 4. 픽셀배열로 비트맵을 생성한다.
	hr = mpRenderTarget->CreateBitmapFromWicBitmap(
		converter.Get(),
		ppBitmap
	);
	ThrowIfFailed(hr);

	return S_OK;
}

HRESULT BitmapManger::Initialize(ID2D1HwndRenderTarget* pRT)
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

void BitmapManger::Release()
{
    for (auto& e : mBitmaps)
    {
        e.second.Reset();
    }
    mBitmaps.clear();

    mspWICFactory.Reset();
}

ID2D1Bitmap* BitmapManger::LoadBitmap(std::wstring filename)
{
	// 방어 코드
	if (mspWICFactory == nullptr)
		return nullptr;

    // 처음 부르면 생성한다.
	auto result = mBitmaps.insert({ filename, nullptr });
	if (result.second == true)
	{
		auto spBitmap = ComPtr<ID2D1Bitmap>();
		LoadWICBitmap(filename, spBitmap.GetAddressOf());
		result.first->second = spBitmap;
	}
	
	return result.first->second.Get();
}
