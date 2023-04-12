#include <sstream>
#include "D3DFramework.h"

#pragma comment (lib, "d3d11.lib")

void D3DFramework::InitWindow(HINSTANCE hInstance)
{
	mInstance = hInstance;

	// ������ ����ü ������ �����Ѵ�.
	WNDCLASSEX wc;
	ZeroMemory(&wc, sizeof(WNDCLASSEX));
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpszClassName = CLASS_NAME.c_str();
	wc.hInstance = mInstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = 0;
	wc.lpfnWndProc = WindowProc;
	wc.cbSize = sizeof(WNDCLASSEX);

	// Ŭ������ ����Ѵ�.
	if (!RegisterClassEx(&wc))
	{
		MessageBox(nullptr, L"Failed To Register WindowClass!", L"ERROR", MB_OK);
		return;
	}

	mTitleText = TITLE_NAME;

	// �ش� Ŭ�������� �����츦 �����Ѵ�.
	RECT wr{ 0,0,mWindowWidth,mWindowHeight };
	AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);
	mHwnd = CreateWindowEx(
		NULL,
		CLASS_NAME.c_str(),
		mTitleText.c_str(),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		wr.right - wr.left, wr.bottom - wr.top,
		NULL,
		NULL,
		mInstance,
		NULL
	);

	if (mHwnd == NULL)
	{
		MessageBox(nullptr, L"Failed To Create Window!", L"ERROR", MB_OK);
		return;
	}

	// ȭ�鿡 ǥ���Ѵ�.
	ShowWindow(mHwnd, SW_SHOW);
	SetForegroundWindow(mHwnd);
	SetFocus(mHwnd);
	UpdateWindow(mHwnd);

	SetWindowLongPtr(
		mHwnd,GWLP_USERDATA,
		reinterpret_cast<LONG_PTR>(this)
	);
}

void D3DFramework::InitD3D()
{
	// ����ü���� �ʱ�ȭ�Ѵ�.
	DXGI_SWAP_CHAIN_DESC scd{};

	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));
	scd.BufferCount = 1;	// �� ���� ���� ����
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// �ȼ� ���� ����(���⼭�� ����ǥ�� �ɷ¸� ������)
	scd.BufferDesc.Width = mWindowWidth;
	scd.BufferDesc.Height = mWindowHeight;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;	// ����ۿ� ���� ���� ��� ����(���⼭�� ��¿� ����Ÿ��(�׷��� ���)�� ������)
	scd.OutputWindow = mHwnd;	// ������ �ڵ� ����
	scd.SampleDesc.Count = 1;	// 1x MSAA
	scd.Windowed = TRUE;
	scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;	// ��� ������ �����ϵ��� ����

	// ����̽�, ����̽� ���ؽ�Ʈ, ���� ü�� ������Ʈ�� �����Ѵ�.
	D3D11CreateDeviceAndSwapChain(
		NULL,	// �׷��� ī�� ����(�⺻ ����� ���)
		D3D_DRIVER_TYPE_HARDWARE, // ������ ��� ����(�ϵ���� ������ ���)
		NULL,	// ������ ������ ����
		0,		// �÷��� ����
		NULL,	// ��� ���� ����
		0,	
		D3D11_SDK_VERSION,	// SDK ���� ����
		&scd,	// ����ü�� ����ü ����
		mspSwapChain.ReleaseAndGetAddressOf(),	// ������ ����ü�� �ּ�
		mspDevice.ReleaseAndGetAddressOf(),	// ������ ����̽� �ּ�
		NULL,	// ��� ���� �迭 
		mspDeviceContext.ReleaseAndGetAddressOf()	// ������ ����̽� ���ؽ�Ʈ�ּ�
	);

	OnResize();
}

void D3DFramework::CalculateFPS()
{
	static int frameCnt{};	// �ʴ� ó���� ������ �� ����
	static float timeElapsed{};	// �� �����ӿ� �ҿ�� �ð� ����
		
	frameCnt++;	// ������ ī��Ʈ�� ���� ��Ų��.

	if (mTimer.TotalTime() - timeElapsed >= 1.0f)	// Ÿ�̸��� ��ü �ð��� ��� �ð��� ���̰� 1�� �̻��̸�
	{
		// ������ ó���� �ɸ� �ð��� ���Ѵ�.
		float fps = static_cast<float>(frameCnt);
		float mspf = 1000.0f / fps;

		// Ÿ��Ʋ�� �ؽ�Ʈ �ڿ� �������� �߰��Ѵ�.
		std::wostringstream oss;
		oss.precision(6);

		oss << mTitleText << L" - " <<
			L"FPS : " << fps <<
			L" , Frame Time : " << mspf << L"(ms)";

		// ������ Ÿ��Ʋ�� �����Ѵ�.
		SetWindowText(mHwnd, oss.str().c_str());

		// 1�ʰ� �������Ƿ� frameCnt�� �ʱ�ȭ�ϰ�, ����ð��� 1�� �����ش�.
		frameCnt = 0;
		timeElapsed += 1.0f;
	}
}

