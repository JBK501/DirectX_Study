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
	// stride : ���Ҹ� �����ϴ� ����(���⼭�� �ϳ��� ���ؽ� ������ �� ����Ʈ���� ǥ����)
	// offset : �ǳʶ� ��ġ
	UINT stride = sizeof(VERTEX);	
	UINT offset = 0;

	// Input-Assembler�� ���ؽ����۸� �����Ѵ�. (GPU�� �� ���ؽ� ���۸� �б� �����Ѵ�.)
	mspDeviceContext->IASetVertexBuffers(
		0,	// ����� ���� ��ȣ �Է�(�ϳ��� �����)
		1,	// ���ؽ� ���� ���� �Է�(�� ���� ����)
		mspVertexBuffer.GetAddressOf(),	// ���ؽ� ���� �迭 ������ �Է�
		&stride,	
		&offset
	);

	// �⺻������ �����Ѵ�.
	//mspDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	mspDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	
	// ���ؽ� ���̴� ������������ ����� ��� ���۸� �����Ѵ�.
	mspDeviceContext->VSSetConstantBuffers(0, 1, mspConstantBuffer.GetAddressOf());
	
	// ��� ���۸� �����Ѵ�.
	MatrixBuffer mb;	// ���̴��� ������ ��� ���� ����ü
	mb.world = DirectX::XMMatrixTranspose(mWorld);	// ��ġ ����� ���Ѵ�.
	mspDeviceContext->UpdateSubresource(
		mspConstantBuffer.Get(), 
		0, 
		nullptr, 
		&mb, 
		0,
		0
	);

	// �ȼ����̴��� ���ҽ��� �Ѱ��ش�.(�ڵ������� �ȼ����̴��� �ؽ��� ������Ʈ(Texture2D)�� �ؼ��ȴ�.)
	mspDeviceContext->PSSetShaderResources(
		0, 1, mspTextureView.GetAddressOf()
	);

	// ���÷��� �ȼ����̴��� �������ش�.
	mspDeviceContext->PSSetSamplers(
		0,1,mspSamplerState.GetAddressOf()
	);

	// Output-Merger���������� ���� ���¸� �����Ѵ�.
	mspDeviceContext->OMSetBlendState(
		mspBlendState.Get(), nullptr, 0xffffffff
	);

	// �׸���.
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
//	// ȭ�鿡 �׸� �ﰢ���� �ʱ�ȭ �Ѵ�.
//	VERTEX vertices[]
//	{
//		{0.0f, 0.5f, 0.0f, {1.0,0.0f,0.0f,1.0f}},
//		{0.45f, -0.5f, 0.0f, {0.0f,1.0f,0.0f,1.0f}},
//		{-0.45f,-0.5f, 0.0f, {0.0f,0.0f,1.0f,1.0f}},
//	};
//
//	// ���ۿ� ���� ���� ����ü
//	CD3D11_BUFFER_DESC bd(
//		sizeof(vertices),	// ���� ũ��
//		D3D11_BIND_VERTEX_BUFFER,	// �� ������ ������(���ؽ� ���۷� ���ε� �Ѵ�.)
//		D3D11_USAGE_DYNAMIC,	// ���� ����(GPU�б�, GPU����)
//		D3D10_CPU_ACCESS_WRITE	// CPU�� �� ���� �а� �� �� �ִ��� ����(DYNAMIC���۷� ��������Ƿ� CPU�� ���� �� �� �־�� ��.)
//	);
//
//	// ���ؽ� ���۸� �����Ѵ�.
//	mspDevice->CreateBuffer(&bd, nullptr, 
//		mspVertexBuffer.ReleaseAndGetAddressOf());
//	
//	// �Ѱ��� ���ҽ��� �����Ѵ� -> GPU�� ���� �� ����.
//	D3D11_MAPPED_SUBRESOURCE ms;	// ���ε� ���� ���ҽ�(��ü ���ҽ��� �Ϻκ�)�� ���� ����ü
//	mspDeviceContext->Map(
//		mspVertexBuffer.Get(),	// ������ ���ҽ�(���⼭�� ���ؽ� ����)
//		0,	// ���� ���ҽ� �ε���(��°�� �����͸� �Ѱ��ֹǷ� 0�� ������)
//		D3D11_MAP_WRITE_DISCARD,	// ���� ���(���� ���� ���� ���� ������� �ǹ�)
//		0,	// ���� �ɼ�
//		&ms
//	);
//
//	// ���ۿ� �޸𸮿� �����Ѵ�.
//	memcpy(ms.pData, vertices, sizeof(vertices));
//
//	// UNMAP�Ѵ�. -> GPU�� ���� �� ����
//	mspDeviceContext->Unmap(mspVertexBuffer.Get(), 0);
//}

