#pragma once
#include "D3DFramework.h"
#include <d3dcompiler.h>
#include <DirectXMath.h>

class DrawTriangle : public D3DFramework
{
    struct VERTEX
    {
        FLOAT x, y, z;  // position
        FLOAT u, v; // texel coordinate
    };

    // ���̴��� ��� ���۷� �Ѱ� �� �ڷ� ����(�� ����ü�� ID3DBuffer ���·� ��ȯ�� ���̴��� �Ѱ��ش�.)
    struct MatrixBuffer
    {
        DirectX::XMMATRIX world;    // ���� ��ȯ�� ������ �� ��ĺ���
    };

    Microsoft::WRL::ComPtr<ID3D11InputLayout> mspInputLayout;    // ���ؽ� �����͸� Input-Assember�� �����ϴ� ����� ���� ����
    Microsoft::WRL::ComPtr<ID3D11Buffer> mspVertexBuffer;   // ���� ���ؽ� ������ ��� �ִ� ����
    Microsoft::WRL::ComPtr<ID3D11VertexShader> mspVertexShader; // ���ؽ� ���̴� �������̽�
    Microsoft::WRL::ComPtr<ID3D11PixelShader> mspPixelShader;   // �ȼ� ���̴� �������̽�

    Microsoft::WRL::ComPtr<ID3D11Texture2D> mspTexture;    // �ؽ��� �������̽�
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mspTextureView;    // ���̴� ���ҽ� �� �������̽�

    Microsoft::WRL::ComPtr<ID3D11SamplerState> mspSamplerState;     // ���÷� �������̽�

    Microsoft::WRL::ComPtr<ID3D11BlendState> mspBlendState; // ���� ���� �������̽�

    Microsoft::WRL::ComPtr<ID3D11Buffer> mspConstantBuffer; // ���̴��� ������ ���� �������̽�
    
    // ��ġ,ȸ�� ��ȯ ���� ����
    float mX, mY;
    float mRotation;
    DirectX::XMMATRIX mWorld;

public:
    void Initialize(HINSTANCE hInstance, 
        int width = 800,int height = 600) override;
    void Destroy() override;

protected:
    void Render() override;
    void Update(float delta) override;

private:
    void InitTriangle();
    void InitRectangle();
    void InitPipeline();

    HRESULT CreateTextureFromBMP();
};

