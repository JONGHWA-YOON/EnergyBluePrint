#include "GraphicsPch.h"
#include "ResourceManager.h"
#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"
#include "RenderTargetGroup.h"
//#include "FFGraphicsEngine.h"
#include "Material.h"
#include "TableDescriptorHeap.h"
#include "StructuredBuffer.h"
#include "InstancingBuffer.h"
#include "MeshData.h"

ResourceManager::~ResourceManager()
{
	for (auto mesh : m_mapMesh)
	{
		delete mesh.second;
	}

	for (auto material : m_mapMaterial)
	{
		delete material.second;
	}

	for (auto shader : m_mapShader)
	{
		delete shader.second;
	}

	for (auto texture : m_mapTexture)
	{
		delete texture.second;
	}

	for (auto structuredBuffer : m_mapStructuredBuffer)
	{
		delete structuredBuffer.second;
	}

	for (auto instancingBuffer : m_mapInstancingBuffer)
	{
		delete instancingBuffer.second;
	}

	for (auto meshData : m_mapMeshData)
	{
		delete meshData.second;
	}

	for (auto animatorData : m_mapAnimatorData)
	{
		delete animatorData.second.first;
		delete animatorData.second.second;
	}
}

void ResourceManager::Init(Device* p_device, GraphicsCommandQueue* p_graphicsCommadnQueue, ComputeCommandQueue* p_computeCommandQueue, RootSignature* p_rootSignature, GraphicsDescriptorHeap* p_graphicsDescriptorHeap, ComputeDescriptorHeap* p_computeDescriptorHeap, vector<ConstantBuffer*>* p_constantBuffers, array<RenderTargetGroup*, RENDER_TARGET_TYPE_COUNT>* p_renderTargets)
{
	m_device = p_device;
	m_graphicsDescriptorHeap = p_graphicsDescriptorHeap;
	m_graphicsCommandQueue = p_graphicsCommadnQueue;
	m_computeDescriptorHeap = p_computeDescriptorHeap;
	m_computeCommandQueue = p_computeCommandQueue;
	m_rootSignature = p_rootSignature;
	m_constantBuffers = p_constantBuffers;
	m_renderTargets = p_renderTargets;
}

void ResourceManager::CrateDefaultResource()
{
	CreateDefaultShader();
	CreateDefaultMaterial();
}

Mesh* ResourceManager::GetMesh(wstring p_key)
{
	auto findIt = m_mapMesh.find(p_key);
	if (findIt != m_mapMesh.end())
	{
		return findIt->second;
	}

	return nullptr;
}

Material* ResourceManager::GetMaterial(MATERIAL_DESC& p_materialDesc, wstring& p_materialName)
{
	// CreateMaterial 했을 때 FFMATERIAL_DESC를 map에 넣음
	// 그리고 실질적인 Material을 동적할당까지하고 map에 넣음
	// 게임엔진에 제공하는 Draw시리즈들에 항상 인자로 FFMATERIAL_DESC가 넘어옴
	// 그래서 Render할 때 Material정보를 가져오기 위해 이 함수가 불림
	// 이 함수의 인자로 넘어온 MATERIAL_DESC는 이번 프레임에 해당하는 Material정보임
	// 그렇기에 이전 프레임에 사용한 Material정보와 데이터가 다르다면 이번 프레임에 그릴 때
	// 인자로 넘어온 MATERIAL_DESC를 사용해서 그려야함
	// 그리고 Material정보가 바뀌었기 때문에 게임엔진에서 사용하는 FFMATERIAL_DESC도 갱신 해주어야하며 (포인터를 가지고 있기에 값 변경이 가능함),
	// 갱신해 줄 때 MaterialName을 랜덤하게 셋팅해줘야한다. 

	auto findIt = m_mapMaterial.find(p_materialName);
	if (findIt != m_mapMaterial.end())
	{
		if (!((findIt->second)->GetMaterialDesc() == p_materialDesc))
		{
			// 이전 프레임과 이번 프레임의 Material정보가 다를 경우

			// 1. 게임엔진에서 사용하는 FFMATERILA_DESC의 name을 랜덤하게 셋팅한 이름으로 바꾼다. 
			// (어차피 값이 변경된 상태로 넘어오기 때문에 그래픽스 엔진에서 따로 바꿀 필요는 없다.

			Material* newMaterial = new Material(m_device, m_constantBuffers, m_graphicsDescriptorHeap, m_computeDescriptorHeap, m_computeCommandQueue);
			p_materialName += (L" instance " + to_wstring(newMaterial->GetID()));
			p_materialDesc.materialName = p_materialName;
			newMaterial->SetMaterialPramsForDesc(p_materialDesc);
			newMaterial->SetName(p_materialDesc.materialName);
			newMaterial->AddRefCount();

			m_mapNewMaterial.insert({ p_materialDesc.materialName , newMaterial });

			return newMaterial;
		}
		return findIt->second;
	}

	auto findIt2 = m_mapNewMaterial.find(p_materialName);
	if (findIt2 != m_mapNewMaterial.end())
	{
		if (!((findIt2->second)->GetMaterialDesc() == p_materialDesc))
		{
			findIt2->second->SetMaterialPramsForDesc(p_materialDesc);

			return findIt2->second;
		}
		return findIt2->second;
	}

	return nullptr;
}

Material* ResourceManager::GetMaterial(const wstring& p_name)
{
	auto findIt = m_mapMaterial.find(p_name);
	if (findIt != m_mapMaterial.end())
	{
		return findIt->second;
	}

	return nullptr;
}