void DrawTriangle::InitRectangle()
{
	// 1. ȭ�鿡 �׸� �簢���� �ʱ�ȭ �Ѵ�.
	VERTEX vertices[]
	{
		{-0.45f, -0.5f, 0.0f, 0.0f, 1.0f},
		{-0.45f, 0.5f, 0.0f,0.0f, 0.0f },
		{0.45f,-0.5f, 0.0f, 1.0f,1.0f},
		{0.45f,0.5f, 0.0f, 1.0f,0.0f}
	};

	// �׸� ������
	/*std::swap(vertices[0].u, vertices[1].u);
	std::swap(vertices[0].v, vertices[1].v);
	std::swap(vertices[2].u, vertices[3].u);
	std::swap(vertices[2].v, vertices[3].v);*/
	

	// 2. ���ۿ� ���� ���� ����ü
	CD3D11_BUFFER_DESC bd(
		sizeof(vertices),	// ���� ũ��
		D3D11_BIND_VERTEX_BUFFER,	// �� ������ ������(���ؽ� ���۷� ���ε� �Ѵ�.)
		D3D11_USAGE_DYNAMIC,	// ���� ����(GPU�б�, GPU����)
		D3D11_CPU_ACCESS_WRITE	// CPU�� �� ���� �а� �� �� �ִ��� ����(DYNAMIC���۷� ��������Ƿ� CPU�� ���� �� �� �־�� ��.)
	);

	// 3. ���ؽ� ���۸� �����Ѵ�.
	mspDevice->CreateBuffer(&bd, nullptr,
		mspVertexBuffer.ReleaseAndGetAddressOf());

	// 4. ���ؽ� ������ ���ۿ� �Ѱ��ش�.

	// 4.1. �����Ѵ� -> GPU�� ���� �� ����.
	D3D11_MAPPED_SUBRESOURCE ms;	// ���ε� ���� ���ҽ�(��ü ���ҽ��� �Ϻκ�)�� ���� ����ü
	mspDeviceContext->Map(
		mspVertexBuffer.Get(),	// ������ ���ҽ�
		0,	// ���� ���ҽ� �ε���(��°�� �����͸� �Ѱ��ֹǷ� 0�� ������)
		D3D11_MAP_WRITE_DISCARD,	// ���� ���(���� ���� ���� ���� ������� �ǹ�)
		0,	// ���� �ɼ�
		&ms
	);

	// 4.2. ���ۿ� �޸𸮿� �����Ѵ�.
	memcpy(ms.pData, vertices, sizeof(vertices));

	// 4.3. UNMAP�Ѵ�. -> GPU�� ���� �� ����
	mspDeviceContext->Unmap(mspVertexBuffer.Get(), 0);

	// ��� ���� ����
	bd = CD3D11_BUFFER_DESC(
		sizeof(MatrixBuffer),	
		D3D11_BIND_CONSTANT_BUFFER, // ��� ���۷� ����
		D3D11_USAGE_DEFAULT	// GPU�� �а� ���
	);
	mspDevice->CreateBuffer(&bd, nullptr, mspConstantBuffer.ReleaseAndGetAddressOf());

	mX = mY = 0.0f;
	mRotation = 0.0f;
}

