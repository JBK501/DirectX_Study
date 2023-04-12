#include <sstream>
#include "D3DFramework.h"

#pragma comment (lib, "d3d11.lib")

void D3DFramework::InitWindow(HINSTANCE hInstance)
{
	mInstance = hInstance;

	// 윈도우 구조체 정보를 설정한다.
	WNDCLASSEX wc;
	ZeroMemory(&wc, sizeof(WNDCLASSEX));
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpszClassName = CLASS_NAME.c_str();
	wc.hInstance = mInstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = 0;
	wc.lpfnWndProc = WindowProc;
	wc.cbSize = sizeof(WNDCLASSEX);

	// 클래스를 등록한다.
	if (!RegisterClassEx(&wc))
	{
		MessageBox(nullptr, L"Failed To Register WindowClass!", L"ERROR", MB_OK);
		return;
	}

	mTitleText = TITLE_NAME;

	// 해당 클래스에서 윈도우를 생성한다.
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

	// 화면에 표시한다.
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
	// 스왑체인을 초기화한다.
	DXGI_SWAP_CHAIN_DESC scd{};

	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));
	scd.BufferCount = 1;	// 백 버퍼 개수 지정
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// 픽셀 포멧 지정(여기서는 색상표현 능력만 지정함)
	scd.BufferDesc.Width = mWindowWidth;
	scd.BufferDesc.Height = mWindowHeight;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;	// 백버퍼에 대한 접근 방식 지정(여기서는 출력용 렌더타겟(그려질 장소)을 지정함)
	scd.OutputWindow = mHwnd;	// 윈도우 핸들 지정
	scd.SampleDesc.Count = 1;	// 1x MSAA
	scd.Windowed = TRUE;
	scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;	// 모드 변경이 가능하도록 지정

	// 디바이스, 디바이스 컨텍스트, 스왑 체인 오브젝트들 생성한다.
	D3D11CreateDeviceAndSwapChain(
		NULL,	// 그래픽 카드 지정(기본 어댑터 사용)
		D3D_DRIVER_TYPE_HARDWARE, // 렌더링 사용 지정(하드웨어 렌더링 사용)
		NULL,	// 렌더링 구동기 지정
		0,		// 플래그 지정
		NULL,	// 기능 수준 지정
		0,	
		D3D11_SDK_VERSION,	// SDK 버전 지정
		&scd,	// 스왑체인 구조체 지정
		mspSwapChain.ReleaseAndGetAddressOf(),	// 생성할 스왑체인 주소
		mspDevice.ReleaseAndGetAddressOf(),	// 생성할 디바이스 주소
		NULL,	// 기능 수준 배열 
		mspDeviceContext.ReleaseAndGetAddressOf()	// 생성할 디바이스 컨텍스트주소
	);

	OnResize();
}

void D3DFramework::CalculateFPS()
{
	static int frameCnt{};	// 초당 처리된 프레임 수 누적
	static float timeElapsed{};	// 한 프레임에 소요된 시간 저장
		
	frameCnt++;	// 프레임 카운트를 증가 시킨다.

	if (mTimer.TotalTime() - timeElapsed >= 1.0f)	// 타이머의 전체 시간과 경과 시간의 차이가 1초 이상이면
	{
		// 프레임 처리에 걸린 시간을 구한다.
		float fps = static_cast<float>(frameCnt);
		float mspf = 1000.0f / fps;

		// 타이틀용 텍스트 뒤에 프레임을 추가한다.
		std::wostringstream oss;
		oss.precision(6);

		oss << mTitleText << L" - " <<
			L"FPS : " << fps <<
			L" , Frame Time : " << mspf << L"(ms)";

		// 윈도우 타이틀을 갱신한다.
		SetWindowText(mHwnd, oss.str().c_str());

		// 1초가 지났으므로 frameCnt는 초기화하고, 경과시간을 1초 더해준다.
		frameCnt = 0;
		timeElapsed += 1.0f;
	}
}

// [해상도가 변경될 때 영향받는 리소스]
// 1. 스왑 체인
// 2. 렌더타겟 + 렌더타겟뷰
// 3. 깊이-스텐실 버퍼 + 깊이-스텐실 뷰
// 4. 렌더타겟 & 깊이-스텐실 버퍼 바인드
// 5. 뷰포트 크기