Shader* ResourceManager::GetShader(wstring p_key)
{
	auto findIt = m_mapShader.find(p_key);
	if (findIt != m_mapShader.end())
	{
		return findIt->second;
	}

	return nullptr;
}

Texture* ResourceManager::GetTexture(wstring p_key)
{
	auto findIt = m_mapTexture.find(p_key);
	if (findIt != m_mapTexture.end())
	{
		return findIt->second;
	}

	if (p_key == L"D_Position")
	{
		return (*m_renderTargets)[static_cast<uint8>(RENDER_TARGET_TYPE::G_BUFFER)]->GetRTTexture(static_cast<uint32>(POSITION));
	}
	else if (p_key == L"D_Normal")
	{
		return (*m_renderTargets)[static_cast<uint8>(RENDER_TARGET_TYPE::G_BUFFER)]->GetRTTexture(static_cast<uint32>(NORMAL));
	}
	else if (p_key == L"D_Color")
	{
		return (*m_renderTargets)[static_cast<uint8>(RENDER_TARGET_TYPE::G_BUFFER)]->GetRTTexture(static_cast<uint32>(COLOR));
	}
	else if (p_key == L"D_DiffuseLight")
	{
		return (*m_renderTargets)[static_cast<uint8>(RENDER_TARGET_TYPE::LIGHTING)]->GetRTTexture(static_cast<uint32>(DIFFUSELIGHT));
	}
	else if (p_key == L"D_SpecularLight")
	{
		return (*m_renderTargets)[static_cast<uint8>(RENDER_TARGET_TYPE::LIGHTING)]->GetRTTexture(static_cast<uint32>(SPECULARLIGHT));
	}

	return nullptr;
}

StructuredBuffer* ResourceManager::GetStructuredBuffer(uint32 p_id)
{
	auto findIt = m_mapStructuredBuffer.find(p_id);
	return findIt->second;
}

FF::FFMeshData& ResourceManager::GetMeshDataFromMeshName(const wstring& p_meshName)
{
	auto iter = m_mapMeshNameForData.find(p_meshName);

	if (iter != m_mapMeshNameForData.end())
	{
		return iter->second;
	}

	FFMeshData data = {}; 
	return data;
}

void ResourceManager::CreateStructureBuffer(uint32 p_id, uint32 p_size, uint32 p_count)
{
	StructuredBuffer* buffer = new StructuredBuffer();
	buffer->Init(p_size, p_count, m_device, m_graphicsDescriptorHeap, m_computeDescriptorHeap);
	m_mapStructuredBuffer.insert({ p_id,buffer });
}

void ResourceManager::ClearInstancingBuffer()
{
	for (auto& iter : m_mapInstancingBuffer)
	{
		iter.second->Clear();
	}
}

Texture* ResourceManager::CreateTexture(const wstring& name, DXGI_FORMAT format, uint32 width, uint32 height, const D3D12_HEAP_PROPERTIES& heapProperty, D3D12_HEAP_FLAGS heapFlags, D3D12_RESOURCE_FLAGS resFlags, FFVec4 clearColor)
{
	Texture* texture = new Texture();
	texture->Create(m_device, m_graphicsCommandQueue, format, width, height, heapProperty, heapFlags, resFlags, clearColor);
	m_mapTexture.insert({ name,texture });

	return texture;
}

void ResourceManager::CreateMaterial(const wstring& p_key, const FFMATERIAL_DESC& p_materialDesc)
{
	MATERIAL_DESC _desc = {};
	_desc.ChangeFromFFMATERIAL_DESC(p_materialDesc);

	Material* material = new Material(m_device, m_constantBuffers, m_graphicsDescriptorHeap, m_computeDescriptorHeap, m_computeCommandQueue);
	material->SetMaterialPramsForDesc(_desc);
	m_mapMaterial.insert({ p_key, material });
}

void ResourceManager::LoadFBX(const wstring& p_key, const wstring& p_shaderPath)
{
	auto iter = m_mapMeshData.find(p_key);
	if (iter != m_mapMeshData.end())
		return;

	MeshData* meshData = new MeshData(m_device, m_constantBuffers, m_graphicsDescriptorHeap, m_computeDescriptorHeap, m_graphicsCommandQueue,m_computeCommandQueue);
	meshData->LoadFromFBX(p_key, p_shaderPath);
	meshData->SetName(p_key);
	m_mapMeshData.insert({ p_key, meshData });

	vector<MeshRenderInfo>& meshRenderInfo =  meshData->GetMeshRenderInfo();
	vector<AnimClipInfo>& animClipInfo = meshData->GetAnimClip();

	vector<FFMeshData> vecFFMeshData = {};
	for (auto& info : meshRenderInfo)
	{
		FFMeshData ffMeshData = {};
		ffMeshData.meshName = info.mesh->GetName();
		ffMeshData.hasAnimation = info.mesh->IsAnimMesh();

		for (size_t i = 0; i < info.materials.size(); ++i)
		{
			FFMATERIAL_DESC ffMaterialDesc = info.materials[i]->GetMaterialDesc().ChangeToFFMATERIAL_DESC();
			ffMeshData.vecMaterialDesc.push_back(ffMaterialDesc);
		}

		for (auto& animClip : animClipInfo)
		{
			FF::FFAnimationClip ffClip = {};
			ffClip.animName = animClip.animName;
			ffClip.duration = animClip.duration;
			ffClip.frameCount = animClip.frameCount;
			ffClip.isLoop = false;

			ffMeshData.vecAnimClip.push_back(ffClip);
		}

		m_mapMeshNameForData.insert({ffMeshData.meshName, ffMeshData});

		vecFFMeshData.push_back(ffMeshData);
	}

	m_mapFFMeshData.insert({ p_key, vecFFMeshData });
}

