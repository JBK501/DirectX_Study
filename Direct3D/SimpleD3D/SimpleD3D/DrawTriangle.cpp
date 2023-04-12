#include <fstream>
#include <vector>
#include "DrawTriangle.h"

#pragma comment(lib,"d3dcompiler.lib")

void DrawTriangle::Initialize(HINSTANCE hInstance, int width, int height)
{
	D3DFramework::Initialize(hInstance, width, height);

	//InitTriangle();
	InitRectangle();
	InitPipeline();
	CreateTextureFromBMP();
}

void DrawTriangle::Destroy()
{
	mspBlendState.Reset();
	mspSamplerState.Reset();
	mspTextureView.Reset();
	mspTexture.Reset();
	mspInputLayout.Reset();
	mspPixelShader.Reset();
	mspVertexShader.Reset();
	mspVertexBuffer.Reset();

	D3DFramework::Destroy();
}

void DrawTriangle::Render()
{
	// stride : 원소를 구분하는 정보(여기서는 하나의 버텍스 정보가 몇 바이트인지 표시함)
	// offset : 건너뛴 위치
	UINT stride = sizeof(VERTEX);	
	UINT offset = 0;

	// Input-Assembler에 버텍스버퍼를 연결한다. (GPU는 이 버텍스 버퍼를 읽기 시작한다.)
	mspDeviceContext->IASetVertexBuffers(
		0,	// 연결된 슬롯 번호 입력(하나만 사용함)
		1,	// 버텍스 버퍼 개수 입력(한 개만 있음)
		mspVertexBuffer.GetAddressOf(),	// 버텍스 버퍼 배열 포인터 입력
		&stride,	
		&offset
	);

	// 기본도형을 지정한다.
	//mspDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	mspDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	
	// 버텍스 셰이더 스테이지에서 사용할 상수 버퍼를 지정한다.
	mspDeviceContext->VSSetConstantBuffers(0, 1, mspConstantBuffer.GetAddressOf());
	
	// 상수 버퍼를 갱신한다.
	MatrixBuffer mb;	// 셰이더에 전달할 상수 버퍼 구조체
	mb.world = DirectX::XMMatrixTranspose(mWorld);	// 전치 행렬을 구한다.
	mspDeviceContext->UpdateSubresource(
		mspConstantBuffer.Get(), 
		0, 
		nullptr, 
		&mb, 
		0,
		0
	);

	// 픽셀셰이더에 리소스를 넘겨준다.(자동적으로 픽셀셰이더의 텍스쳐 오브젝트(Texture2D)로 해석된다.)
	mspDeviceContext->PSSetShaderResources(
		0, 1, mspTextureView.GetAddressOf()
	);

	// 샘플러를 픽셀셰이더에 연결해준다.
	mspDeviceContext->PSSetSamplers(
		0,1,mspSamplerState.GetAddressOf()
	);

	// Output-Merger스테이지에 블렌딩 상태를 지정한다.
	mspDeviceContext->OMSetBlendState(
		mspBlendState.Get(), nullptr, 0xffffffff
	);

	// 그린다.
	//mspDeviceContext->Draw(3, 0);
	mspDeviceContext->Draw(4, 0);
	
}

void DrawTriangle::Update(float delta)
{
	if (mInput.IsKeyDown('W'))
	{
		mY += 1.0f * delta;
	}
	else if (mInput.IsKeyDown('S'))
	{
		mY -= 1.0f * delta;
	}

	if (mInput.IsKeyDown('A'))
	{
		mX -= 1.0f * delta;
	} 
	else if (mInput.IsKeyDown('D'))
	{
		mX += 1.0f * delta;
	}

	if (mInput.IsKeyDown('Q'))
	{
		mRotation += DirectX::XM_PI * delta;
	}
	else if (mInput.IsKeyDown('E'))
	{
		mRotation -= DirectX::XM_PI * delta;
	}

	if (mInput.IsKeyDown('1'))
	{
		mTimer.SetScale(1.0f);
	}
	else if (mInput.IsKeyDown('2'))
	{
		mTimer.SetScale(2.0f);
	}
	else if (mInput.IsKeyDown('3'))
	{
		mTimer.SetScale(0.5f);
	}

	mWorld = DirectX::XMMatrixIdentity();
	mWorld *= DirectX::XMMatrixRotationZ(mRotation);
	mWorld *= DirectX::XMMatrixTranslation(mX, mY, 0.0f);
}