// 처음 스왑체인을 생성할 때는 해상도를 지정하지만 해상도가 변경되어 OnResize()가 호출되면 스왑체인 크기는 변하지 않는다.
// 따라서 스왑체인 버퍼 크기를 변경하는 기능을 추가해야 한다.
void D3DFramework::OnResize()
{
	// 방어코드 
	if (mspDevice == nullptr)
		return;

	// 기존에 생성된 디바이스 리소스들을 해제한다.
	ID3D11RenderTargetView* views[]{ nullptr };
	mspDeviceContext->OMSetRenderTargets(	// 현재 등록된 정보를 모두 초기화 한다.
		_countof(views), views, nullptr
	);
	// _countof() : MS에서만 사용하며 배열의 개수를 구하는 함수이다.

	mspRenderTargetView.Reset();	
	mspRenderTarget.Reset();	
	mspDepthStencilView.Reset();	
	mspDepthStencil.Reset();	

	// 병목현상으로 기다리고 있는 작업을 즉시 다 처리한다.
	mspDeviceContext->Flush();

	// 스왑체인 버퍼크기를 변경한다.
	mspSwapChain->ResizeBuffers(
		0,	// 스왑체인 버퍼 개수(0 : 기존에 있던 버퍼개수 유지)
		mWindowWidth, mWindowHeight,
		DXGI_FORMAT_UNKNOWN,	// 변경할 포멧을 지정(DXGI_FORMAT_UNKNOWN : 기존포멧 유지)
		0	// 추가 옵션
	);

	// 스왑체인에서 만들어진 백버퍼를 가져와서 렌더타켓으로 만든다.
	mspSwapChain->GetBuffer(
		0, // 버퍼 인덱스(여기서는 백버퍼를 하나만 사용함)
		IID_PPV_ARGS(mspRenderTarget.ReleaseAndGetAddressOf())	// 버퍼로 사용될 인터페이스 포인터(여기서는 백버퍼)
	);

	// 만들어진 리소스를 해석할 수 있는 렌더타켓 뷰를 생성한다.
	mspDevice->CreateRenderTargetView(
		mspRenderTarget.Get(),	// 렌더타겟 포인터 입력
		nullptr,	// 렌더타겟 뷰에 대한 설명 구조체 포인터(nullptr -> 밉맵 레벨 0의 모든 리소스에 접근 가능한 뷰가 생성됨)
		mspRenderTargetView.ReleaseAndGetAddressOf()	// 생성될 렌더타겟 뷰 인터페이스 
	);

	// 깊이-스텐실 버퍼 생성
	 
	// 2D 텍스쳐를 설정한다.
	CD3D11_TEXTURE2D_DESC td(	// 2D 텍스쳐를 설명하는 구조체
		DXGI_FORMAT_D24_UNORM_S8_UINT, // 텍셀 포멧(depth 24비트, stencil 8비트 사용)
		mWindowWidth,
		mWindowHeight,
		1,	 // 배열 크기 
		1,	// 밉맵 레벨
		D3D11_BIND_DEPTH_STENCIL	// 용도(깊이-스텐실)
	);

	// 2D 텍스쳐를 생성한다.
	mspDevice->CreateTexture2D(
		&td,	// 텍스처 설정한 구조체
		nullptr,	// 초기값
		mspDepthStencil.ReleaseAndGetAddressOf()	// 생성될 텍스쳐 인터페이스
	);

	// 깊이-스텐실 뷰 생성

	// 깊이-스텐실 뷰 설정
	CD3D11_DEPTH_STENCIL_VIEW_DESC dsvd(
		D3D11_DSV_DIMENSION_TEXTURE2D	// 깊이-스텐실 버퍼 포멧
	);

	// 깊이-스텐실 뷰 생성
	mspDevice->CreateDepthStencilView(
		mspDepthStencil.Get(),
		&dsvd,
		mspDepthStencilView.ReleaseAndGetAddressOf()	// 생성될 깊이-스텐실뷰 인터페이스
	);

	// OM(Output-Merger) : 3차원 렌더링의 결과물들을 합쳐서 최종장면을 만드는 과정을 말한다.
	// 렌터타겟과 깊이-스텐실버퍼를 바인딩 한다.
	mspDeviceContext->OMSetRenderTargets(
		1,
		mspRenderTargetView.GetAddressOf(),
		mspDepthStencilView.Get()
	);
}

void D3DFramework::DestroyD3D()
{
	// 전체화면모드를 해제한다.
	// 해제하지 않을 시 게임 종료할 때 응답없음이나 비정상 종료문제 발생 가능성이 있음.
	mspSwapChain->SetFullscreenState(FALSE, NULL);	

	mspDepthStencilView.Reset();
	mspDepthStencil.Reset();
	mspRenderTargetView.Reset();
	mspRenderTarget.Reset();
	mspSwapChain.Reset();
	mspDevice.Reset();
	mspDeviceContext.Reset();
	
}