void ResourceManager::AddMesh(const wstring& p_key, Mesh* p_mesh)
{
	m_mapMesh.insert({ p_key, p_mesh });
}

void ResourceManager::AddMaterial(const wstring& p_key, Material* p_material)
{
	m_mapMaterial.insert({ p_key, p_material });
}

void ResourceManager::CheckUsingMaterial()
{
	for (auto iter = m_mapNewMaterial.begin(); iter != m_mapNewMaterial.end();)
	{
		if (iter->second->GetRefCount() == 0)
		{
			delete iter->second;
			m_mapNewMaterial.erase(iter++);
		}
		else
		{
			++iter;
		}
	}
}

void ResourceManager::ClearRefCount()
{
	for (auto& iter : m_mapNewMaterial)
	{
		iter.second->ResetRefCount();
	}
}

void ResourceManager::AddRefCount(const wstring& p_materialName)
{
	auto iter = m_mapNewMaterial.find(p_materialName);
	if (iter != m_mapNewMaterial.end())
	{
		iter->second->AddRefCount();
	}
}

void ResourceManager::SetMaterialDescForMapMaterial(MATERIAL_DESC& p_desc, const wstring& p_materialName)
{
	{
		auto iter = m_mapMaterial.find(p_materialName);

		if (iter != m_mapMaterial.end())
		{
			p_desc.intParams = iter->second->GetMaterialDesc().intParams;
		}
	}

	{
		auto iter = m_mapNewMaterial.find(p_materialName);

		if (iter != m_mapNewMaterial.end())
		{
			p_desc.intParams = iter->second->GetMaterialDesc().intParams;
		}
	}
}

vector<FFMeshData>& ResourceManager::GetMeshDataFromFBX(const wstring& p_path)
{
	auto iter = m_mapFFMeshData.find(p_path);

	if (iter != m_mapFFMeshData.end())
	{
		return iter->second;
	}
	
	assert(iter != m_mapFFMeshData.end());
}

void ResourceManager::PushAnimationData(unsigned __int64 p_id, __int32 p_frame, __int32 p_nextFrame, float p_frameRatio)
{
	auto iter = m_mapAnimatorData.find(p_id);

	if (iter != m_mapAnimatorData.end())
	{
		iter->second.first->SetInt(1, p_frame);
		iter->second.first->SetInt(2, p_nextFrame);
		iter->second.first->SetFloat(0, p_frameRatio);
	}
	else
	{
		Shader* shader = ResourceManager::GetInstance()->GetShader(L"ComputeAnimation");
		Material* material = new Material(m_device, m_constantBuffers, m_graphicsDescriptorHeap, m_computeDescriptorHeap, m_computeCommandQueue);;
		material->SetShader(shader);
		material->SetInt(1, p_frame);
		material->SetInt(2, p_nextFrame);
		material->SetFloat(0, p_frameRatio);
		m_mapAnimatorData.insert({ p_id, {material, new StructuredBuffer()} });
	}
}

Texture* ResourceManager::CreateTextureFromResource(const wstring& name, ComPtr<ID3D12Resource> tex2D)
{
	Texture* texture = new Texture();
	texture->CreateFromResource(tex2D, m_device, m_graphicsCommandQueue);
	m_mapTexture.insert({ name,texture });

	return texture;
}

void ResourceManager::CreateMesh(const wstring& p_key)
{
	if (p_key == L"Cube")
	{
		LoadCubeMesh();
	}
	else if (p_key == L"Sphere")
	{
		LoadSphereMesh();
	}
	else if (p_key == L"Rectangle")
	{
		LoadRactangleMesh();
	}
	else if (p_key == L"Point")
	{
		LoadPointMesh();
	}
	else if (p_key == L"Line")
	{
		LoadLineMesh();
	}
}

void ResourceManager::CreateShader(const wstring& p_key, const ShaderInfo& p_info)
{
	Shader* newShader = new Shader();
	newShader->CreateGraphicsShader(m_device, m_rootSignature, m_graphicsCommandQueue, m_computeCommandQueue, p_key, p_info);
	m_mapShader.insert({ p_key,newShader });
}

void ResourceManager::CreateTexture(const wstring& p_key)
{
	Texture* newTexture = new Texture();
	newTexture->Init(p_key, m_device, m_graphicsCommandQueue);
	m_mapTexture.insert({ p_key,newTexture });
}

