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

    // 셰이더의 상수 버퍼로 넘겨 줄 자료 형식(이 구조체를 ID3DBuffer 형태로 변환해 셰이더에 넘겨준다.)
    struct MatrixBuffer
    {
        DirectX::XMMATRIX world;    // 월드 변환을 저장해 둘 행렬변수
    };

    Microsoft::WRL::ComPtr<ID3D11InputLayout> mspInputLayout;    // 버텍스 데이터를 Input-Assember에 전달하는 방법에 대한 정의
    Microsoft::WRL::ComPtr<ID3D11Buffer> mspVertexBuffer;   // 실제 버텍스 정보를 담고 있는 버퍼
    Microsoft::WRL::ComPtr<ID3D11VertexShader> mspVertexShader; // 버텍스 셰이더 인터페이스
    Microsoft::WRL::ComPtr<ID3D11PixelShader> mspPixelShader;   // 픽셀 셰이더 인터페이스

    Microsoft::WRL::ComPtr<ID3D11Texture2D> mspTexture;    // 텍스쳐 인터페이스
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mspTextureView;    // 셰이더 리소스 뷰 인터페이스

    Microsoft::WRL::ComPtr<ID3D11SamplerState> mspSamplerState;     // 샘플러 인터페이스

    Microsoft::WRL::ComPtr<ID3D11BlendState> mspBlendState; // 블렌드 상태 인터페이스

    Microsoft::WRL::ComPtr<ID3D11Buffer> mspConstantBuffer; // 셰이더에 전달할 버퍼 인터페이스
    
    // 위치,회전 변환 정보 저장
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

