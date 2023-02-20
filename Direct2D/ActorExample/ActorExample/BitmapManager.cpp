#include "BitmapManager.h"

#pragma comment(lib,"WindowsCodecs.lib")

using namespace std;
using namespace Microsoft::WRL;

HRESULT BitmapManager::LoadWICBitmap(std::wstring filename, ID2D1Bitmap** ppBitmap)
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
	// 방어 코드
	if (mspWICFactory == nullptr)
		return nullptr;

    // 처음 부르면 생성한다.
	// insert했을 때 해당 파일이 있으면 중복된 iterator(key)와 false(value)쌍으로 값을 돌려준다.
	// insert했을 때 해당 파일이 없으면 삽입된 iterator(key)와 true(value)쌍으로 값을 돌려준다.
	auto result = mBitmapResources.insert({ filename, nullptr });
	
	// 1. 해당 파일이 없으면
	if (result.second == true)	
	{
		auto spBitmap = ComPtr<ID2D1Bitmap>();
		LoadWICBitmap(filename, spBitmap.GetAddressOf());	// 1.1. 이미지를 로딩한다.
		result.first->second = spBitmap;	// 1.2. map에 추가한다.
	}
	
	return result.first->second.Get();
}