Mesh* ResourceManager::LoadCubeMesh()
{
	Mesh* cubeMesh = new Mesh(m_device,m_graphicsCommandQueue,m_graphicsDescriptorHeap);

	float w2 = 0.5f;
	float h2 = 0.5f;
	float d2 = 0.5f;

	vector<Vertex> vec(24);

	// 앞면
	vec[0] = Vertex(Vec3(-w2, -h2, -d2), Vec2(0.0f, 1.0f), Vec3(0.0f, 0.0f, -1.0f), Vec3(1.0f, 0.0f, 0.0f));
	vec[1] = Vertex(Vec3(-w2, +h2, -d2), Vec2(0.0f, 0.0f), Vec3(0.0f, 0.0f, -1.0f), Vec3(1.0f, 0.0f, 0.0f));
	vec[2] = Vertex(Vec3(+w2, +h2, -d2), Vec2(1.0f, 0.0f), Vec3(0.0f, 0.0f, -1.0f), Vec3(1.0f, 0.0f, 0.0f));
	vec[3] = Vertex(Vec3(+w2, -h2, -d2), Vec2(1.0f, 1.0f), Vec3(0.0f, 0.0f, -1.0f), Vec3(1.0f, 0.0f, 0.0f));
	// 뒷면
	vec[4] = Vertex(Vec3(-w2, -h2, +d2), Vec2(1.0f, 1.0f), Vec3(0.0f, 0.0f, 1.0f), Vec3(-1.0f, 0.0f, 0.0f));
	vec[5] = Vertex(Vec3(+w2, -h2, +d2), Vec2(0.0f, 1.0f), Vec3(0.0f, 0.0f, 1.0f), Vec3(-1.0f, 0.0f, 0.0f));
	vec[6] = Vertex(Vec3(+w2, +h2, +d2), Vec2(0.0f, 0.0f), Vec3(0.0f, 0.0f, 1.0f), Vec3(-1.0f, 0.0f, 0.0f));
	vec[7] = Vertex(Vec3(-w2, +h2, +d2), Vec2(1.0f, 0.0f), Vec3(0.0f, 0.0f, 1.0f), Vec3(-1.0f, 0.0f, 0.0f));
	// 윗면
	vec[8] = Vertex(Vec3(-w2, +h2, -d2), Vec2(0.0f, 1.0f), Vec3(0.0f, 1.0f, 0.0f), Vec3(1.0f, 0.0f, 0.0f));
	vec[9] = Vertex(Vec3(-w2, +h2, +d2), Vec2(0.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f), Vec3(1.0f, 0.0f, 0.0f));
	vec[10] = Vertex(Vec3(+w2, +h2, +d2), Vec2(1.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f), Vec3(1.0f, 0.0f, 0.0f));
	vec[11] = Vertex(Vec3(+w2, +h2, -d2), Vec2(1.0f, 1.0f), Vec3(0.0f, 1.0f, 0.0f), Vec3(1.0f, 0.0f, 0.0f));
	// 아랫면
	vec[12] = Vertex(Vec3(-w2, -h2, -d2), Vec2(1.0f, 1.0f), Vec3(0.0f, -1.0f, 0.0f), Vec3(-1.0f, 0.0f, 0.0f));
	vec[13] = Vertex(Vec3(+w2, -h2, -d2), Vec2(0.0f, 1.0f), Vec3(0.0f, -1.0f, 0.0f), Vec3(-1.0f, 0.0f, 0.0f));
	vec[14] = Vertex(Vec3(+w2, -h2, +d2), Vec2(0.0f, 0.0f), Vec3(0.0f, -1.0f, 0.0f), Vec3(-1.0f, 0.0f, 0.0f));
	vec[15] = Vertex(Vec3(-w2, -h2, +d2), Vec2(1.0f, 0.0f), Vec3(0.0f, -1.0f, 0.0f), Vec3(-1.0f, 0.0f, 0.0f));
	// 왼쪽면
	vec[16] = Vertex(Vec3(-w2, -h2, +d2), Vec2(0.0f, 1.0f), Vec3(-1.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, -1.0f));
	vec[17] = Vertex(Vec3(-w2, +h2, +d2), Vec2(0.0f, 0.0f), Vec3(-1.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, -1.0f));
	vec[18] = Vertex(Vec3(-w2, +h2, -d2), Vec2(1.0f, 0.0f), Vec3(-1.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, -1.0f));
	vec[19] = Vertex(Vec3(-w2, -h2, -d2), Vec2(1.0f, 1.0f), Vec3(-1.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, -1.0f));
	// 오른쪽면
	vec[20] = Vertex(Vec3(+w2, -h2, -d2), Vec2(0.0f, 1.0f), Vec3(1.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, 1.0f));
	vec[21] = Vertex(Vec3(+w2, +h2, -d2), Vec2(0.0f, 0.0f), Vec3(1.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, 1.0f));
	vec[22] = Vertex(Vec3(+w2, +h2, +d2), Vec2(1.0f, 0.0f), Vec3(1.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, 1.0f));
	vec[23] = Vertex(Vec3(+w2, -h2, +d2), Vec2(1.0f, 1.0f), Vec3(1.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, 1.0f));

	vector<uint32> idx(36);

	// 앞면
	idx[0] = 0; idx[1] = 1; idx[2] = 2;
	idx[3] = 0; idx[4] = 2; idx[5] = 3;
	// 뒷면
	idx[6] = 4; idx[7] = 5; idx[8] = 6;
	idx[9] = 4; idx[10] = 6; idx[11] = 7;
	// 윗면
	idx[12] = 8; idx[13] = 9; idx[14] = 10;
	idx[15] = 8; idx[16] = 10; idx[17] = 11;
	// 아랫면
	idx[18] = 12; idx[19] = 13; idx[20] = 14;
	idx[21] = 12; idx[22] = 14; idx[23] = 15;
	// 왼쪽면
	idx[24] = 16; idx[25] = 17; idx[26] = 18;
	idx[27] = 16; idx[28] = 18; idx[29] = 19;
	// 오른쪽면
	idx[30] = 20; idx[31] = 21; idx[32] = 22;
	idx[33] = 20; idx[34] = 22; idx[35] = 23;



	cubeMesh->Init(vec, idx);
	cubeMesh->SetBoundRadius(CalBoundRadius(Vec3(-w2, -h2, -d2)));
	m_mapMesh.insert({ L"Cube",cubeMesh });

	return cubeMesh;
}

