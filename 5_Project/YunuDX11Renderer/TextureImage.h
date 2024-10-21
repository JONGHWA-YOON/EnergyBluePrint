#pragma once

#include <map>
#include <wrl.h>
#include "d3dUtil.h"
#include "d3dx11Effect.h"	// effect, tech
#include "Vertex.h"
#include "InputLayout.h"
#include "Effects.h"
#include "../Yunu3DLib/ASEParser.h"
#include "../Yunu3DLib/Animation.h"
#include "../Yunu3DLib/ParsingDataClass.h"


class Camera;

namespace render_object
{
    class TextureImage
    {
    public:
        TextureImage(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, ID3D11RasterizerState* rs)
            : m_pRenderstate(rs)
        {
            md3dDevice = pDevice;
            md3dDeviceContext = pDeviceContext;
            SetRect(1, -1, -1, 1);
            mTech = Effects::screenImageFX->imageTech;
            D3DX11_PASS_DESC passDesc;
            mTech->GetPassByIndex(0)->GetDesc(&passDesc);
            HR(md3dDevice->CreateInputLayout(InputLayoutDesc::ScreenImage, 2, passDesc.pIAInputSignature,
                passDesc.IAInputSignatureSize, mInputLayout.GetAddressOf()));
        }
        ~TextureImage()
        {
        }

    public:
        //void LoadImageTexture(const wstring& filename);
        void LoadImageTexture(ID3D11ShaderResourceView* textureSRV)
        {
            mImageTextureSRV = textureSRV;
        }
        void SetRect(float top, float bottom, float left, float right)
        {
            UINT ByteWidth;
            D3D11_USAGE Usage;
            UINT BindFlags;
            UINT CPUAccessFlags;
            UINT MiscFlags;
            UINT StructureByteStride;
            VertexStruct::ScreenImage vertices[4]
            {
                {.Pos = {left,bottom,0},.Tex = {0,1}},
                {.Pos = {right,bottom,0},.Tex = {1,1}},
                {.Pos = {left,top,0},.Tex = {0,0}},
                {.Pos = {right,top,0},.Tex = {1,0}},
            };
            D3D11_BUFFER_DESC vbd
            {
                    .ByteWidth = sizeof(VertexStruct::ScreenImage) * 4,
                    .Usage = D3D11_USAGE_IMMUTABLE,
                    .BindFlags = D3D11_BIND_VERTEX_BUFFER,
                    .CPUAccessFlags = 0,
                    .MiscFlags = 0,
            };
            D3D11_SUBRESOURCE_DATA vinitData;
            vinitData.pSysMem = &vertices[0];
            HR(md3dDevice->CreateBuffer(&vbd, &vinitData, mVB.GetAddressOf()));
            UINT indices[6]{ 0,2,1,1,2,3 };
            D3D11_BUFFER_DESC ibd
            {
                    .ByteWidth = sizeof(UINT) * 6,
                    .Usage = D3D11_USAGE_IMMUTABLE,
                    .BindFlags = D3D11_BIND_INDEX_BUFFER,
                    .CPUAccessFlags = 0,
                    .MiscFlags = 0,
            };
            D3D11_SUBRESOURCE_DATA iinitData;
            iinitData.pSysMem = &indices[0];
            HR(md3dDevice->CreateBuffer(&ibd, &iinitData, mIB.GetAddressOf()));
        }
        void RenderToTexture()
        {
            constexpr UINT vertexStride{ sizeof(VertexStruct::ScreenImage) };
            constexpr UINT vertexOffset{ 0 };
            constexpr UINT indexStride{ sizeof(UINT) };
            constexpr UINT indexOffset{ 0 };
            Effects::screenImageFX->SetImageTexture(mImageTextureSRV.Get());
            md3dDeviceContext->IASetInputLayout(mInputLayout.Get());
            md3dDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
            md3dDeviceContext->RSSetState(m_pRenderstate.Get());
            md3dDeviceContext->IASetVertexBuffers(0, 1, mVB.GetAddressOf(), &vertexStride, &vertexOffset);
            md3dDeviceContext->IASetIndexBuffer(mIB.Get(), DXGI_FORMAT_R32_UINT, 0);
            mTech->GetPassByIndex(0)->Apply(0, md3dDeviceContext.Get());
            md3dDeviceContext->DrawIndexed(6, 0, 0);
        }
    private:
        Microsoft::WRL::ComPtr<ID3D11InputLayout> mInputLayout;
        Microsoft::WRL::ComPtr<ID3DX11EffectTechnique> mTech;
        Microsoft::WRL::ComPtr<ID3D11Device> md3dDevice;						// D3D11 디바이스
        Microsoft::WRL::ComPtr<ID3D11DeviceContext> md3dDeviceContext;		// 디바이스 컨텍스트

        Microsoft::WRL::ComPtr<ID3D11Buffer> mVB;
        Microsoft::WRL::ComPtr<ID3D11Buffer> mIB;

        Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_pRenderstate;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mImageTextureSRV;
    };
}