// [�ػ󵵰� ����� �� ����޴� ���ҽ�]
// 1. ���� ü��
// 2. ����Ÿ�� + ����Ÿ�ٺ�
// 3. ����-���ٽ� ���� + ����-���ٽ� ��
// 4. ����Ÿ�� & ����-���ٽ� ���� ���ε�
// 5. ����Ʈ ũ��

// ó�� ����ü���� ������ ���� �ػ󵵸� ���������� �ػ󵵰� ����Ǿ� OnResize()�� ȣ��Ǹ� ����ü�� ũ��� ������ �ʴ´�.
// ���� ����ü�� ���� ũ�⸦ �����ϴ� ����� �߰��ؾ� �Ѵ�.
void D3DFramework::OnResize()
{
	// ����ڵ� 
	if (mspDevice == nullptr)
		return;

	// ������ ������ ����̽� ���ҽ����� �����Ѵ�.
	ID3D11RenderTargetView* views[]{ nullptr };
	mspDeviceContext->OMSetRenderTargets(	// ���� ��ϵ� ������ ��� �ʱ�ȭ �Ѵ�.
		_countof(views), views, nullptr
	);
	// _countof() : MS������ ����ϸ� �迭�� ������ ���ϴ� �Լ��̴�.

	mspRenderTargetView.Reset();	
	mspRenderTarget.Reset();	
	mspDepthStencilView.Reset();	
	mspDepthStencil.Reset();	

	// ������������ ��ٸ��� �ִ� �۾��� ��� �� ó���Ѵ�.
	mspDeviceContext->Flush();

	// ����ü�� ����ũ�⸦ �����Ѵ�.
	mspSwapChain->ResizeBuffers(
		0,	// ����ü�� ���� ����(0 : ������ �ִ� ���۰��� ����)
		mWindowWidth, mWindowHeight,
		DXGI_FORMAT_UNKNOWN,	// ������ ������ ����(DXGI_FORMAT_UNKNOWN : �������� ����)
		0	// �߰� �ɼ�
	);

	// ����ü�ο��� ������� ����۸� �����ͼ� ����Ÿ������ �����.
	mspSwapChain->GetBuffer(
		0, // ���� �ε���(���⼭�� ����۸� �ϳ��� �����)
		IID_PPV_ARGS(mspRenderTarget.ReleaseAndGetAddressOf())	// ���۷� ���� �������̽� ������(���⼭�� �����)
	);

	// ������� ���ҽ��� �ؼ��� �� �ִ� ����Ÿ�� �並 �����Ѵ�.
	mspDevice->CreateRenderTargetView(
		mspRenderTarget.Get(),	// ����Ÿ�� ������ �Է�
		nullptr,	// ����Ÿ�� �信 ���� ���� ����ü ������(nullptr -> �Ӹ� ���� 0�� ��� ���ҽ��� ���� ������ �䰡 ������)
		mspRenderTargetView.ReleaseAndGetAddressOf()	// ������ ����Ÿ�� �� �������̽� 
	);

	// ����-���ٽ� ���� ����
	 
	// 2D �ؽ��ĸ� �����Ѵ�.
	CD3D11_TEXTURE2D_DESC td(	// 2D �ؽ��ĸ� �����ϴ� ����ü
		DXGI_FORMAT_D24_UNORM_S8_UINT, // �ؼ� ����(depth 24��Ʈ, stencil 8��Ʈ ���)
		mWindowWidth,
		mWindowHeight,
		1,	 // �迭 ũ�� 
		1,	// �Ӹ� ����
		D3D11_BIND_DEPTH_STENCIL	// �뵵(����-���ٽ�)
	);

	// 2D �ؽ��ĸ� �����Ѵ�.
	mspDevice->CreateTexture2D(
		&td,	// �ؽ�ó ������ ����ü
		nullptr,	// �ʱⰪ
		mspDepthStencil.ReleaseAndGetAddressOf()	// ������ �ؽ��� �������̽�
	);

	// ����-���ٽ� �� ����

	// ����-���ٽ� �� ����
	CD3D11_DEPTH_STENCIL_VIEW_DESC dsvd(
		D3D11_DSV_DIMENSION_TEXTURE2D	// ����-���ٽ� ���� ����
	);

	// ����-���ٽ� �� ����
	mspDevice->CreateDepthStencilView(
		mspDepthStencil.Get(),
		&dsvd,
		mspDepthStencilView.ReleaseAndGetAddressOf()	// ������ ����-���ٽǺ� �������̽�
	);

	// OM(Output-Merger) : 3���� �������� ��������� ���ļ� ��������� ����� ������ ���Ѵ�.
	// ����Ÿ�ٰ� ����-���ٽǹ��۸� ���ε� �Ѵ�.
	mspDeviceContext->OMSetRenderTargets(
		1,
		mspRenderTargetView.GetAddressOf(),
		mspDepthStencilView.Get()
	);
}