Mesh* ResourceManager::LoadSphereMesh()
{
	Mesh* sphereMesh = new Mesh(m_device, m_graphicsCommandQueue, m_graphicsDescriptorHeap);

	float radius = 0.5f; // 구의 반지름
	uint32 stackCount = 20; // 가로 분할
	uint32 sliceCount = 20; // 세로 분할

	vector<Vertex> vec;

	Vertex v;

	// 북극
	v.pos = Vec3(0.0f, radius, 0.0f);
	v.uv = Vec2(0.5f, 0.0f);
	v.normal = v.pos;
	v.normal.Normalize();
	v.tangent = Vec3(1.0f, 0.0f, 1.0f);
	vec.push_back(v);

	float stackAngle = XM_PI / stackCount;
	float sliceAngle = XM_2PI / sliceCount;

	float deltaU = 1.f / static_cast<float>(sliceCount);
	float deltaV = 1.f / static_cast<float>(stackCount);

	// 고리마다 돌면서 정점을 계산한다 (북극/남극 단일점은 고리가 X)
	for (uint32 y = 1; y <= stackCount - 1; ++y)
	{
		float phi = y * stackAngle;

		// 고리에 위치한 정점
		for (uint32 x = 0; x <= sliceCount; ++x)
		{
			float theta = x * sliceAngle;

			v.pos.x = radius * sinf(phi) * cosf(theta);
			v.pos.y = radius * cosf(phi);
			v.pos.z = radius * sinf(phi) * sinf(theta);

			v.uv = Vec2(deltaU * x, deltaV * y);

			v.normal = v.pos;
			v.normal.Normalize();

			v.tangent.x = -radius * sinf(phi) * sinf(theta);
			v.tangent.y = 0.0f;
			v.tangent.z = radius * sinf(phi) * cosf(theta);
			v.tangent.Normalize();

			vec.push_back(v);
		}
	}

	// 남극
	v.pos = Vec3(0.0f, -radius, 0.0f);
	v.uv = Vec2(0.5f, 1.0f);
	v.normal = v.pos;
	v.normal.Normalize();
	v.tangent = Vec3(1.0f, 0.0f, 0.0f);
	vec.push_back(v);

	vector<uint32> idx(36);

	// 북극 인덱스
	for (uint32 i = 0; i <= sliceCount; ++i)
	{
		//  [0]
		//   |  \
		//  [i+1]-[i+2]
		idx.push_back(0);
		idx.push_back(i + 2);
		idx.push_back(i + 1);
	}

	// 몸통 인덱스
	uint32 ringVertexCount = sliceCount + 1;
	for (uint32 y = 0; y < stackCount - 2; ++y)
	{
		for (uint32 x = 0; x < sliceCount; ++x)
		{
			//  [y, x]-[y, x+1]
			//  |		/
			//  [y+1, x]
			idx.push_back(1 + (y)*ringVertexCount + (x));
			idx.push_back(1 + (y)*ringVertexCount + (x + 1));
			idx.push_back(1 + (y + 1) * ringVertexCount + (x));
			//		 [y, x+1]
			//		 /	  |
			//  [y+1, x]-[y+1, x+1]
			idx.push_back(1 + (y + 1) * ringVertexCount + (x));
			idx.push_back(1 + (y)*ringVertexCount + (x + 1));
			idx.push_back(1 + (y + 1) * ringVertexCount + (x + 1));
		}
	}

	// 남극 인덱스
	uint32 bottomIndex = static_cast<uint32>(vec.size()) - 1;
	uint32 lastRingStartIndex = bottomIndex - ringVertexCount;
	for (uint32 i = 0; i < sliceCount; ++i)
	{
		//  [last+i]-[last+i+1]
		//  |      /
		//  [bottom]
		idx.push_back(bottomIndex);
		idx.push_back(lastRingStartIndex + i);
		idx.push_back(lastRingStartIndex + i + 1);
	}

	sphereMesh->Init(vec, idx);
	sphereMesh->SetBoundRadius(radius);
	m_mapMesh.insert({ L"Sphere", sphereMesh });

	return sphereMesh;
}

Mesh* ResourceManager::LoadRactangleMesh()
{
	Mesh* mesh = new Mesh(m_device, m_graphicsCommandQueue, m_graphicsDescriptorHeap);

	float w2 = 0.5f;
	float h2 = 0.5f;

	vector<Vertex> vec(4);

	// 앞면
	vec[0] = Vertex(Vec3(-w2, -h2, 0), Vec2(0.0f, 1.0f), Vec3(0.0f, 0.0f, 0.0f), Vec3(1.0f, 0.0f, 0.0f));
	vec[1] = Vertex(Vec3(-w2, +h2, 0), Vec2(0.0f, 0.0f), Vec3(0.0f, 0.0f, 0.0f), Vec3(1.0f, 0.0f, 0.0f));
	vec[2] = Vertex(Vec3(+w2, +h2, 0), Vec2(1.0f, 0.0f), Vec3(0.0f, 0.0f, 0.0f), Vec3(1.0f, 0.0f, 0.0f));
	vec[3] = Vertex(Vec3(+w2, -h2, 0), Vec2(1.0f, 1.0f), Vec3(0.0f, 0.0f, 0.0f), Vec3(1.0f, 0.0f, 0.0f));

	vector<uint32> idx(6);

	idx[0] = 0; idx[1] = 1; idx[2] = 2;
	idx[3] = 0; idx[4] = 2; idx[5] = 3;

	mesh->Init(vec, idx);
	mesh->SetBoundRadius(CalBoundRadius(Vec3(-w2, -h2, 0)));
	m_mapMesh.insert({ L"Rectangle", mesh });

	return mesh;
}

