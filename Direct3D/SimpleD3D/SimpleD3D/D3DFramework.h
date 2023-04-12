#pragma once

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <wrl/client.h>
#include <d3d11.h>
#include <string>
#include "Timer.h"
#include "Input.h"

// [Direct3D �ʱ�ȭ �ٰŸ�]
// 1. ����̽� ����(����̽� ���ؽ�Ʈ ����)
// 2. ����ü�� ����
// 3. ����۸� ���� Ÿ������ ����(���� Ÿ�� �� ����)
// 4. ����-���ٽ� ���� ����(����-���� �� ����)
// 5. ����Ʈ ����
// 6. ȭ�鿡 �׸���


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
	bool mPaused{ false };	// ���� ������� �ƴ����� ���� �÷���

	std::wstring mTitleText{};	// Ÿ��Ʋ �ؽ�Ʈ
	MyUtil::Timer mTimer;	// Ÿ�̸� �ν��Ͻ�
	MyUtil::Input mInput;

	Microsoft::WRL::ComPtr<ID3D11Device> mspDevice{};
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> mspDeviceContext{};
	Microsoft::WRL::ComPtr<IDXGISwapChain> mspSwapChain{};

	// Resource(������ ����) 
	Microsoft::WRL::ComPtr<ID3D11Texture2D> mspRenderTarget{};	// �������� ������� 2���� �ؽ��� ����
	Microsoft::WRL::ComPtr<ID3D11Texture2D> mspDepthStencil{};	// ���� ���ٽ� ���۷� ����� 2���� �ؽ���

	// Resource-View(������ ������ ���� ����) -> ���� �ؽ��� �����͸� �ؼ� 
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> mspRenderTargetView{};
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> mspDepthStencilView{};

private:
	void InitWindow(HINSTANCE hInstance);
	void InitD3D();
	void CalculateFPS();	// FPS�� ����Ͽ� Ÿ��Ʋ�ٿ� ����Ѵ�.

protected:
	void OnResize();
	void DestroyD3D();
	void RenderFrame();
	virtual void Render() {}; // �ش� �����ӿ�ũ�� ��ӹ��� ������ �������̵��Ͽ� ������ �׸��⿡�� �����Ѵ�.
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

