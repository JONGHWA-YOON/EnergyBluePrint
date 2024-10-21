#pragma once

//***************************************************************************************
// Effects.h by Frank Luna (C) 2011 All Rights Reserved.
//
// Defines lightweight effect wrappers to group an effect and its variables.
// Also defines a static Effects class from which we can access all of our effects.
//***************************************************************************************


#include <array>
#include <concepts>
#include <wrl.h>
#include "d3dUtil.h"
#include "d3dx11Effect.h"	// effect, tech

template<typename Container, typename ValueType>
concept ContiguousRange =
std::ranges::range<Container> &&
std::is_same_v<std::ranges::range_value_t<Container>, ValueType>&&
std::contiguous_iterator<std::ranges::iterator_t<Container>>;

class PosEffect;

#pragma region Effect
class Effect
{
public:
    Effect(ID3D11Device* device, const std::wstring& filename)
    {
        std::ifstream fin(filename, std::ios::binary);

        fin.seekg(0, std::ios_base::end);
        int size = (int)fin.tellg();
        fin.seekg(0, std::ios_base::beg);
        std::vector<char> compiledShader(size);

        fin.read(&compiledShader[0], size);
        fin.close();

        HR(D3DX11CreateEffectFromMemory(&compiledShader[0], size,
            0, device, mFX.GetAddressOf()));
    }
protected:
    Microsoft::WRL::ComPtr<ID3DX11Effect> mFX;
};
#pragma endregion

#pragma region BasicEffect
class BasicEffect : public Effect
{
public:
    BasicEffect(ID3D11Device* device, const std::wstring& filename) : Effect(device, filename)
    {
        Light1Tech = mFX->GetTechniqueByName("Light1");
        Light2Tech = mFX->GetTechniqueByName("Light2");
        Light3Tech = mFX->GetTechniqueByName("Light3");
        WorldViewProj = mFX->GetVariableByName("gWorldViewProj")->AsMatrix();
        World = mFX->GetVariableByName("gWorld")->AsMatrix();
        WorldInvTranspose = mFX->GetVariableByName("gWorldInvTranspose")->AsMatrix();
        EyePosW = mFX->GetVariableByName("gEyePosW")->AsVector();
        DirLights = mFX->GetVariableByName("gDirLights");
        PointLights = mFX->GetVariableByName("gPointLights");
        SpotLights = mFX->GetVariableByName("gSpotLights");
        Mat = mFX->GetVariableByName("gMaterial");

        /// 텍스쳐
        Light0TexTech = mFX->GetTechniqueByName("Light0Tex");
        Light1TexTech = mFX->GetTechniqueByName("Light1Tex");
        Light2TexTech = mFX->GetTechniqueByName("Light2Tex");
        Light3TexTech = mFX->GetTechniqueByName("Light3Tex");

        TexTransform = mFX->GetVariableByName("gTexTransform")->AsMatrix();
        DiffuseMap = mFX->GetVariableByName("gDiffuseMap")->AsShaderResource();
    }
    virtual ~BasicEffect() {}

    void SetWorldViewProj(CXMMATRIX M) { WorldViewProj->SetMatrix(reinterpret_cast<const float*>(&M)); }
    void SetWorld(CXMMATRIX M) { World->SetMatrix(reinterpret_cast<const float*>(&M)); }
    void SetWorldInvTranspose(CXMMATRIX M) { WorldInvTranspose->SetMatrix(reinterpret_cast<const float*>(&M)); }
    void SetEyePosW(const XMFLOAT3& v) { EyePosW->SetRawValue(&v, 0, sizeof(XMFLOAT3)); }
    void SetDirLights(const DirectionalLight* lights) { DirLights->SetRawValue(lights, 0, 3 * sizeof(DirectionalLight)); }
    void SetPointLights(const PointLight* lights) { PointLights->SetRawValue(lights, 0, 3 * sizeof(PointLight)); }
    void SetSpotLights(const SpotLight* lights) { SpotLights->SetRawValue(lights, 0, 3 * sizeof(SpotLight)); }
    void SetMaterial(const Material& mat) { Mat->SetRawValue(&mat, 0, sizeof(Material)); }