Mesh* ResourceManager::LoadPointMesh()
{
	Mesh* mesh = new Mesh(m_device, m_graphicsCommandQueue, m_graphicsDescriptorHeap);

	vector<Vertex> vec(1);

	vec[0] = Vertex(Vec3(0, 0, 0), Vec2(0.5f, 0.5f), Vec3(0.0f, 0.0f, -1.0f), Vec3(1.0f, 0.0f, 0.0f));

	vector<uint32> idx(1);

	idx[0] = 0;

	mesh->Init(vec, idx);
	mesh->SetBoundRadius(0);
	m_mapMesh.insert({ L"Point", mesh });

	return mesh;
}

Mesh* ResourceManager::LoadLineMesh()
{
	Mesh* mesh = new Mesh(m_device, m_graphicsCommandQueue, m_graphicsDescriptorHeap);

	vector<Vertex> vec(2);

	vec[0] = Vertex(Vec3(0.0f, 0, 0), Vec2(0.5f, 0.5f), Vec3(0.0f, 0.0f, -1.0f), Vec3(1.0f, 0.0f, 0.0f));
	vec[1] = Vertex(Vec3(1.f, 0, 0), Vec2(0.5f, 0.5f), Vec3(0.0f, 0.0f, -1.0f), Vec3(1.0f, 0.0f, 0.0f));

	vector<uint32> idx(2);

	idx[0] = 0;
	idx[1] = 1;

	mesh->Init(vec, idx);
	mesh->SetBoundRadius(1);
	m_mapMesh.insert({ L"Line", mesh });

	return mesh;
}

void ResourceManager::CreateDefaultShader()
{
	//// 게임엔진에서 쓸 경계 디버그 정보용 셰이더
	//{
	//	Shader* shader = new Shader();
	//	ShaderInfo info = {};
	//	info.rasterizerType = RASTERIZER_TYPE::WIREFRAME;
	//	shader->SetShaderInfo(info);
	//	shader->CreateGraphicsShader(m_device, m_rootSignature, m_graphicsCommandQueue, m_computeCommandQueue,L"..\\Resources\\Shader\\debug.hlsli", info);
	//	m_mapShader.insert({ L"BoundaryDebug", shader });
	//}

	// Deferred 정보용 셰이더
	{
		Shader* shader = new Shader();
		ShaderInfo info = {};
		info.shaderType = SHADER_TYPE::FORWARD;
		info.rasterizerType = RASTERIZER_TYPE::CULL_NONE;
		info.depthStencilType = DEPTH_STENCIL_TYPE::NO_DEPTH_TEST_NO_WRITE;
		info.vs = "VS_Tex";
		info.ps = "PS_Tex";
		shader->SetShaderInfo(info);
		shader->CreateGraphicsShader(m_device, m_rootSignature, m_graphicsCommandQueue, m_computeCommandQueue, L"..\\Resources\\Shader\\forward.hlsli", info);
		m_mapShader.insert({ L"Texture",shader });
	}

	// Deferred가 끝나고 실제로 그릴 때 쓰는 셰이더
	{
		Shader* shader = new Shader();
		ShaderInfo info = {};
		info.shaderType = SHADER_TYPE::FORWARD;
		info.rasterizerType = RASTERIZER_TYPE::CULL_NONE;
		info.depthStencilType = DEPTH_STENCIL_TYPE::NO_DEPTH_TEST_NO_WRITE;
		info.vs = "VS_Final";
		info.ps = "PS_Final";
		shader->SetShaderInfo(info);
		shader->CreateGraphicsShader(m_device, m_rootSignature, m_graphicsCommandQueue, m_computeCommandQueue, L"..\\Resources\\Shader\\deferredFinal.hlsli", info);
		m_mapShader.insert({ L"Final",shader });
	}

	//// Compute 셰이더 테스트용
	{
		Shader* shader = new Shader();
		shader->CreateComputeShader(m_device, m_rootSignature, m_graphicsCommandQueue, m_computeCommandQueue, L"..\\Resources\\Shader\\compute.hlsli", "CS_Main", "cs_5_0");
		m_mapShader.insert({ L"ComputeShaderTest", shader });
	}

	// Shadow
	{
		Shader* shader = new Shader();
		ShaderInfo info =
		{
			SHADER_TYPE::SHADOW,
			RASTERIZER_TYPE::CULL_BACK,
			DEPTH_STENCIL_TYPE::LESS
		};
		shader->CreateGraphicsShader(m_device, m_rootSignature, m_graphicsCommandQueue, m_computeCommandQueue, L"..\\Resources\\Shader\\shadow.hlsli", info);
		m_mapShader.insert({ L"Shadow",shader });
	}

	// Shadow Skinned
	{
		Shader* shader = new Shader();
		ShaderInfo info =
		{
			SHADER_TYPE::SHADOW,
			RASTERIZER_TYPE::CULL_BACK,
			DEPTH_STENCIL_TYPE::LESS
		};
		shader->CreateGraphicsShader(m_device, m_rootSignature, m_graphicsCommandQueue, m_computeCommandQueue, L"..\\Resources\\Shader\\shadow_skinned.hlsli", info);
		m_mapShader.insert({ L"ShadowSkinned",shader });
	}

	// Animation Compute 셰이더 테스트용
	{
		Shader* shader = new Shader();
		shader->CreateComputeShader(m_device, m_rootSignature, m_graphicsCommandQueue, m_computeCommandQueue, L"..\\Resources\\Shader\\computeAnimation.hlsli", "CS_Main", "cs_5_0");
		m_mapShader.insert({ L"ComputeAnimation", shader });
	}
}