void D3DFramework::DestroyD3D()
{
	// ��üȭ���带 �����Ѵ�.
	// �������� ���� �� ���� ������ �� ��������̳� ������ ���Ṯ�� �߻� ���ɼ��� ����.
	mspSwapChain->SetFullscreenState(FALSE, NULL);	

	mspDepthStencilView.Reset();
	mspDepthStencil.Reset();
	mspRenderTargetView.Reset();
	mspRenderTarget.Reset();
	mspSwapChain.Reset();
	mspDevice.Reset();
	mspDeviceContext.Reset();
	
}


//[������ ����]
// 1. ����Ʈ�� �����Ѵ�.
// 2. ����Ÿ���� �����.
// 3. ����-���ٽ� ���۸� �����.
// 4. �ʿ��� ������Ʈ��  �������Ѵ�.
// 5. ����۸� �����Ѵ�. -> �������� �̹����� ����ڿ��� �����ش�.
void D3DFramework::RenderFrame()
{
	// Rasterizer
	//	-> �ȼ��� �̹����� ǥ���ϴ� ���
	//	-> ���⼭�� 3D���� ������ 2���� �̹����� ��ȯ��Ų�ٴ� �ǹ��̴�.

	// ����Ʈ�� �����Ѵ�.
	CD3D11_VIEWPORT viewport( // ����Ʈ ���� (x, y, w, h)
		0.0f, 0.0f,
		static_cast<float>(mWindowWidth),
		static_cast<float>(mWindowHeight)
	);

	// ����Ʈ�� ���ε� �Ѵ�.
	mspDeviceContext->RSSetViewports(1, &viewport);

	// ����Ÿ���� �����.
	const static float clear_color[4]{ 0.0f,0.2f,0.4f,1.0f };	// ����
	mspDeviceContext->ClearRenderTargetView( 
		mspRenderTargetView.Get(), // ����Ÿ�Ϻ� �������̽� 
		clear_color	// ����
	);

	// ���� ���ٽ� ���۸� �����.
	mspDeviceContext->ClearDepthStencilView(
		mspDepthStencilView.Get(),	// ����-���ٽǺ� �������̽� 
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,	// � ���ҽ��� ���� �� �÷�������
		1.0f,	// ���� ���� ���� ��
		0	// ���� ���ٽ� ���� ��
	);

	Render();

	// ����ü���� �̿��Ͽ� ȭ�鿡 �׸���.(�������� �̹����� ����ڿ��� �����ش�.)
	mspSwapChain->Present(0, 0);
}

void D3DFramework::Initialize(HINSTANCE hInstance, int width, int height)
{
	mWindowWidth = width;
	mWindowHeight = height;
	mPaused = false;


	InitWindow(hInstance);
	InitD3D();
}

void D3DFramework::Destroy()
{
	DestroyD3D();

	// �������α׷��� ������ ����� ��� �޸� �� ���� ���� ����.
	// ���� DestroyWindow�� ����Ѵ�.
	DestroyWindow(mHwnd);
	UnregisterClass(CLASS_NAME.c_str(), mInstance);	// ������ Ŭ���� �������
}