// ���̴� �� ������������ Input-Assembler ���������� ���� ������ �ʱ�ȭ �Ѵ�.
void DrawTriangle::InitPipeline()
{
	// Blob : Binary Large OBject -> ������ ũ�⸦ ���� ���۶�� ������ ��
	Microsoft::WRL::ComPtr<ID3DBlob> spVS;
	Microsoft::WRL::ComPtr<ID3DBlob> spPS;

	// ���̴��� ������ �Ѵ�.
	D3DCompileFromFile(
		L"VertexShader.hlsl",	// ���� �̸�
		nullptr,	// HLSL ��ũ�� ���
		nullptr,	// HLSL ������� ����
		"main",		// ��Ʈ�� ����Ʈ
		"vs_4_0_level_9_3",	// ���̴� �ڵ��� ����
		0,	// �߰� ������ �ɼ�
		0,	// �߰� ������ �ɼ�
		spVS.GetAddressOf(),	// ������ ������ ���̳ʸ� �ڵ� ���
		nullptr	// �����޽������� ���� ����
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

	// �������� ��� ������ ���̴� �������̽��� �����Ѵ�.
	mspDevice->CreateVertexShader(
		spVS->GetBufferPointer(), // �����ϵ� ���̴� �ڵ� �Է�
		spVS->GetBufferSize(),	// �����ϵ� ���̴� �ڵ��� ����Ʈ ũ�� �Է�
		nullptr,	// link
		mspVertexShader.ReleaseAndGetAddressOf()	// ������ ���̴� �������̽� ���
	);

	mspDevice->CreatePixelShader(
		spPS->GetBufferPointer(),
		spPS->GetBufferSize(),
		nullptr,
		mspPixelShader.ReleaseAndGetAddressOf()
	);

	// ������� ���̴� �������̽��� ������ ���������ο��� �����Ѵ�.
	mspDeviceContext->VSSetShader(
		mspVertexShader.Get(),	// ������ ���̴� �Է�
		nullptr, // Ŭ���� �������̽� �迭 �Է�
		0	// Ŭ���� �ν��Ͻ� �����Է�
	);

	mspDeviceContext->PSSetShader(
		mspPixelShader.Get(),
		nullptr,
		0
	);

	// Input-Assember ���������� �Է°��� ������� �����Ǿ� �ִ� ���� �����Ѵ�.
	D3D11_INPUT_ELEMENT_DESC ied[]{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	// Input-Layout�� �����.
	mspDevice->CreateInputLayout(
		ied,	// ��ǲ ������Ʈ ����ü �Է�
		2,	// ������Ʈ �����Է�
		spVS->GetBufferPointer(),	// �����ϵ� ���̴� ������ �Է�	
		spVS->GetBufferSize(),	// �����ϵ� ���̴� ũ�� �Է�
		mspInputLayout.ReleaseAndGetAddressOf()	// Input-Layout �������̽� ���
	);

	// Input-Assembler�� Input-Layout�� �����Ѵ�.
	mspDeviceContext->IASetInputLayout(mspInputLayout.Get());


	// ���÷� ���¸� �����Ѵ�.
	float border[4]{ 1.0f,0.0f,0.0f,1.0f };		// �ؽ��� �׵θ� ����
	CD3D11_SAMPLER_DESC sd(
		D3D11_FILTER_MIN_MAG_MIP_POINT,	// ���ø��� ����� ���� ��������	
		D3D11_TEXTURE_ADDRESS_WRAP,	// u��ǥ�� 0~1�ۿ� ���� �� ���� ���
		D3D11_TEXTURE_ADDRESS_WRAP,	// v��ǥ�� 0~1�ۿ� ���� �� ���� ���
		D3D11_TEXTURE_ADDRESS_WRAP,	// w��ǥ�� 0~1�ۿ� ���� �� ���� ���
		0.0f,	// ������ LOD���� ���Ⱚ
		1,	// ���漺 ���͸� ������ �� �ִ밪
		D3D11_COMPARISON_ALWAYS,	// ������ ���ø��� �����Ϳ� ���ø��ѵ����͸� ���� �� ����ϴ� �Լ� ����(�׻� �� ����)
		border,	// �ؽ��� �ܰ��� ��������
		0,	// LOD�� ������ �� ���� ��ȭ�� �̹����� ����� ���Ѱ�
		0	// LOD�� ������ �� ���� ��ȭ�� �̹����� ����� ���Ѱ�
	);
	mspDevice->CreateSamplerState(&sd, mspSamplerState.ReleaseAndGetAddressOf());

	// ���� ���¸� �����Ѵ�.
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

	// Output-Merger ���������� ����� ���� ���¸� �����Ѵ�.
	mspDevice->CreateBlendState(&bd, mspBlendState.ReleaseAndGetAddressOf());
}

HRESULT DrawTriangle::CreateTextureFromBMP()
{
	// 1. ���� ����
	std::ifstream file;
	file.open("Data/32.bmp", std::ios::binary);

	BITMAPFILEHEADER bmh;
	BITMAPINFOHEADER bmi;

	// 2. BITMAPFILEHEADER �б�
	file.read(reinterpret_cast<char*>(&bmh), sizeof(BITMAPFILEHEADER));
	// 3. BITMAPINFOGHEADER �б�
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
	// 4. �ȼ��� �ǳʶٱ�
	file.seekg(bmh.bfOffBits);
	// 5. ��Ʈ�� �б�
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
		DXGI_FORMAT_B8G8R8A8_UNORM,	// ��Ʈ���� BGRA����
		bmi.biWidth,	// BitmapInfoHeader�� ����ũ��
		bmi.biHeight,	// BitmapInfoHeader�� ����ũ��
		1,	// ������ �ؽ�
		1	// MipMap�� ���ϰ� ǥ���� �ʿ䰡 ����
	);

	// ���긮�ҽ��� �ʱ�ȭ�Ѵ�.
	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = &pPixels[0];	// �ʱ�ȭ �����Ϳ� ���� ������
	initData.SysMemPitch = pitch;	// �ʺ��� ����Ʈ ����
	initData.SysMemSlicePitch = 0;	// ���� �ܰ���� ����Ʈ �Ÿ�

	// BMP������ �ȼ��� �ؽ��ĸ� �����Ѵ�.
	mspDevice->CreateTexture2D(
		&td,
		&initData, 
		mspTexture.ReleaseAndGetAddressOf()
	);
	
	// ���̴� ���ҽ� �並 �����Ѵ�.
	CD3D11_SHADER_RESOURCE_VIEW_DESC srvd(
		D3D11_SRV_DIMENSION_TEXTURE2D,	// 2���� �ؽ��ķ� ����
		td.Format,	// �ؽ��� ����� �����ϰ� ����
		0,	// ������ ù���� ����
		1	// ������ ���� ����(�ؽ��ĸ� ������ �� ���ҿ� ���ƾ� �Ѵ�.)
	);

	// ���̴����ҽ� �����Ϳ� �����ϱ����� ���̴� ���ҽ� �並 �����Ѵ�.
	mspDevice->CreateShaderResourceView(
		mspTexture.Get(),
		&srvd,
		mspTextureView.ReleaseAndGetAddressOf()
	);

	return S_OK;
}