//void DrawTriangle::InitTriangle()
//{
//	// 화면에 그릴 삼각형을 초기화 한다.
//	VERTEX vertices[]
//	{
//		{0.0f, 0.5f, 0.0f, {1.0,0.0f,0.0f,1.0f}},
//		{0.45f, -0.5f, 0.0f, {0.0f,1.0f,0.0f,1.0f}},
//		{-0.45f,-0.5f, 0.0f, {0.0f,0.0f,1.0f,1.0f}},
//	};
//
//	// 버퍼에 대한 설명 구초체
//	CD3D11_BUFFER_DESC bd(
//		sizeof(vertices),	// 버퍼 크기
//		D3D11_BIND_VERTEX_BUFFER,	// 이 버퍼의 사용목적(버텍스 버퍼로 바인딩 한다.)
//		D3D11_USAGE_DYNAMIC,	// 버퍼 사용법(GPU읽기, GPU쓰기)
//		D3D10_CPU_ACCESS_WRITE	// CPU가 이 값을 읽고 쓸 수 있는지 지정(DYNAMIC버퍼로 만들었으므로 CPU가 값을 쓸 수 있어야 함.)
//	);
//
//	// 버텍스 버퍼를 생성한다.
//	mspDevice->CreateBuffer(&bd, nullptr, 
//		mspVertexBuffer.ReleaseAndGetAddressOf());
//	
//	// 넘겨준 리소스를 매핑한다 -> GPU가 읽을 수 없음.
//	D3D11_MAPPED_SUBRESOURCE ms;	// 매핑된 서브 리소스(전체 리소스의 일부분)에 대한 구조체
//	mspDeviceContext->Map(
//		mspVertexBuffer.Get(),	// 매핑할 리소스(여기서는 버텍스 버퍼)
//		0,	// 서브 리소스 인덱스(통째로 데이터를 넘겨주므로 0을 지정함)
//		D3D11_MAP_WRITE_DISCARD,	// 매핑 방식(값을 쓰고 이전 값은 버리라는 의미)
//		0,	// 매핑 옵션
//		&ms
//	);
//
//	// 버퍼에 메모리에 복사한다.
//	memcpy(ms.pData, vertices, sizeof(vertices));
//
//	// UNMAP한다. -> GPU가 읽을 수 있음
//	mspDeviceContext->Unmap(mspVertexBuffer.Get(), 0);
//}

void DrawTriangle::InitRectangle()
{
	// 1. 화면에 그릴 사각형을 초기화 한다.
	VERTEX vertices[]
	{
		{-0.45f, -0.5f, 0.0f, 0.0f, 1.0f},
		{-0.45f, 0.5f, 0.0f,0.0f, 0.0f },
		{0.45f,-0.5f, 0.0f, 1.0f,1.0f},
		{0.45f,0.5f, 0.0f, 1.0f,0.0f}
	};

	// 그림 뒤집기
	/*std::swap(vertices[0].u, vertices[1].u);
	std::swap(vertices[0].v, vertices[1].v);
	std::swap(vertices[2].u, vertices[3].u);
	std::swap(vertices[2].v, vertices[3].v);*/
	

	// 2. 버퍼에 대한 설명 구초체
	CD3D11_BUFFER_DESC bd(
		sizeof(vertices),	// 버퍼 크기
		D3D11_BIND_VERTEX_BUFFER,	// 이 버퍼의 사용목적(버텍스 버퍼로 바인딩 한다.)
		D3D11_USAGE_DYNAMIC,	// 버퍼 사용법(GPU읽기, GPU쓰기)
		D3D11_CPU_ACCESS_WRITE	// CPU가 이 값을 읽고 쓸 수 있는지 지정(DYNAMIC버퍼로 만들었으므로 CPU가 값을 쓸 수 있어야 함.)
	);

	// 3. 버텍스 버퍼를 생성한다.
	mspDevice->CreateBuffer(&bd, nullptr,
		mspVertexBuffer.ReleaseAndGetAddressOf());

	// 4. 버텍스 정보를 버퍼에 넘겨준다.

	// 4.1. 매핑한다 -> GPU가 읽을 수 없음.
	D3D11_MAPPED_SUBRESOURCE ms;	// 매핑된 서브 리소스(전체 리소스의 일부분)에 대한 구조체
	mspDeviceContext->Map(
		mspVertexBuffer.Get(),	// 매핑할 리소스
		0,	// 서브 리소스 인덱스(통째로 데이터를 넘겨주므로 0을 지정함)
		D3D11_MAP_WRITE_DISCARD,	// 매핑 방식(값을 쓰고 이전 값은 버리라는 의미)
		0,	// 매핑 옵션
		&ms
	);

	// 4.2. 버퍼에 메모리에 복사한다.
	memcpy(ms.pData, vertices, sizeof(vertices));

	// 4.3. UNMAP한다. -> GPU가 읽을 수 있음
	mspDeviceContext->Unmap(mspVertexBuffer.Get(), 0);

	// 상수 버퍼 생성
	bd = CD3D11_BUFFER_DESC(
		sizeof(MatrixBuffer),	
		D3D11_BIND_CONSTANT_BUFFER, // 상수 버퍼로 연결
		D3D11_USAGE_DEFAULT	// GPU만 읽고 사용
	);
	mspDevice->CreateBuffer(&bd, nullptr, mspConstantBuffer.ReleaseAndGetAddressOf());

	mX = mY = 0.0f;
	mRotation = 0.0f;
}