void D3DFramework::GameLoop()
{
	mTimer.Start();
	mInput.Initialize();

	MSG msg{};
	while (true)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message == WM_QUIT)
				break;
			else
			{
				mTimer.Update();	// Ÿ�̸Ӹ� ������Ʈ �Ѵ�.
				
				if (mPaused)	// ���� ���� ������	
				{
					Sleep(100);	// 0.1�� ����Ѵ�.(�ٸ� �ۿ� �����ȸ�� ��)
				}
				else // ���� �۵� ���̸�
				{
					CalculateFPS();	 // �������� ����Ͽ� Ÿ��Ʋ�� ǥ���Ѵ�.

					Update(mTimer.DeltaTime());	// ������Ʈ�� �����Ѵ�.

					RenderFrame();	// �׸��⸦ �����Ѵ�.
				}
			}
		}
	}
}

LRESULT D3DFramework::MessageHandler(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_KEYDOWN:
			mInput.SetKeyDown(wParam);
			break;
		case WM_KEYUP:
			mInput.SetKeyUp(wParam);
			break;
		case WM_LBUTTONDOWN:
			mInput.SetKeyDown(VK_LBUTTON);
			break;
		case WM_LBUTTONUP:
			mInput.SetKeyUp(VK_LBUTTON);
			break;
		case WM_RBUTTONDOWN:
			mInput.SetKeyDown(VK_RBUTTON);
			break;
		case WM_RBUTTONUP:
			mInput.SetKeyUp(VK_RBUTTON);
			break;
		case WM_MOUSEMOVE:
			mInput.SetCursor(LOWORD(lParam),HIWORD(lParam));
			break;

		case WM_ACTIVATE:
			if (LOWORD(wParam) == WA_INACTIVE)
			{
				mPaused = true;
				mTimer.Stop();
			}
			else
			{
				mPaused = false;
				mTimer.Resume();
			}
			break;

		// ������ ������ �� ��
		case WM_ENTERSIZEMOVE:
			mResizing = true; // ������ ������ �÷��� ����
			mPaused = true;
			mTimer.Stop();
			break;

		// ������ �������� ��
		case WM_SIZE:
			mWindowWidth = LOWORD(lParam);
			mWindowHeight = HIWORD(lParam);

			if (wParam == SIZE_MINIMIZED)	// �ּ�ȭ �Ǿ��ٸ�
			{
				if (!mPaused)
				{
					mTimer.Stop();
				}
				mPaused = true;
				mMinimized = true;
				mMaximized = false;
			}
			else if (wParam == SIZE_MAXIMIZED)	// �ִ�ȭ �Ǿ��ٸ�
			{
				mTimer.Resume();
				mPaused = false;
				mMinimized = false;
				mMaximized = true;
				OnResize();
			}
			else if (wParam == SIZE_RESTORED)	// ������ ����
			{
				if (mMinimized)	// �ּ�ȭ �Ǿ��ٰ� ���ƿ´ٸ�
				{
					mPaused = false;
					mTimer.Resume();
					mMinimized = false;
					OnResize();
				}
				else if (mMaximized)	// �ִ�ȭ �Ǿ��ٰ� ���ƿ´ٸ�
				{
					mPaused = false;
					mTimer.Resume();

					mMaximized = false;
					OnResize();
				}
				else if (mResizing)	// �巡�׷� ������ �������̶��
				{
					RenderFrame();
				}
				else
				{
					OnResize();
				}
			}
			break;

			// ������ ������ ������ ��
		case WM_EXITSIZEMOVE:
			mPaused = false;
			mTimer.Resume();
			mResizing = false;	// ������ ������ �÷��� ����
			OnResize();
			break;

			// �ּ� ������ ����
		case WM_GETMINMAXINFO:
			((MINMAXINFO*)lParam)->ptMinTrackSize.x = 640;
			((MINMAXINFO*)lParam)->ptMinTrackSize.y = 480;
			break;

			// alt + enter ����� ����
		case WM_MENUCHAR:
			return MAKELRESULT(0, MNC_CLOSE);
			break;

		case WM_CLOSE:
			DestroyWindow(mHwnd);
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;

		default:
			return DefWindowProc(hwnd, message, wParam, lParam);
			break;
	}

	return 0;
}

LRESULT D3DFramework::WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	D3DFramework* pFramework = reinterpret_cast<D3DFramework*>(
		GetWindowLongPtr(hwnd, GWLP_USERDATA)
	);

	if (pFramework == nullptr)
		return DefWindowProc(hwnd, message, wParam, lParam);
	else
		return pFramework->MessageHandler(hwnd, message, wParam, lParam);
}