void ResourceManager::CreateDefaultMaterial()
{
	//// 게임엔진에서 쓸 경계 디버그 정보용 material
	//{
	//	ShaderInfo info = {};
	//	info.rasterizerType = RASTERIZER_TYPE::WIREFRAME;
	//	//info.topology = D3D_PRIMITIVE_TOPOLOGY_LINELIST;
	//	Shader* shader = GetShader(L"BoundaryDebug", info);
	//	Material* material = new Material(m_device, m_constantBuffers, m_graphicsDescriptorHeap, m_computeDescriptorHeap, m_computeCommandQueue);
	//	material->SetShader(shader);
	//	material->SetVec4(0, Vec4(0.f,1.f,0.f,1.f));
	//	m_mapMaterial.insert({ L"BoundaryDebug",material });
	//}

	// Deferred Position
	{
		/*ShaderInfo info = {};
		info.shaderType = SHADER_TYPE::FORWARD;
		info.rasterizerType = RASTERIZER_TYPE::CULL_NONE;
		info.depthStencilType = DEPTH_STENCIL_TYPE::NO_DEPTH_TEST_NO_WRITE;
		info.vs = "VS_Tex";
		info.ps = "PS_Tex";*/
		Shader* shader = GetShader(L"Texture");
		Material* material = new Material(m_device, m_constantBuffers, m_graphicsDescriptorHeap, m_computeDescriptorHeap, m_computeCommandQueue);
		material->SetName(L"DeferredPosition");
		material->SetShader(shader);
		material->SetTexture(0,
			(*m_renderTargets)[static_cast<uint8>(RENDER_TARGET_TYPE::G_BUFFER)]->GetRTTexture(POSITION));
		m_mapMaterial.insert({ L"DeferredPosition" ,material });
	}

	// Deferred Normal
	{
		/*ShaderInfo info = {};
		info.shaderType = SHADER_TYPE::FORWARD;
		info.rasterizerType = RASTERIZER_TYPE::CULL_NONE;
		info.depthStencilType = DEPTH_STENCIL_TYPE::NO_DEPTH_TEST_NO_WRITE;
		info.vs = "VS_Tex";
		info.ps = "PS_Tex";*/
		Shader* shader = GetShader(L"Texture");
		Material* material = new Material(m_device, m_constantBuffers, m_graphicsDescriptorHeap, m_computeDescriptorHeap, m_computeCommandQueue);
		material->SetName(L"DeferredNormal");
		material->SetShader(shader);
		material->SetTexture(0, (*m_renderTargets)[static_cast<uint8>(RENDER_TARGET_TYPE::G_BUFFER)]->GetRTTexture(NORMAL));
		m_mapMaterial.insert({ L"DeferredNormal",material });
	}

	// Deferred Color
	{
		/*ShaderInfo info = {};
		info.shaderType = SHADER_TYPE::FORWARD;
		info.rasterizerType = RASTERIZER_TYPE::CULL_NONE;
		info.depthStencilType = DEPTH_STENCIL_TYPE::NO_DEPTH_TEST_NO_WRITE;
		info.vs = "VS_Tex";
		info.ps = "PS_Tex";*/
		Shader* shader = GetShader(L"Texture");
		Material* material = new Material(m_device, m_constantBuffers, m_graphicsDescriptorHeap, m_computeDescriptorHeap, m_computeCommandQueue);
		material->SetName(L"DeferredColor");
		material->SetShader(shader);
		material->SetTexture(0, (*m_renderTargets)[static_cast<uint8>(RENDER_TARGET_TYPE::G_BUFFER)]->GetRTTexture(COLOR));
		m_mapMaterial.insert({ L"DeferredColor",material });
	}

	// Deferred Depth
	{
		/*ShaderInfo info = {};
		info.shaderType = SHADER_TYPE::FORWARD;
		info.rasterizerType = RASTERIZER_TYPE::CULL_NONE;
		info.depthStencilType = DEPTH_STENCIL_TYPE::NO_DEPTH_TEST_NO_WRITE;
		info.vs = "VS_Tex";
		info.ps = "PS_Tex";*/
		Shader* shader = GetShader(L"Texture");
		Material* material = new Material(m_device, m_constantBuffers, m_graphicsDescriptorHeap, m_computeDescriptorHeap, m_computeCommandQueue);
		material->SetName(L"DeferredDepth");
		material->SetShader(shader);
		material->SetTexture(0, (*m_renderTargets)[static_cast<uint8>(RENDER_TARGET_TYPE::G_BUFFER)]->GetRTTexture(DEPTH));
		m_mapMaterial.insert({ L"DeferredDepth",material });
	}

	// Deferred DiffuseLight
	{
		/*ShaderInfo info = {};
		info.shaderType = SHADER_TYPE::FORWARD;
		info.rasterizerType = RASTERIZER_TYPE::CULL_NONE;
		info.depthStencilType = DEPTH_STENCIL_TYPE::NO_DEPTH_TEST_NO_WRITE;
		info.vs = "VS_Tex";
		info.ps = "PS_Tex";*/
		Shader* shader = GetShader(L"Texture");
		Material* material = new Material(m_device, m_constantBuffers, m_graphicsDescriptorHeap, m_computeDescriptorHeap, m_computeCommandQueue);
		material->SetName(L"DeferredDiffuseLight");
		material->SetShader(shader);
		material->SetTexture(0, (*m_renderTargets)[static_cast<uint8>(RENDER_TARGET_TYPE::LIGHTING)]->GetRTTexture(DIFFUSELIGHT));
		m_mapMaterial.insert({ L"DeferredDiffuseLight"  ,material });
	}

	// Deferred SpecularLight
	{
		/*ShaderInfo info = {};
		info.shaderType = SHADER_TYPE::FORWARD;
		info.rasterizerType = RASTERIZER_TYPE::CULL_NONE;
		info.depthStencilType = DEPTH_STENCIL_TYPE::NO_DEPTH_TEST_NO_WRITE;
		info.vs = "VS_Tex";
		info.ps = "PS_Tex";*/
		Shader* shader = GetShader(L"Texture");
		Material* material = new Material(m_device, m_constantBuffers, m_graphicsDescriptorHeap, m_computeDescriptorHeap, m_computeCommandQueue);
		material->SetName(L"DeferredSpecularLight");
		material->SetShader(shader);
		material->SetTexture(0, (*m_renderTargets)[static_cast<uint8>(RENDER_TARGET_TYPE::LIGHTING)]->GetRTTexture(SPECULARLIGHT));
		m_mapMaterial.insert({ L"DeferredSpecularLight" ,material });
	}

	// ShadowMap
	{
		/*ShaderInfo info = {};
		info.shaderType = SHADER_TYPE::FORWARD;
		info.rasterizerType = RASTERIZER_TYPE::CULL_NONE;
		info.depthStencilType = DEPTH_STENCIL_TYPE::NO_DEPTH_TEST_NO_WRITE;
		info.vs = "VS_Tex";
		info.ps = "PS_Tex";*/
		Shader* shader = GetShader(L"Texture");
		Material* material = new Material(m_device, m_constantBuffers, m_graphicsDescriptorHeap, m_computeDescriptorHeap, m_computeCommandQueue);
		material->SetName(L"ShadowTarget");
		material->SetShader(shader);
		material->SetTexture(0, (*m_renderTargets)[static_cast<uint8>(RENDER_TARGET_TYPE::SHADOW)]->GetRTTexture(SHADOW));
		m_mapMaterial.insert({ L"ShadowTarget" ,material });
	}

	// Final Material
	{
		/*	ShaderInfo info = {};
			info.shaderType = SHADER_TYPE::FORWARD;
			info.rasterizerType = RASTERIZER_TYPE::CULL_NONE;
			info.depthStencilType = DEPTH_STENCIL_TYPE::NO_DEPTH_TEST_NO_WRITE;
			info.vs = "VS_Final";
			info.ps = "PS_Final";*/
		Shader* shader = GetShader(L"Final");
		Material* material = new Material(m_device, m_constantBuffers, m_graphicsDescriptorHeap, m_computeDescriptorHeap, m_computeCommandQueue);
		material->SetName(L"FinalMaterial");
		material->SetShader(shader);
		material->SetTexture(0, (*m_renderTargets)[static_cast<uint8>(RENDER_TARGET_TYPE::G_BUFFER)]->GetRTTexture(COLOR));
		material->SetTexture(1, (*m_renderTargets)[static_cast<uint8>(RENDER_TARGET_TYPE::LIGHTING)]->GetRTTexture(DIFFUSELIGHT));
		material->SetTexture(2, (*m_renderTargets)[static_cast<uint8>(RENDER_TARGET_TYPE::LIGHTING)]->GetRTTexture(SPECULARLIGHT));
		m_mapMaterial.insert({ L"FinalMaterial" ,material });
	}

	//// ComputeShader 테스트용
	//{
	//	// UAV용 Texture 생성
	//	Texture* texture = ResourceManager::GetInstance()->CreateTexture(L"UAVTexture",
	//		DXGI_FORMAT_R8G8B8A8_UNORM, 1024, 1024,
	//		CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE,
	//		D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
	//
	//	/*ShaderInfo info = {};
	//	info.shaderType = SHADER_TYPE::COMPUTE;*/
	//	Shader* shader = ResourceManager::GetInstance()->GetShader(L"ComputeShaderTest");
	//	Material* material = new Material(m_device, m_constantBuffers, m_graphicsDescriptorHeap, m_computeDescriptorHeap, m_computeCommandQueue);
	//	material->SetName(L"ComputeMaterial");
	//	material->SetTexture(0, texture);
	//	material->SetShader(shader);
	//	m_computeDescriptorHeap->SetUAV(texture->GetUAVCpuHandle(), UAV_REGISTER::u0);
	//
	//	material->Dispatch(1, 1024, 1);
	//	m_mapMaterial.insert({ L"ComputeMaterial", material });
	//}

	// Shadow
	{
		Shader* shader = ResourceManager::GetInstance()->GetShader(L"Shadow");
		Material* material = new Material(m_device, m_constantBuffers, m_graphicsDescriptorHeap, m_computeDescriptorHeap, m_computeCommandQueue);
		material->SetName(L"Shadow");
		material->SetShader(shader);
		m_mapMaterial.insert({ L"Shadow", material });
	}

	// Shadow_Skinned
	{
		Shader* shader = ResourceManager::GetInstance()->GetShader(L"ShadowSkinned");
		Material* material = new Material(m_device, m_constantBuffers, m_graphicsDescriptorHeap, m_computeDescriptorHeap, m_computeCommandQueue);
		material->SetName(L"ShadowSkinned");
		material->SetShader(shader);
		m_mapMaterial.insert({ L"ShadowSkinned", material });
	}
}
