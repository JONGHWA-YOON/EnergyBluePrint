#pragma once

#include "IResource.h"

class Mesh;
class Material;
class Shader;
class Texture;
class MeshData;
class StructuredBuffer;
class InstancingBuffer;

class Device;
class GraphicsCommandQueue;
class ComputeCommandQueue;
class RootSignature;
class GraphicsDescriptorHeap;
class ComputeDescriptorHeap;
class ConstantBuffer;
class RenderTargetGroup;


class ResourceManager
{
private:
	ResourceManager() {};
	~ResourceManager();

public:
	static ResourceManager* GetInstance()
	{
		static ResourceManager inst;
		return &inst;
	}

	void Init(Device* p_device, GraphicsCommandQueue* p_graphicsCommadnQueue, ComputeCommandQueue* p_computeCommandQueue, RootSignature* p_rootSignature
		, GraphicsDescriptorHeap* p_graphicsDescriptorHeap, ComputeDescriptorHeap* p_computeDescriptorHeap, vector<ConstantBuffer*>* p_constantBuffers, 
		array<RenderTargetGroup*, RENDER_TARGET_TYPE_COUNT>* p_renderTargets);

	void CrateDefaultResource();

	Mesh* GetMesh(wstring p_key);
	Material* GetMaterial(MATERIAL_DESC& p_materialDesc, wstring& p_materialName);
	Material* GetMaterial(const wstring& p_name);
	Shader* GetShader(wstring p_key);
	Texture* GetTexture(wstring p_key);
	StructuredBuffer* GetStructuredBuffer(uint32 p_id);
	FFMeshData& GetMeshDataFromMeshName(const wstring& p_meshName);

	void CreateStructureBuffer(uint32 p_id, uint32 p_size, uint32 p_count);
	map<uint32, InstancingBuffer*>& GetMapInstancingBuffer() { return m_mapInstancingBuffer; }
	void ClearInstancingBuffer();

	Texture* CreateTexture(const wstring& name, DXGI_FORMAT format, uint32 width, uint32 height,
		const D3D12_HEAP_PROPERTIES& heapProperty, D3D12_HEAP_FLAGS heapFlags,
		D3D12_RESOURCE_FLAGS resFlags = D3D12_RESOURCE_FLAG_NONE, FFVec4 clearColor = FFVec4());

	Texture* CreateTextureFromResource(const wstring& name, ComPtr<ID3D12Resource> tex2D);

public:
	void CreateMesh(const wstring& p_key);
	void CreateShader(const wstring& p_key, const ShaderInfo& p_info);
	void CreateTexture(const wstring& p_key);
	void CreateMaterial(const wstring& p_key, const FFMATERIAL_DESC& p_materialDesc);
	void LoadFBX(const wstring& p_key, const wstring& p_shaderPath);

	void AddMesh(const wstring& p_key, Mesh* p_mesh);
	void AddMaterial(const wstring& p_key, Material* p_material);

	void CheckUsingMaterial();

	void ClearRefCount();
	void AddRefCount(const wstring& p_materialName);
	void SetMaterialDescForMapMaterial(MATERIAL_DESC& p_desc, const wstring& p_materialName);

	vector<FFMeshData>& GetMeshDataFromFBX(const wstring& p_path);

	void PushAnimationData(unsigned __int64 p_id, __int32 p_frame, __int32 p_nextFrame, float p_frameRatio);
	pair<Material*, StructuredBuffer*>& GetAnimationData(const unsigned __int64 p_id)
	{
		auto iter = m_mapAnimatorData.find(p_id);
		if (iter != m_mapAnimatorData.end())
		{
			return m_mapAnimatorData[p_id];
		}
	}
		
private:
	//template<typename Type>
	//RESOURCE_TYPE GetResourceType();
	//
	//template<typename Type>
	//Type* Get(const wstring& p_key);
	//
	//template<typename Type>
	//Type* Create(const wstring& p_key);

	Mesh* LoadCubeMesh();
	Mesh* LoadSphereMesh();
	Mesh* LoadRactangleMesh();
	Mesh* LoadPointMesh();
	Mesh* LoadLineMesh();

	void CreateDefaultShader();
	void CreateDefaultMaterial();

private:
	//array<unordered_map<wstring, IResource*>, RESOURCE_TYPE_COUNT> m_mapResources;

	map<wstring, Mesh*> m_mapMesh;

	map<wstring, Material*> m_mapMaterial;
	map<wstring, Material*> m_mapNewMaterial;

	multimap<wstring, Shader*> m_mapShader;
	map<wstring, Texture*> m_mapTexture;

	map<wstring, MeshData*> m_mapMeshData;
	map<wstring, vector<FFMeshData>> m_mapFFMeshData;
	map<wstring, FFMeshData> m_mapMeshNameForData;

	map<uint64, pair<Material*, StructuredBuffer*>> m_mapAnimatorData;

	map<uint32,StructuredBuffer*> m_mapStructuredBuffer;
	map<uint32,InstancingBuffer*> m_mapInstancingBuffer;

private:
	// 참조용 변수들
	Device* m_device;
	GraphicsCommandQueue* m_graphicsCommandQueue;
	ComputeCommandQueue* m_computeCommandQueue;
	RootSignature* m_rootSignature;
	GraphicsDescriptorHeap* m_graphicsDescriptorHeap;
	ComputeDescriptorHeap* m_computeDescriptorHeap;
	vector<ConstantBuffer*>* m_constantBuffers;
	array<RenderTargetGroup*, RENDER_TARGET_TYPE_COUNT>* m_renderTargets;

public:
	GraphicsCommandQueue* GetGraphicsCmdQueue() { return  m_graphicsCommandQueue; }
	GraphicsDescriptorHeap* GetGraphicsDescriptorHeap() { return m_graphicsDescriptorHeap; }
	ComputeDescriptorHeap* GetComputeDescriptorHeap() { return m_computeDescriptorHeap; }
	ComputeCommandQueue* GetComputeCommandQueue() { return m_computeCommandQueue; }

	map<uint64, pair<Material*, StructuredBuffer*>>& GetAnimData() { return m_mapAnimatorData; }
};


//template<typename Type>
//inline RESOURCE_TYPE ResourceManager::GetResourceType()
//{
//	if (is_same_v<Mesh, Type>) return RESOURCE_TYPE::MESH;
//	else if (is_same_v<Shader, Type>) return RESOURCE_TYPE::SHADER;
//	else if (is_same_v<Texture, Type>) return RESOURCE_TYPE::TEXTURE;
//	else if (is_same_v<Material, Type>) return RESOURCE_TYPE::MATERIAL;
//}

//template<typename Type>
//Type* ResourceManager::Get(const wstring& p_key)
//{
//	RESOURCE_TYPE type = GetResourceType<Type>();
//	auto& mapResources = m_mapResources[static_cast<uint8>(type)];
//	auto iter = mapResources.find(p_key);
//	if (iter != mapResources.end())
//	{
//		return static_cast<Type*>(iter->second);
//	}
//
//	return nullptr;
//	//return Create<Type>(p_key);
//}
//
//template<typename Type>
//Type* ResourceManager::Create(const wstring& p_key)
//{
//	RESOURCE_TYPE type = GetResourceType<Type>();
//
//	Type* newResource = new Type();
//
//}