    /// 텍스쳐 추가
    void SetTexTransform(CXMMATRIX M) { TexTransform->SetMatrix(reinterpret_cast<const float*>(&M)); }
    void SetDiffuseMap(ID3D11ShaderResourceView* tex) { DiffuseMap->SetResource(tex); }


    Microsoft::WRL::ComPtr<ID3DX11EffectTechnique> Light1Tech;
    Microsoft::WRL::ComPtr<ID3DX11EffectTechnique> Light2Tech;
    Microsoft::WRL::ComPtr<ID3DX11EffectTechnique> Light3Tech;

    Microsoft::WRL::ComPtr<ID3DX11EffectMatrixVariable> WorldViewProj;
    Microsoft::WRL::ComPtr<ID3DX11EffectMatrixVariable> World;
    Microsoft::WRL::ComPtr<ID3DX11EffectMatrixVariable> WorldInvTranspose;
    Microsoft::WRL::ComPtr<ID3DX11EffectVectorVariable> EyePosW;
    Microsoft::WRL::ComPtr<ID3DX11EffectVariable> DirLights;
    Microsoft::WRL::ComPtr<ID3DX11EffectVariable> PointLights;
    Microsoft::WRL::ComPtr<ID3DX11EffectVariable> SpotLights;
    Microsoft::WRL::ComPtr<ID3DX11EffectVariable> Mat;


    /// 텍스쳐 추가
    Microsoft::WRL::ComPtr<ID3DX11EffectTechnique> Light0TexTech;
    Microsoft::WRL::ComPtr<ID3DX11EffectTechnique> Light1TexTech;
    Microsoft::WRL::ComPtr<ID3DX11EffectTechnique> Light2TexTech;
    Microsoft::WRL::ComPtr<ID3DX11EffectTechnique> Light3TexTech;

    Microsoft::WRL::ComPtr<ID3DX11EffectMatrixVariable> TexTransform;
    Microsoft::WRL::ComPtr<ID3DX11EffectShaderResourceVariable> DiffuseMap;
};
#pragma endregion

// Skinning animation을 구현하는 데에 쓰이는 Effect
class SkinnedBasicEffect : public Effect
{
public:
    SkinnedBasicEffect(ID3D11Device* device, const std::wstring& filename) : Effect(device, filename)
    {
        bones = mFX->GetVariableByName("bones")->AsMatrix();

        WorldViewProj = mFX->GetVariableByName("gWorldViewProj")->AsMatrix();
        World = mFX->GetVariableByName("gWorld")->AsMatrix();
        WorldInvTranspose = mFX->GetVariableByName("gWorldInvTranspose")->AsMatrix();
        EyePosW = mFX->GetVariableByName("gEyePosW")->AsVector();
        DirLights = mFX->GetVariableByName("gDirLights");
        PointLights = mFX->GetVariableByName("gPointLights");
        SpotLights = mFX->GetVariableByName("gSpotLights");
        Mat = mFX->GetVariableByName("gMaterial");

        /// 텍스쳐
        defaultTech = mFX->GetTechniqueByName("DefaultTech");
        deferredGeometryTech = mFX->GetTechniqueByName("DeferredGeometry");

        TexTransform = mFX->GetVariableByName("gTexTransform")->AsMatrix();
        DiffuseMap = mFX->GetVariableByName("gDiffuseMap")->AsShaderResource();
    }

    void SetWorldViewProj(CXMMATRIX M) { WorldViewProj->SetMatrix(reinterpret_cast<const float*>(&M)); }
    void SetWorld(CXMMATRIX M) { World->SetMatrix(reinterpret_cast<const float*>(&M)); }
    void SetWorldInvTranspose(CXMMATRIX M) { WorldInvTranspose->SetMatrix(reinterpret_cast<const float*>(&M)); }
    void SetEyePosW(const XMFLOAT3& v) { EyePosW->SetRawValue(&v, 0, sizeof(XMFLOAT3)); }
    void SetDirLights(const DirectionalLight* lights) { DirLights->SetRawValue(lights, 0, 3 * sizeof(DirectionalLight)); }
    void SetPointLights(const PointLight* lights) { PointLights->SetRawValue(lights, 0, 3 * sizeof(PointLight)); }
    void SetSpotLights(const SpotLight* lights) { SpotLights->SetRawValue(lights, 0, 3 * sizeof(SpotLight)); }
    void SetMaterial(const Material& mat) { Mat->SetRawValue(&mat, 0, sizeof(Material)); }
    void SetTexTransform(CXMMATRIX M) { TexTransform->SetMatrix(reinterpret_cast<const float*>(&M)); }
    void SetDiffuseMap(ID3D11ShaderResourceView* tex) { DiffuseMap->SetResource(tex); }