//[렌더링 과정]
// 1. 뷰포트를 설정한다.
// 2. 렌더타켓을 지운다.
// 3. 깊이-스텐실 버퍼를 지운다.
// 4. 필요한 오브젝트를  렌더링한다.
// 5. 백버퍼를 스왑한다. -> 렌더링된 이미지를 사용자에게 보여준다.
void D3DFramework::RenderFrame()
{
	// Rasterizer
	//	-> 픽셀로 이미지를 표시하는 방식
	//	-> 여기서는 3D공간 정보를 2차원 이미지로 변환시킨다는 의미이다.

	// 뷰포트를 설정한다.
	CD3D11_VIEWPORT viewport( // 뷰포트 정보 (x, y, w, h)
		0.0f, 0.0f,
		static_cast<float>(mWindowWidth),
		static_cast<float>(mWindowHeight)
	);

	// 뷰포트를 바인딩 한다.
	mspDeviceContext->RSSetViewports(1, &viewport);

	// 렌더타겟을 지운다.
	const static float clear_color[4]{ 0.0f,0.2f,0.4f,1.0f };	// 배경색
	mspDeviceContext->ClearRenderTargetView( 
		mspRenderTargetView.Get(), // 렌더타켓뷰 인터페이스 
		clear_color	// 색상
	);

	// 깊이 스텐실 버퍼를 지운다.
	mspDeviceContext->ClearDepthStencilView(
		mspDepthStencilView.Get(),	// 깊이-스텐실뷰 인터페이스 
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,	// 어떤 리소스로 지울 지 플래그지정
		1.0f,	// 지울 깊이 버퍼 값
		0	// 지울 스텐실 버퍼 값
	);

	Render();

	// 스왑체인을 이용하여 화면에 그린다.(렌더링된 이미지를 사용자에게 보여준다.)
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

	// 응용프로그램이 비정상 종료될 경우 메모리 상에 남을 수도 있음.
	// 따라서 DestroyWindow를 사용한다.
	DestroyWindow(mHwnd);
	UnregisterClass(CLASS_NAME.c_str(), mInstance);	// 윈도우 클래스 등록해제
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
				mTimer.Update();	// 타이머를 업데이트 한다.
				
				if (mPaused)	// 앱이 멈춰 있으면	
				{
					Sleep(100);	// 0.1초 대기한다.(다른 앱에 실행기회를 줌)
				}
				else // 앱이 작동 중이면
				{
					CalculateFPS();	 // 프레임을 계산하여 타이틀에 표시한다.

					Update(mTimer.DeltaTime());	// 업데이트를 수행한다.

					RenderFrame();	// 그리기를 수행한다.
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

		// 사이즈 변경을 할 때
		case WM_ENTERSIZEMOVE:
			mResizing = true; // 사이즈 조절중 플래그 설정
			mPaused = true;
			mTimer.Stop();
			break;

		// 사이즈 변경중일 때
		case WM_SIZE:
			mWindowWidth = LOWORD(lParam);
			mWindowHeight = HIWORD(lParam);

			if (wParam == SIZE_MINIMIZED)	// 최소화 되었다면
			{
				if (!mPaused)
				{
					mTimer.Stop();
				}
				mPaused = true;
				mMinimized = true;
				mMaximized = false;
			}
			else if (wParam == SIZE_MAXIMIZED)	// 최대화 되었다면
			{
				mTimer.Resume();
				mPaused = false;
				mMinimized = false;
				mMaximized = true;
				OnResize();
			}
			else if (wParam == SIZE_RESTORED)	// 사이즈 복귀
			{
				if (mMinimized)	// 최소화 되었다가 돌아온다면
				{
					mPaused = false;
					mTimer.Resume();
					mMinimized = false;
					OnResize();
				}
				else if (mMaximized)	// 최대화 되었다가 돌아온다면
				{
					mPaused = false;
					mTimer.Resume();

					mMaximized = false;
					OnResize();
				}
				else if (mResizing)	// 드래그로 사이즈 조절중이라면
				{
					RenderFrame();
				}
				else
				{
					OnResize();
				}
			}
			break;

			// 사이즈 변경이 끝났을 때
		case WM_EXITSIZEMOVE:
			mPaused = false;
			mTimer.Resume();
			mResizing = false;	// 사이즈 조절중 플래그 해제
			OnResize();
			break;

			// 최소 사이즈 지정
		case WM_GETMINMAXINFO:
			((MINMAXINFO*)lParam)->ptMinTrackSize.x = 640;
			((MINMAXINFO*)lParam)->ptMinTrackSize.y = 480;
			break;

			// alt + enter 경고음 제거
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
