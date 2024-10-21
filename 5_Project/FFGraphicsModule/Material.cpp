#include "GraphicsPch.h"
#include "Material.h"

//#include "FFGraphicsEngine.h"
#include "ConstantBuffer.h"
#include "TableDescriptorHeap.h"
#include "CommandQueue.h"

#include "Shader.h"
#include "Texture.h"
#include "ResourceManager.h"

Material::Material(Device* p_device, vector<ConstantBuffer*>* p_constantBuffers, GraphicsDescriptorHeap* p_graphicsDescriptorHeap, ComputeDescriptorHeap* p_computeDescriptorHeap, ComputeCommandQueue* p_computeCommandQueue)
	: IResource(RESOURCE_TYPE::MATERIAL)
{
	m_device = p_device;
	m_constantBuffers = p_constantBuffers;
	m_graphicsDescriptorHeap = p_graphicsDescriptorHeap;
	m_computeDescriptorHeap = p_computeDescriptorHeap;
	m_computeCommandQueue = p_computeCommandQueue;
}

Material::~Material()
{

}

void Material::SetMaterialPramsForDesc(const MATERIAL_DESC & p_materialDesc)
{
	for (size_t i = 0; i < p_materialDesc.intParams.size(); ++i)
	{
		this->SetInt(static_cast<uint8>(i), p_materialDesc.intParams[i]);
	}

	for (size_t i = 0; i < p_materialDesc.floatParams.size(); ++i)
	{
		this->SetFloat(static_cast<uint8>(i), p_materialDesc.floatParams[i]);
	}

	for (size_t i = 0; i < p_materialDesc.vec2Params.size(); ++i)
	{
		this->SetVec2(static_cast<uint8>(i), p_materialDesc.vec2Params[i]);
	}

	for (size_t i = 0; i < p_materialDesc.vec4Params.size(); ++i)
	{
		this->SetVec4(static_cast<uint8>(i), p_materialDesc.vec4Params[i]);
	}

	for (size_t i = 0; i < p_materialDesc.texture.size(); ++i)
	{
		if (p_materialDesc.texture[i] == L"") continue;

		this->SetTexture(static_cast<uint8>(i),
			ResourceManager::GetInstance()->GetTexture(p_materialDesc.texture[i]));
	}
	this->SetColor(p_materialDesc.color);
	this->SetName(p_materialDesc.materialName);
	this->SetShader(ResourceManager::GetInstance()->GetShader(p_materialDesc.shader));

	m_materialDesc = p_materialDesc;
}

void Material::PushGraphicsData()
{
	// CBV 업로드
	(*m_constantBuffers)[static_cast<uint8>(CONSTANT_BUFFER_TYPE::MATERIAL)]->PushGraphicsData(&m_params, sizeof(m_params));
	
	// SRV 업로드
	for (size_t i = 0; i < m_textures.size(); i++)
	{
		if (m_textures[i] == nullptr)
		{
			continue;
		}
	
		SRV_REGISTER reg = SRV_REGISTER(static_cast<int8>(SRV_REGISTER::t0) + i);
		m_graphicsDescriptorHeap->SetSRV(m_textures[i]->GetSRVCpuHandle(), reg);
	}
	// 파이프라인 세팅
	m_shader->Update();
}

void Material::PushComputeSRVData()
{
	(*m_constantBuffers)[static_cast<uint8>(CONSTANT_BUFFER_TYPE::MATERIAL)]->PushComputeSRVData(&m_params, sizeof(m_params));

	for (size_t i = 0; i < m_textures.size(); ++i)
	{
		if (m_textures[i] == nullptr)
		{
			continue;
		}

		SRV_REGISTER reg = SRV_REGISTER(static_cast<int8>(SRV_REGISTER::t0) + i);
		m_computeDescriptorHeap->SetSRV(m_textures[i]->GetSRVCpuHandle(), reg);
	}

	m_shader->Update();
}

void Material::Dispatch(uint32 p_x, uint32 p_y, UINT p_z)
{
	PushComputeSRVData();

	m_computeDescriptorHeap->CommitTable();

	m_computeCommandQueue->GetComputeCmdList()->Dispatch(p_x, p_y, p_z);

	m_computeCommandQueue->FlushComputeCommandQueue();
}

void Material::ResetMaterialDescForParams()
{
	m_materialDesc.materialName = m_name;


}
