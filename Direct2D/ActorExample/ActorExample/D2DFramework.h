#pragma once
#include <Windows.h>
#include "BitmapManager.h"

class D2DFramework
{
	const LPCWSTR gClassName = { L"MyWindowClass" };

protected:	
	HWND mHwnd{};

	// shared_ptr<>
	Microsoft::WRL::ComPtr<ID2D1Factory> mspD2DFactory{};	// Device 독립
	Microsoft::WRL::ComPtr<ID2D1HwndRenderTarget> mspRenderTarget{};	// Device 종속

protected:
	HRESULT InitWindow(HINSTANCE hInstance, LPCWSTR title, UINT width, UINT height);
	virtual HRESULT InitD2D(HWND hwnd);
	virtual HRESULT CreateDeviceResources();

public:
	virtual void Initialize(HINSTANCE hInstance,
		LPCWSTR title = L"Direct2D Example",
		UINT width = 1024, UINT height = 768);
	virtual void Release();
	virtual void Render();
	virtual int GameLoop();

	void ShowErrorMsg(
		LPCWSTR msg, 
		HRESULT error = E_FAIL,
		LPCWSTR title = L"Error"
	);

public:
	ID2D1HwndRenderTarget* GetRenderTarget() const
	{
		return mspRenderTarget.Get();
	}
	ID2D1Factory* GetD2DFactory() const
	{
		return mspD2DFactory.Get();
	}
};

LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
