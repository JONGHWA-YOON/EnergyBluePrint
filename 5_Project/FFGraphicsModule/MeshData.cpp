#include "GraphicsPch.h"
#include "MeshData.h"

#include "Mesh.h"
#include "Material.h"
#include "FBXLoader.h"

#include "ResourceManager.h"

MeshData::MeshData(Device* p_device, vector<ConstantBuffer*>* p_constantBuffers, GraphicsDescriptorHeap* p_graphicsDescriptorHeap, ComputeDescriptorHeap* p_computeDescriptorHeap,
	GraphicsCommandQueue* p_graphicsCommandQueue, ComputeCommandQueue* p_computeCommandQueue) : IResource(RESOURCE_TYPE::MESH_DATA)
{
	m_device = p_device;
	m_constantBuffers = p_constantBuffers;
	m_graphicsDescriptorHeap = p_graphicsDescriptorHeap;
	m_computeDescriptorHeap = p_computeDescriptorHeap;
	m_graphicsCommandQueue = p_graphicsCommandQueue;
	m_computeCommandQueue = p_computeCommandQueue;
}

MeshData::~MeshData()
{

}

void MeshData::LoadFromFBX(const wstring& path, const wstring& p_shaderPath)
{
	FBXLoader loader;
	loader.Init(m_device, m_constantBuffers, m_graphicsDescriptorHeap, m_computeDescriptorHeap, m_computeCommandQueue);
	loader.LoadFbx(path, p_shaderPath);

	for (int32 i = 0; i < loader.GetMeshCount(); i++)
	{
		Mesh* mesh = new Mesh(m_device, m_graphicsCommandQueue, m_graphicsDescriptorHeap);
		mesh->CreateFromFBX(&loader.GetMesh(i), loader);

		ResourceManager::GetInstance()->AddMesh(mesh->GetName(), mesh);

		// Material 찾아서 연동
		vector<Material*> materials;
		for (size_t j = 0; j < loader.GetMesh(i).materials.size(); j++)
		{
			Material* material = ResourceManager::GetInstance()->GetMaterial(loader.GetMesh(i).materials[j].name);
			materials.push_back(material);
		}

		MeshRenderInfo info = {};
		info.mesh = mesh;
		info.hasAnimation = mesh->IsAnimMesh();
		info.materials = materials;
		_meshRenders.push_back(info);

		vector<AnimClipInfo> clipInfo = mesh->GetAnimClipInfo();
		m_animationClip = mesh->GetAnimClipInfo();
	}
}

void MeshData::Load(const wstring& path)
{
	// TODO
}

void MeshData::Save(const wstring& path)
{
	// TODO
}