    // 월드 변환 행렬에 관련된 정보들
    Microsoft::WRL::ComPtr<ID3DX11EffectMatrixVariable> WorldViewProj;
    Microsoft::WRL::ComPtr<ID3DX11EffectMatrixVariable> World;
    Microsoft::WRL::ComPtr<ID3DX11EffectMatrixVariable> WorldInvTranspose;
    Microsoft::WRL::ComPtr<ID3DX11EffectVectorVariable> EyePosW;

    // 매우 레거시한 라이트 정보들
    Microsoft::WRL::ComPtr<ID3DX11EffectVariable> DirLights;
    Microsoft::WRL::ComPtr<ID3DX11EffectVariable> PointLights;
    Microsoft::WRL::ComPtr<ID3DX11EffectVariable> SpotLights;
    Microsoft::WRL::ComPtr<ID3DX11EffectVariable> Mat;

    // 텍스처 좌표 위치에 행렬곱을 일괄적으로 적용하고 싶을때 쓰이는 트랜스폼
    Microsoft::WRL::ComPtr<ID3DX11EffectMatrixVariable> TexTransform;
    // 디퓨즈 텍스처
    Microsoft::WRL::ComPtr<ID3DX11EffectShaderResourceVariable> DiffuseMap;
    // 메시 본 트랜스폼
    Microsoft::WRL::ComPtr<ID3DX11EffectMatrixVariable> bones{ nullptr };

    // 테크닉
    Microsoft::WRL::ComPtr<ID3DX11EffectTechnique> defaultTech;
    Microsoft::WRL::ComPtr<ID3DX11EffectTechnique> deferredGeometryTech;

    template<typename Container>
        requires ContiguousRange<Container, XMMATRIX>&&
        requires (const Container& c) { {c.data()}->std::same_as<const XMMATRIX*>; }&&
        requires (const Container& c) { {c.size()}->std::same_as<size_t>; }
    void SetBoneFinalMatrix(const Container& matrixContainer)
    {
        assert(matrixContainer.size() <= 96);
        bones->SetRawValue(matrixContainer.data(), 0, matrixContainer.size() * sizeof(XMMATRIX));
    }
    void SetBoneFinalMatrix2(const std::vector<XMMATRIX>& matrixContainer)
    {
        assert(matrixContainer.size() <= 96);
        //bones->SetRawValue(&matrixContainer[0], 0, matrixContainer.size() * sizeof(XMMATRIX));
        //bones->setmatrixarr(&matrixContainer[0], 0 * sizeof(XMMATRIX), sizeof(XMMATRIX));
        bones->SetMatrixArray(reinterpret_cast<const float*>(&matrixContainer.data()[0]), 0, matrixContainer.size());
    }
};
class ScreenImageEffect : public Effect
{
public:
    ScreenImageEffect(ID3D11Device* device, const std::wstring& filename) :Effect(device, filename)
    {
        imageTech = mFX->GetTechniqueByName("imageTech");
        imageTexture = mFX->GetVariableByName("imageTexture")->AsShaderResource();
    }
    virtual ~ScreenImageEffect() {};

    void SetImageTexture(ID3D11ShaderResourceView* tex) { imageTexture->SetResource(tex); }

    Microsoft::WRL::ComPtr<ID3DX11EffectTechnique> imageTech;
    Microsoft::WRL::ComPtr<ID3DX11EffectShaderResourceVariable> imageTexture;
};

#pragma region Effects
class Effects
{
public:
    static void InitAll(ID3D11Device* device);
    static void DestroyAll();

    static BasicEffect* BasicFX;
    static BasicEffect* BasicTexFX;
    static SkinnedBasicEffect* skinnedBasicTexFX;
    static ScreenImageEffect* screenImageFX;
    static PosEffect* Pos;
};
#pragma endregion