// 셰이더 및 파이프라인의 Input-Assembler 스테이지에 대한 내용을 초기화 한다.
void DrawTriangle::InitPipeline()
{
	// Blob : Binary Large OBject -> 임의의 크기를 가진 버퍼라고 생각할 것
	Microsoft::WRL::ComPtr<ID3DBlob> spVS;
	Microsoft::WRL::ComPtr<ID3DBlob> spPS;

	// 셰이더를 컴파일 한다.
	D3DCompileFromFile(
		L"VertexShader.hlsl",	// 파일 이름
		nullptr,	// HLSL 매크로 목록
		nullptr,	// HLSL 헤더파일 지정
		"main",		// 엔트리 포인트
		"vs_4_0_level_9_3",	// 셰이더 코드의 버전
		0,	// 추가 컴파일 옵션
		0,	// 추가 컴파일 옵션
		spVS.GetAddressOf(),	// 컴파일 성공한 바이너리 코드 출력
		nullptr	// 에러메시지지를 가진 버퍼
	);

	D3DCompileFromFile(
		L"PixelShader.hlsl",
		nullptr,
		nullptr,
		"main",
		"ps_4_0_level_9_3",
		0,
		0,
		spPS.GetAddressOf(),
		nullptr
	);

	// 컴파일한 기계어를 가지고 셰이더 인터페이스를 생성한다.
	mspDevice->CreateVertexShader(
		spVS->GetBufferPointer(), // 컴파일된 셰이더 코드 입력
		spVS->GetBufferSize(),	// 컴파일된 셰이더 코드의 바이트 크기 입력
		nullptr,	// link
		mspVertexShader.ReleaseAndGetAddressOf()	// 생성된 셰이더 인터페이스 출력
	);

	mspDevice->CreatePixelShader(
		spPS->GetBufferPointer(),
		spPS->GetBufferSize(),
		nullptr,
		mspPixelShader.ReleaseAndGetAddressOf()
	);

	// 만들어진 셰이더 인터페이스를 렌더링 파이프라인에서 지정한다.
	mspDeviceContext->VSSetShader(
		mspVertexShader.Get(),	// 설정할 셰이더 입력
		nullptr, // 클래스 인터페이스 배열 입력
		0	// 클래스 인스턴스 개수입력
	);

	mspDeviceContext->PSSetShader(
		mspPixelShader.Get(),
		nullptr,
		0
	);

	// Input-Assember 스테이지의 입력값이 어떤식으로 구성되어 있는 가를 지정한다.
	D3D11_INPUT_ELEMENT_DESC ied[]{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	// Input-Layout을 만든다.
	mspDevice->CreateInputLayout(
		ied,	// 인풋 엘리먼트 구조체 입력
		2,	// 엘리먼트 개수입력
		spVS->GetBufferPointer(),	// 컴파일된 셰이더 포인터 입력	
		spVS->GetBufferSize(),	// 컴파일된 셰이더 크기 입력
		mspInputLayout.ReleaseAndGetAddressOf()	// Input-Layout 인터페이스 출력
	);

	// Input-Assembler에 Input-Layout을 연결한다.
	mspDeviceContext->IASetInputLayout(mspInputLayout.Get());


	// 샘플러 상태를 설정한다.
	float border[4]{ 1.0f,0.0f,0.0f,1.0f };		// 텍스쳐 테두리 색상
	CD3D11_SAMPLER_DESC sd(
		D3D11_FILTER_MIN_MAG_MIP_POINT,	// 샘플링에 사용할 필터 종류지정	
		D3D11_TEXTURE_ADDRESS_WRAP,	// u좌표가 0~1밖에 있을 때 매핑 방법
		D3D11_TEXTURE_ADDRESS_WRAP,	// v좌표가 0~1밖에 있을 때 매핑 방법
		D3D11_TEXTURE_ADDRESS_WRAP,	// w좌표가 0~1밖에 있을 때 매핑 방법
		0.0f,	// 적용할 LOD레벨 편향값
		1,	// 비등방성 필터링 적용할 때 최대값
		D3D11_COMPARISON_ALWAYS,	// 기존의 샘플링된 데이터와 샘플링한데이터를 비교할 때 사용하는 함수 지정(항상 비교 전달)
		border,	// 텍스쳐 외곽선 색상지정
		0,	// LOD를 적용할 때 가장 저화질 이미지를 사용할 하한값
		0	// LOD를 적용할 때 가장 고화질 이미지를 사용할 상한값
	);
	mspDevice->CreateSamplerState(&sd, mspSamplerState.ReleaseAndGetAddressOf());

	// 블렌딩 상태를 설정한다.
	D3D11_BLEND_DESC bd;
	ZeroMemory(&bd, sizeof(D3D11_BLEND_DESC));
	bd.RenderTarget[0].BlendEnable = true;

	bd.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	bd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	// Output-Merger 스테이지에 사용할 블렌딩 상태를 생성한다.
	mspDevice->CreateBlendState(&bd, mspBlendState.ReleaseAndGetAddressOf());
}

HRESULT DrawTriangle::CreateTextureFromBMP()
{
	// 1. 파일 열기
	std::ifstream file;
	file.open("Data/32.bmp", std::ios::binary);

	BITMAPFILEHEADER bmh;
	BITMAPINFOHEADER bmi;

	// 2. BITMAPFILEHEADER 읽기
	file.read(reinterpret_cast<char*>(&bmh), sizeof(BITMAPFILEHEADER));
	// 3. BITMAPINFOGHEADER 읽기
	file.read(reinterpret_cast<char*>(&bmi), sizeof(BITMAPINFOHEADER));
	if (bmh.bfType != 0x4D42)
	{
		return E_FAIL;
	}
	if (bmi.biBitCount != 32)
	{
		return E_FAIL;
	}

	std::vector<char> pPixels(bmi.biSizeImage);
	// 4. 픽셀로 건너뛰기
	file.seekg(bmh.bfOffBits);
	// 5. 비트맵 읽기
	int pitch = bmi.biWidth * (bmi.biBitCount / 8);

	int index{};
	char r{}, g{}, b{}, a{};
	for (int y{ bmi.biHeight - 1 }; y >= 0; --y)
	{
		index = y * pitch;
		for (int x{ 0 }; x < bmi.biWidth; ++x)
		{
			file.read(&b, 1);
			file.read(&g, 1);
			file.read(&r, 1);
			file.read(&a, 1);

			if (static_cast<unsigned char>(r) == 30 &&
				static_cast<unsigned char>(g) == 199 &&
				static_cast<unsigned char>(b) == 250)
			{
				pPixels[index] = 0;
				pPixels[index + 1] = 0;
				pPixels[index + 2] = 0;
				pPixels[index + 3] = 0;
			}
			else
			{
				pPixels[index] = b;
				pPixels[index + 1] = g;
				pPixels[index + 2] = r;
				pPixels[index + 3] = a;
			}

			index += 4;
		}
	}

	file.close();

	CD3D11_TEXTURE2D_DESC td(
		DXGI_FORMAT_B8G8R8A8_UNORM,	// 비트맵은 BGRA순임
		bmi.biWidth,	// BitmapInfoHeader의 가로크기
		bmi.biHeight,	// BitmapInfoHeader의 세로크기
		1,	// 한장의 텍스
		1	// MipMap을 상세하게 표현할 필요가 없음
	);

	// 서브리소스를 초기화한다.
	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = &pPixels[0];	// 초기화 데이터에 대한 포인터
	initData.SysMemPitch = pitch;	// 너비의 바이트 단위
	initData.SysMemSlicePitch = 0;	// 다음 단계까지 바이트 거리

	// BMP파일의 픽셀로 텍스쳐를 생성한다.
	mspDevice->CreateTexture2D(
		&td,
		&initData, 
		mspTexture.ReleaseAndGetAddressOf()
	);
	
	// 셰이더 리소스 뷰를 설정한다.
	CD3D11_SHADER_RESOURCE_VIEW_DESC srvd(
		D3D11_SRV_DIMENSION_TEXTURE2D,	// 2차원 텍스쳐로 지정
		td.Format,	// 텍스쳐 포멧과 동일하게 지정
		0,	// 버퍼의 첫번재 원소
		1	// 버퍼의 원소 개수(텍스쳐를 생성할 때 원소와 같아야 한다.)
	);

	// 셰이더리소스 데이터에 접근하기위한 셰이더 리소스 뷰를 생성한다.
	mspDevice->CreateShaderResourceView(
		mspTexture.Get(),
		&srvd,
		mspTextureView.ReleaseAndGetAddressOf()
	);

	return S_OK;
}
