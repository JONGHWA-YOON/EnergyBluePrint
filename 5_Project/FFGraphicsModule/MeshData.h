#pragma once

#include "IResource.h"

#include "Mesh.h"

class Mesh;
class Material;
class Device;
class ConstantBuffer;
class GraphicsDescriptorHeap;
class ComputeDescriptorHeap;
class GraphicsCommandQueue;
class ComputeCommandQueue;

class MeshData : public IResource
{
public:
	MeshData(Device* p_device, vector<ConstantBuffer*>* p_constantBuffers, GraphicsDescriptorHeap* p_graphicsDescriptorHeap, ComputeDescriptorHeap* p_computeDescriptorHeap,
		GraphicsCommandQueue* p_graphicsCommandQueue,ComputeCommandQueue* p_computeCommandQueue);
	virtual ~MeshData();

public:
	void LoadFromFBX(const wstring& path, const wstring& p_shaderPath);

	virtual void Load(const wstring& path) override;
	virtual void Save(const wstring& path) override;

	void SetName(const wstring& p_name) { m_name = p_name; }
	wstring& Getname() { return m_name; }

	vector<MeshRenderInfo>& GetMeshRenderInfo() { return _meshRenders; }
	vector<AnimClipInfo>& GetAnimClip() { return m_animationClip; }

private:
	wstring m_name;
	//Mesh*				_mesh;
	//vector<Material*>	_materials;

	vector<MeshRenderInfo> _meshRenders;
	vector<AnimClipInfo> m_animationClip;

private:
	// 참조용 변수들 
	wstring m_shaderPath;
	Device* m_device;
	vector<ConstantBuffer*>* m_constantBuffers;
	GraphicsDescriptorHeap* m_graphicsDescriptorHeap;
	ComputeDescriptorHeap* m_computeDescriptorHeap;
	GraphicsCommandQueue* m_graphicsCommandQueue;
	ComputeCommandQueue* m_computeCommandQueue;
};

