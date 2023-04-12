#pragma once

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <wrl/client.h>
#include <d3d11.h>
#include <string>
#include "Timer.h"
#include "Input.h"

// [Direct3D 초기화 줄거리]
// 1. 디바이스 생성(디바이스 컨텍스트 포함)
// 2. 스왑체인 생성
// 3. 백버퍼를 렌더 타켓으로 지정(렌더 타켓 뷰 생성)
// 4. 깊이-스텐실 버퍼 생성(깊이-버퍼 뷰 생성)
// 5. 뷰포트 지정
// 6. 화면에 그리기


class D3DFramework
{
private:
	const std::wstring CLASS_NAME{ L"MyWindowClass" };
	const std::wstring TITLE_NAME{ L"Direct3D Sample" };

protected:
	int mWindowWidth{ 800 };
	int mWindowHeight{ 600 };
	HWND mHwnd{};
	HINSTANCE mInstance{};
	bool mMinimized{ false };
	bool mMaximized{ false };
	bool mResizing{ false };
	bool mPaused{ false };	// 앱이 멈췄는지 아닌지에 대한 플래그

	std::wstring mTitleText{};	// 타이틀 텍스트
	MyUtil::Timer mTimer;	// 타이머 인스턴스
	MyUtil::Input mInput;

	Microsoft::WRL::ComPtr<ID3D11Device> mspDevice{};
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> mspDeviceContext{};
	Microsoft::WRL::ComPtr<IDXGISwapChain> mspSwapChain{};

	// Resource(데이터 묶음) 
	Microsoft::WRL::ComPtr<ID3D11Texture2D> mspRenderTarget{};	// 렌더링할 대상으로 2차원 텍스쳐 지정
	Microsoft::WRL::ComPtr<ID3D11Texture2D> mspDepthStencil{};	// 깊이 스텐실 버퍼로 사용할 2차원 텍스쳐

	// Resource-View(데이터 묶음에 대한 설명서) -> 범용 텍스쳐 데이터를 해석 
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> mspRenderTargetView{};
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> mspDepthStencilView{};

private:
	void InitWindow(HINSTANCE hInstance);
	void InitD3D();
	void CalculateFPS();	// FPS를 계산하여 타이틀바에 출력한다.

protected:
	void OnResize();
	void DestroyD3D();
	void RenderFrame();
	virtual void Render() {}; // 해당 프레임워크를 상속받은 곳에서 오버라이딩하여 오로직 그리기에만 집중한다.
	virtual void Update(float delta) {};

public:
	virtual void Initialize(
		HINSTANCE hInstance,
		int width = 800, 
		int height = 600
	);
	virtual void Destroy();
	virtual void GameLoop();

public:
	LRESULT CALLBACK MessageHandler(HWND hwnd, UINT message,
		WPARAM wParam, LPARAM lParam);

	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
};

