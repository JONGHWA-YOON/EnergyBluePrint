#pragma once

#include "IResource.h"

class Device;
class ConstantBuffer;
class GraphicsDescriptorHeap;
class ComputeDescriptorHeap;
class ComputeCommandQueue;

class Shader;
class Texture;

enum
{
	MATERIAL_INT_COUNT = 4,
	MATERIAL_FLOAT_COUNT = 4,
	MATERIAL_TEXTURE_COUNT = 4,
	MATERIAL_VECTOR2_COUNT = 4,
	MATERIAL_VECTOR4_COUNT = 4,
};

struct MaterialParams
{
	MaterialParams()
	{
		for (int32 i = 0; i < 4; i++)
		{
			SetInt(i, 0);
			SetFloat(i, 0.f);
			SetTextureOn(i, 0);
		}
	}

	void SetInt(uint8 p_index, int32 p_value) { intParams[p_index] = p_value; }
	void SetFloat(uint8 p_index, float p_value) { floatParams[p_index] = p_value; }
	void SetTextureOn(uint8 p_index, int32 p_value) { texOnParams[p_index] = p_value; }
	void SetVec2(uint8 p_index, Vec2 p_value) { vec2Params[p_index] = p_value; }
	void SetVec4(uint8 p_index, Vec4 p_value) { vec4Params[p_index] = p_value; }
	void SetColor(Vec4 p_color) { color = p_color; }

	array<int32, MATERIAL_INT_COUNT> intParams;
	array<float, MATERIAL_FLOAT_COUNT> floatParams;
	array<int32, MATERIAL_TEXTURE_COUNT> texOnParams;
	array<Vec2, MATERIAL_VECTOR2_COUNT > vec2Params;
	array<Vec4, MATERIAL_VECTOR4_COUNT > vec4Params;
	Vec4 color = {1.f,1.f,1.f,1.f};
};

/// <summary>
/// Mesh에 적용될 Material을 정의 하는 클래스
/// 내부로 텍스처와 셰이더가 있음
/// </summary>
class Material : public IResource
{
public:
	Material(Device* p_device, vector<ConstantBuffer*>* p_constantBuffers, GraphicsDescriptorHeap* p_graphicsDescriptorHeap, ComputeDescriptorHeap* p_computeDescriptorHeap, ComputeCommandQueue* p_computeCommandQueue);
	virtual ~Material();


	Shader* GetShader() { return m_shader; }

#pragma region Setter
	void SetMaterialDesc(const MATERIAL_DESC& p_materialDesc) { m_materialDesc = p_materialDesc; }
	void SetMaterialPramsForDesc(const MATERIAL_DESC& p_materialDesc);
	void SetName(const wstring& p_name)
	{
		m_name = p_name; 
		m_materialDesc.materialName = p_name;
	}
	void SetShader(Shader* p_shader) { m_shader = p_shader; }
	void SetInt(uint8 p_index, int32 p_value) { m_params.SetInt(p_index, p_value); }
	void SetFloat(uint8 p_index, float p_value) { m_params.SetFloat(p_index, p_value); }
	void SetTexture(uint8 p_index, Texture* p_texture)
	{
		m_textures[p_index] = p_texture;
		m_params.SetTextureOn(p_index, (p_texture == nullptr ? 0 : 1));
	}
	void SetVec2(uint8 p_index, Vec2 p_value) { m_params.SetVec2(p_index, p_value); }
	void SetVec4(uint8 p_index, Vec4 p_value) { m_params.SetVec4(p_index, p_value); }
	void SetColor(Vec4 p_color) { m_params.SetColor(p_color); }
#pragma endregion
	wstring& GetName() { return m_name; }
	MATERIAL_DESC& GetMaterialDesc() { return m_materialDesc; }
	MaterialParams& GetMaterialParams() { return m_params; }
	array<Texture*, MATERIAL_TEXTURE_COUNT>& GetTextures() { return m_textures; }

	void PushGraphicsData();
	void PushComputeSRVData();
	void Dispatch(uint32 p_x, uint32 p_y, UINT p_z);

	void ResetRefCount() { m_refCount = 0; }
	void AddRefCount() { ++m_refCount; }
	uint64 GetRefCount() { return m_refCount; }

	void ResetMaterialDescForParams();
private:
	wstring m_name;
	Shader*	m_shader;
	MaterialParams		m_params;
	array<Texture*, MATERIAL_TEXTURE_COUNT> m_textures;

	MATERIAL_DESC m_materialDesc;

	uint64 m_refCount = 0;

private:
	// 참조용 변수들
	Device* m_device;
	vector<ConstantBuffer*>* m_constantBuffers;
	GraphicsDescriptorHeap* m_graphicsDescriptorHeap;
	ComputeDescriptorHeap* m_computeDescriptorHeap;
	ComputeCommandQueue* m_computeCommandQueue;
};

