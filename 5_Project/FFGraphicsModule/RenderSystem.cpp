#include "GraphicsPch.h"
#include "RenderSystem.h"

#include "Mesh.h"
#include "Material.h"
#include "ResourceManager.h"
#include "RenderTargetGroup.h"
#include "SwapChain.h"
#include "CameraManager.h"
#include "FFCamera.h"
#include "ConstantBuffer.h"
#include "InstancingBuffer.h"
#include "Shader.h"

#include "StructuredBuffer.h"

#include "Frustum.h"
#include "TimeManager.h"

DirectX::BoundingSphere bound;

XMVECTOR lightDir;
XMVECTOR lightPos;
XMVECTOR targetPos;
XMVECTOR lightUp;
XMMATRIX lightView;

RenderSystem::~RenderSystem()
{
	for (auto& buffer : m_buffers)
	{
		delete buffer.second;
	}
}

void RenderSystem::Init(Device* p_device, SwapChain* p_swapChain, array<RenderTargetGroup*, RENDER_TARGET_TYPE_COUNT>* p_renderTargets, vector<ConstantBuffer*>* p_constantBuffers)
{
	bound.Center = Vec3(0, 0, 0);
	bound.Radius = sqrt(45 * 45);

	m_device = p_device;
	m_swapChain = p_swapChain;
	m_renderTargets = p_renderTargets;
	m_constantBuffers = p_constantBuffers;
}

void RenderSystem::PushData(const Matrix& p_WTM, const wstring& p_mesh, const MATERIAL_DESC& p_materialDesc, wstring& p_materialName, int p_materialIndex)
{
	tuple<Matrix, wstring, MATERIAL_DESC, wstring&, int> _data = { p_WTM, p_mesh, p_materialDesc,p_materialName,p_materialIndex };
	m_renderObjects.push_back(_data);
}

void RenderSystem::Render()
{
	// ·»´õ Å¸°Ùµé Å¬¸®¾î
	ClearRTV();

	// ¿ÀºêÁ§Æ® Á¤·Ä
	SortObject();

	// ±×¸²ÀÚ ·»´õ (½¦µµ¿ì ¸Ê ¸¸µé±âÀÓ)
	RenderShadow();

	// µðÆÛµå ·»´õ
	Render_Deferred();

	// ½ºÅ²µå ¸Þ½¬ ·»´õ
	Render_SkinnedMesh();

	// ¶óÀÌÆ® ·»´õ
	Render_Light();

	// ÃÖÁ¾ È­¸é Ãâ·Â
	Render_Final();

	// Æ÷¿öµå ·»´õ
	Render_Forward();

	// ÀÌÆåÆ® ·»´õ
	Render_Effect();

	// UI ·»´õ
	Render_UI();

	m_skinnedMeshTest.clear();
	m_textureAnim.clear();
	m_effect.clear();
}

void RenderSystem::ClearInstanceBuffer()
{
	for (auto& buffer : m_buffers)
	{
		buffer.second->Clear();
	}
}

void RenderSystem::ClearRenderObjects()
{
	m_renderObjects.clear();
}

void RenderSystem::DrawSkinnedMesh(unsigned __int64 p_id, __int32 p_frame, __int32 p_nextFrame, float p_frameRatio, const std::wstring& p_currentAnimationName, const Matrix& p_WTM, const std::wstring& p_mesh, const MATERIAL_DESC& p_material, std::wstring& p_materialName, int p_materialIndex)
{
	//wstring a = L"rig|Run";

	//tuple<unsigned __int64, __int32, __int32, float,std::wstring, Matrix, std::wstring, MATERIAL_DESC, std::wstring&> temp =
	//{
	//	p_id,
	//	p_frame,
	//	p_nextFrame,
	//	p_frameRatio,
	//	p_currentAnimationName,
	//	p_WTM,
	//	p_mesh,
	//	p_material,
	//	p_materialName
	//};

	SkinnedMeshData temp =
	{
		p_id,
		p_frame,
		p_nextFrame,
		p_frameRatio,
		p_currentAnimationName,
		p_WTM,
		p_mesh,
		p_material,
		p_materialName,
		p_materialIndex
	};

	Shader* shader = ResourceManager::GetInstance()->GetShader(p_material.shader);
	switch (shader->GetShaderInfo().shaderType)
	{
		case SHADER_TYPE::EFFECT_ANIM:
		{
			m_textureAnim.emplace_back(temp);
		}
		break;
		case SHADER_TYPE::EFFECT:
		{
			m_effect.emplace_back(temp);
		}
		break;
		default:
		{
			m_skinnedMeshTest.emplace_back(temp);
		}
		break;
	}
}

void RenderSystem::ClearRTV()
{
	int8 backIndex = m_swapChain->GetBackBufferIndex();

	// ¶óÀÌÆ® Á¤º¸ Push

	// ·»´õ Å¸°Ùµé Clear
	(*m_renderTargets)[static_cast<uint8>(RENDER_TARGET_TYPE::SWAP_CHAIN)]->ClearRenderTargetView(backIndex);
	(*m_renderTargets)[static_cast<uint8>(RENDER_TARGET_TYPE::G_BUFFER)]->ClearRenderTargetView();
	(*m_renderTargets)[static_cast<uint8>(RENDER_TARGET_TYPE::LIGHTING)]->ClearRenderTargetView();
	(*m_renderTargets)[static_cast<uint8>(RENDER_TARGET_TYPE::SHADOW)]->ClearRenderTargetView();
}

void RenderSystem::SortObject()
{
	m_deferred.clear();
	m_forward.clear();
	m_lights.clear();
	m_particles.clear();
	m_ui.clear();

	for (auto& object : m_renderObjects)
	{
		switch (ResourceManager::GetInstance()->GetShader(get<2>(object).shader)->GetShaderInfo().shaderType)
		{
			case SHADER_TYPE::DEFERRED:
			{
				m_deferred.push_back(object);
			}
			break;
			case SHADER_TYPE::DEFERRED_AND_SKIN:
			{
				//m_skinnedMesh.push_back(object);
			}
			break;
			case SHADER_TYPE::FORWARD:
			{
				m_forward.push_back(object);
			}
			break;
			case SHADER_TYPE::LIGHTING:
			{
				m_lights.push_back(object);
			}
			break;
			case SHADER_TYPE::PARTICLE:
			{
				m_particles.push_back(object);
			}
			break;
			case SHADER_TYPE::UI:
			{
				m_ui.push_back(object);
			}
			break;
		}
	}
}

void RenderSystem::RenderShadow()
{
	(*m_renderTargets)[static_cast<uint8>(RENDER_TARGET_TYPE::SHADOW)]->OMSetRenderTargets();

	Frustum* frustum = CameraManager::GetInstance().GetCamera(0)->GetFrustum();

	for (int i = 0; i < m_lightParams->lightCount; ++i)
	{
		if (static_cast<LIGHT_TYPE>(m_lightParams->lights[i].lightType) != LIGHT_TYPE::DIRECTIONAL_LIGHT)
		{
			continue;
		}

		Matrix lightWorld = get<0>(m_lights[i]);

		Vec3 dir = lightWorld.Backward();

#pragma region TEST
		{
			lightDir = XMLoadFloat3(&dir);
			lightPos = -2.0f * bound.Radius * lightDir;
			targetPos = XMLoadFloat3(&bound.Center);
			lightUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
			lightView = XMMatrixLookAtLH(lightPos, targetPos, lightUp);
		}
#pragma  endregion

		XMFLOAT3 sphereCenterLS;
		XMStoreFloat3(&sphereCenterLS, XMVector3TransformCoord(targetPos, lightView));

		float l = sphereCenterLS.x - bound.Radius;
		float b = sphereCenterLS.y - bound.Radius;
		float n = sphereCenterLS.z - bound.Radius;
		float r = sphereCenterLS.x + bound.Radius;
		float t = sphereCenterLS.y + bound.Radius;
		float f = sphereCenterLS.z + bound.Radius;

		XMMATRIX lightProj = XMMatrixOrthographicOffCenterLH(l, r, b, t, n, f);

		static float speed = 100;
		//speed++;
		Matrix move =
		{
			0,0,0,0,
			0,0,0,0,
			0,0,0,0,
			dir.x * speed, dir.y * speed, dir.z * speed, 0
		};

		//Matrix lightView = CameraManager::GetInstance().GetCamera(0)->matView;

		//Matrix lightView = lightWorld.Invert();
		//lightView += move;
		
		//Matrix lightProjection = ::XMMatrixOrthographicLH(1000, 1000, 0.1, 1000);
		//Matrix lightProjection = CameraManager::GetInstance().GetCamera(0)->matPerspectiveProjection;

		for (auto& i : m_deferred)
		{
			if (!frustum->ContainSphere(get<0>(i), ResourceManager::GetInstance()->GetMesh(get<1>(i))->GetBoundRadius()))
			{
				continue;
			}

			TransformParams transformParams = {};
			transformParams.matWorld = get<0>(i);
			transformParams.matView = lightView;
			transformParams.matProjection = lightProj;
			transformParams.matWV = transformParams.matWorld * transformParams.matView;
			transformParams.matWVP = transformParams.matWV * transformParams.matProjection;

			(*m_constantBuffers)[static_cast<uint8>(CONSTANT_BUFFER_TYPE::TRANSFORM)]->PushGraphicsData(&transformParams, sizeof(transformParams));

			ResourceManager::GetInstance()->GetMaterial(L"Shadow")->PushGraphicsData();

			ResourceManager::GetInstance()->GetMesh(get<1>(i))->Render(false, 1, get<4>(i));
		}

		for (auto& i : m_skinnedMeshTest)
		{
			unsigned __int64 id = i.animatorID;
			__int32 frame = i.curFrame;
			__int32 nextFrame = i.nextFrame;
			float frameRatio = i.ratio;
			wstring curAnimName = i.curAnimationName;
			Matrix wtm = i.WTM;
			wstring meshName = i.meshName;
			MATERIAL_DESC desc = i.materialDesc;
			wstring& materialName = i.materialName;
			int materialIndex = i.materialIndex;
		
			Mesh* mesh = ResourceManager::GetInstance()->GetMesh(meshName);
			Material* material = ResourceManager::GetInstance()->GetMaterial(desc, materialName);
				
			if (!frustum->ContainSphere(wtm, mesh->GetBoundRadius()))
			{
				continue;
			}
		
			map<uint64, pair<Material*, StructuredBuffer*>>& mapAnimData = ResourceManager::GetInstance()->GetAnimData();
		
			auto iter = mapAnimData.find(id);
		
			if (iter == mapAnimData.end())
			{
				Shader* shader = ResourceManager::GetInstance()->GetShader(L"ComputeAnimation");
				Material* material = new Material(m_device, m_constantBuffers,
					ResourceManager::GetInstance()->GetGraphicsDescriptorHeap(),
					ResourceManager::GetInstance()->GetComputeDescriptorHeap(),
					ResourceManager::GetInstance()->GetComputeCommandQueue());
		
				material->SetShader(shader);
				mapAnimData.insert({ id, {material, new StructuredBuffer()} });
			}
		
			
		
			uint32 boneCount = mesh->GetBoneInfo().size();
		
			if (mapAnimData[id].second->GetElementCount() < boneCount)
			{
				mapAnimData[id].second->Init(sizeof(Matrix), boneCount, m_device,
					ResourceManager::GetInstance()->GetGraphicsDescriptorHeap(),
					ResourceManager::GetInstance()->GetComputeDescriptorHeap());
			}
		
			FFMeshData& meshData = ResourceManager::GetInstance()->GetMeshDataFromMeshName(meshName);
		
			uint32 animIndex = 0;
			for (size_t i = 0; i < meshData.vecAnimClip.size(); ++i)
			{
				if (curAnimName == meshData.vecAnimClip[i].animName)
				{
					animIndex = i;
					break;
				}
			}
		
			mesh->GetBoneFrameDataBuffer(animIndex)->PushComputeSRVData(SRV_REGISTER::t8);
			mesh->GetBoneOffsetBuffer()->PushComputeSRVData(SRV_REGISTER::t9);
		
			mapAnimData[id].second->PushComputeUAVData(UAV_REGISTER::u0);
		
			mapAnimData[id].first->SetInt(0, boneCount);
			mapAnimData[id].first->SetInt(1, frame);
			mapAnimData[id].first->SetInt(2, nextFrame);
			mapAnimData[id].first->SetFloat(0, frameRatio);
		
			uint32 groupCount = (boneCount / 256) + 1;
			mapAnimData[id].first->Dispatch(groupCount, 1, 1);
		
			mapAnimData[id].second->PushGraphicsData(SRV_REGISTER::t7);
		
			TransformParams transformParams = {};
			transformParams.matWorld = wtm;
			transformParams.matView = lightView;
			transformParams.matProjection = lightProj;
			transformParams.matWV = transformParams.matWorld * transformParams.matView;
			transformParams.matWVP = transformParams.matWV * transformParams.matProjection;
		
			(*m_constantBuffers)[static_cast<uint8>(CONSTANT_BUFFER_TYPE::TRANSFORM)]->PushGraphicsData(&transformParams, sizeof(transformParams));
		
			ResourceManager::GetInstance()->GetMaterial(L"ShadowSkinned")->PushGraphicsData();
			mesh->Render(false, 1, materialIndex);
		}
	}

	(*m_renderTargets)[static_cast<uint8>(RENDER_TARGET_TYPE::SHADOW)]->WaitTargetToResource();
}

void RenderSystem::Render_Deferred()
{
	// µðÆÛµå¿ë ·»´õ Å¸°Ù Set
	(*m_renderTargets)[static_cast<uint8>(RENDER_TARGET_TYPE::G_BUFFER)]->OMSetRenderTargets();
	Render_Object(m_deferred);
}

void RenderSystem::Render_SkinnedMesh()
{
	//for (auto& i : m_skinnedMesh)
	//{
	//	TransformParams transformParams = {};
	//	transformParams.matWorld = get<0>(i);
	//	transformParams.matView = CameraManager::GetInstance().GetCamera(0)->matView;
	//	transformParams.matProjection = CameraManager::GetInstance().GetCamera(0)->matPerspectiveProjection;
	//	transformParams.matWV = transformParams.matWorld * transformParams.matView;
	//	transformParams.matWVP = transformParams.matWV * transformParams.matProjection;
	//	transformParams.matViewInv = CameraManager::GetInstance().GetCamera(0)->matView.Invert();
	//	
	//	(*m_constantBuffers)[static_cast<uint8>(CONSTANT_BUFFER_TYPE::TRANSFORM)]->PushGraphicsData(&transformParams, sizeof(transformParams));
	//	
	//	Mesh* mesh = ResourceManager::GetInstance()->GetMesh(get<1>(i));
	//	Material* material = ResourceManager::GetInstance()->GetMaterial(get<2>(i), get<3>(i));
	//	
	//	material->PushGraphicsData();
	//	mesh->Render();
	//}

	for (auto& i : m_skinnedMeshTest)
	{
		Frustum* frustum = CameraManager::GetInstance().GetCamera(0)->GetFrustum();

		unsigned __int64 id = i.animatorID;
		__int32 frame = i.curFrame;
		__int32 nextFrame = i.nextFrame;
		float frameRatio = i.ratio;
		wstring curAnimName = i.curAnimationName;
		Matrix wtm = i.WTM;
		wstring meshName = i.meshName;
		MATERIAL_DESC desc = i.materialDesc;
		wstring& materialName = i.materialName;
		int materialIndex = i.materialIndex;

		Mesh* mesh = ResourceManager::GetInstance()->GetMesh(meshName);
		Material* material = ResourceManager::GetInstance()->GetMaterial(desc, materialName);

		if (!frustum->ContainSphere(wtm, mesh->GetBoundRadius()))
		{
			continue;
		}

		map<uint64, pair<Material*, StructuredBuffer*>>& mapAnimData = ResourceManager::GetInstance()->GetAnimData();

		auto iter = mapAnimData.find(id);

		if (iter == mapAnimData.end())
		{
			Shader* shader = ResourceManager::GetInstance()->GetShader(L"ComputeAnimation");
			Material* material = new Material(m_device, m_constantBuffers,
				ResourceManager::GetInstance()->GetGraphicsDescriptorHeap(),
				ResourceManager::GetInstance()->GetComputeDescriptorHeap(),
				ResourceManager::GetInstance()->GetComputeCommandQueue());

			material->SetShader(shader);
			mapAnimData.insert({ id, {material, new StructuredBuffer()} });
		}

		uint32 boneCount = mesh->GetBoneInfo().size();

		if (mapAnimData[id].second->GetElementCount() < boneCount)
		{
			mapAnimData[id].second->Init(sizeof(Matrix), boneCount, m_device,
				ResourceManager::GetInstance()->GetGraphicsDescriptorHeap(),
				ResourceManager::GetInstance()->GetComputeDescriptorHeap());
		}

		FFMeshData& meshData = ResourceManager::GetInstance()->GetMeshDataFromMeshName(meshName);

		uint32 animIndex = 0;
		for (size_t i = 0; i < meshData.vecAnimClip.size(); ++i)
		{
			if (curAnimName == meshData.vecAnimClip[i].animName)
			{
				animIndex = i;
				break;
			}
		}

		mesh->GetBoneFrameDataBuffer(animIndex)->PushComputeSRVData(SRV_REGISTER::t8);
		mesh->GetBoneOffsetBuffer()->PushComputeSRVData(SRV_REGISTER::t9);

		mapAnimData[id].second->PushComputeUAVData(UAV_REGISTER::u0);

		mapAnimData[id].first->SetInt(0, boneCount);
		mapAnimData[id].first->SetInt(1, frame);
		mapAnimData[id].first->SetInt(2, nextFrame);
		mapAnimData[id].first->SetFloat(0, frameRatio);

		uint32 groupCount = (boneCount / 256) + 1;
		mapAnimData[id].first->Dispatch(groupCount, 1, 1);

		mapAnimData[id].second->PushGraphicsData(SRV_REGISTER::t7);

		TransformParams transformParams = {};
		transformParams.matWorld = wtm;
		transformParams.matView = CameraManager::GetInstance().GetCamera(0)->matView;
		transformParams.matProjection = CameraManager::GetInstance().GetCamera(0)->matPerspectiveProjection;
		transformParams.matWV = transformParams.matWorld * transformParams.matView;
		transformParams.matWVP = transformParams.matWV * transformParams.matProjection;
		transformParams.matViewInv = CameraManager::GetInstance().GetCamera(0)->matView.Invert();

		(*m_constantBuffers)[static_cast<uint8>(CONSTANT_BUFFER_TYPE::TRANSFORM)]->PushGraphicsData(&transformParams, sizeof(transformParams));

		material->PushGraphicsData();
		mesh->Render(false, 1, materialIndex);
	}

	// µðÆÛµå¿ë ·»´õ Å¸°Ù Wait
	(*m_renderTargets)[static_cast<uint8>(RENDER_TARGET_TYPE::G_BUFFER)]->WaitTargetToResource();
}

void RenderSystem::Render_Light()
{
	// ¶óÀÌÆ®¿ë ·»´õ Å¸°Ù Set
	(*m_renderTargets)[static_cast<uint8>(RENDER_TARGET_TYPE::LIGHTING)]->OMSetRenderTargets();

	for (int i = 0; i < m_lightParams->lightCount; ++i)
	{
		if (static_cast<LIGHT_TYPE>(m_lightParams->lights[i].lightType) != LIGHT_TYPE::DIRECTIONAL_LIGHT)
		{
			continue;
		}

		Matrix lightWorld = get<0>(m_lights[i]);

		Vec3 dir = lightWorld.Backward();

#pragma region TEST
		{
			lightDir = XMLoadFloat3(&dir);
			lightPos = -2.0f * bound.Radius * lightDir;
			targetPos = XMLoadFloat3(&bound.Center);
			lightUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
			lightView = XMMatrixLookAtLH(lightPos, targetPos, lightUp);
		}
#pragma  endregion

		XMFLOAT3 sphereCenterLS;
		XMStoreFloat3(&sphereCenterLS, XMVector3TransformCoord(targetPos, lightView));

		float l = sphereCenterLS.x - bound.Radius;
		float b = sphereCenterLS.y - bound.Radius;
		float n = sphereCenterLS.z - bound.Radius;
		float r = sphereCenterLS.x + bound.Radius;
		float t = sphereCenterLS.y + bound.Radius;
		float f = sphereCenterLS.z + bound.Radius;

		XMMATRIX lightProj = XMMatrixOrthographicOffCenterLH(l, r, b, t, n, f);

		static float speed = 100;
		//speed++;
		Matrix move =
		{
			0,0,0,0,
			0,0,0,0,
			0,0,0,0,
			dir.x * speed, dir.y * speed, dir.z * speed, 0
		};
		//Matrix lightView = CameraManager::GetInstance().GetCamera(0)->matView;

		//Matrix lightView = lightWorld.Invert();
		//lightView += move;

		//Matrix lightProjection = ::XMMatrixOrthographicLH(1000, 1000, 0.1, 1000);
		//Matrix lightProjection = CameraManager::GetInstance().GetCamera(0)->matPerspectiveProjection;

		TransformParams transformParams = {};
		transformParams.matWorld = get<0>(m_lights[i]);
		transformParams.matView = CameraManager::GetInstance().GetCamera(0)->matView;
		transformParams.matProjection = CameraManager::GetInstance().GetCamera(0)->matPerspectiveProjection;;
		transformParams.matWV = transformParams.matWorld * transformParams.matView;
		transformParams.matWVP = transformParams.matWV * transformParams.matProjection;
		transformParams.matViewInv = transformParams.matView.Invert();
		transformParams.matLightVP = lightView * lightProj;

		(*m_constantBuffers)[static_cast<uint8>(CONSTANT_BUFFER_TYPE::TRANSFORM)]->PushGraphicsData(&transformParams, sizeof(transformParams));

		get<2>(m_lights[i]).intParams[0] = 0;
		get<2>(m_lights[i]).texture[2] = L"ShadowTarget";

		ResourceManager::GetInstance()->GetMaterial(get<2>(m_lights[i]), get<3>(m_lights[i]))->PushGraphicsData();
		ResourceManager::GetInstance()->GetMesh(get<1>(m_lights[i]))->Render();
	}

	// ¶óÀÌÆ®¿ë ·»´õ Å¸°Ù Wait
	(*m_renderTargets)[static_cast<uint8>(RENDER_TARGET_TYPE::LIGHTING)]->WaitTargetToResource();
}

void RenderSystem::Render_Final()
{
	int8 backIndex = m_swapChain->GetBackBufferIndex();
	// ½º¿ÒÃ¼ÀÎÀÇ ¹é¹öÆÛ Set
	(*m_renderTargets)[static_cast<uint8>(RENDER_TARGET_TYPE::SWAP_CHAIN)]->OMSetRenderTargets(1, backIndex);

	ResourceManager::GetInstance()->GetMaterial(L"FinalMaterial")->PushGraphicsData();
	ResourceManager::GetInstance()->GetMesh(L"Rectangle")->Render(true);
}

void RenderSystem::Render_Forward()
{
	Render_Object(m_forward);
}

void RenderSystem::Render_UI()
{
	for (auto& i : m_ui)
	{
		Mesh* mesh = ResourceManager::GetInstance()->GetMesh(get<1>(i));
		Material* material = ResourceManager::GetInstance()->GetMaterial(get<2>(i), get<3>(i));

		TransformParams transformParams = {};
		transformParams.matWorld = get<0>(i);
		transformParams.matView = Matrix::Identity;//CameraManager::GetInstance().GetCamera(0)->matView;
		transformParams.matProjection = CameraManager::GetInstance().GetCamera(0)->matOrthographicProjection;
		transformParams.matWV = transformParams.matWorld * transformParams.matView;
		transformParams.matWVP = transformParams.matWV * transformParams.matProjection;
		transformParams.matViewInv = CameraManager::GetInstance().GetCamera(0)->matView.Invert();

		(*m_constantBuffers)[static_cast<uint8>(CONSTANT_BUFFER_TYPE::TRANSFORM)]->PushGraphicsData(&transformParams, sizeof(transformParams));

		material->PushGraphicsData();
		mesh->Render();
	}
}

void RenderSystem::Render_Object(vector<tuple<Matrix, wstring, MATERIAL_DESC, wstring&, int>>& p_renderObjects)
{
	map<uint64, vector<tuple<Matrix, Mesh*, Material*, int>>> _data;

	for (auto& data : p_renderObjects)
	{
		Mesh* mesh = ResourceManager::GetInstance()->GetMesh(get<1>(data));
		Material* material = ResourceManager::GetInstance()->GetMaterial(get<2>(data), get<3>(data));
		InstanceID instanceID = { mesh->GetID(), material->GetID() };
		tuple<Matrix, Mesh*, Material*, int> _tuple = { get<0>(data), mesh,material , get<4>(data) };
		_data[instanceID.id].push_back(_tuple);
	}

	for (auto& iter : _data)
	{
		auto& vec = iter.second;

		Frustum* frustum =  CameraManager::GetInstance().GetCamera(0)->GetFrustum();

		if (vec.size() == 1)
		{
			TransformParams transformParams = {};
			transformParams.matWorld = get<0>(vec[0]);
			transformParams.matView = CameraManager::GetInstance().GetCamera(0)->matView;
			transformParams.matProjection = CameraManager::GetInstance().GetCamera(0)->matPerspectiveProjection;
			transformParams.matWV = transformParams.matWorld * transformParams.matView;
			transformParams.matWVP = transformParams.matWV * transformParams.matProjection;
			transformParams.matViewInv = CameraManager::GetInstance().GetCamera(0)->matView.Invert();

			if (!frustum->ContainSphere(get<0>(vec[0]), get<1>(vec[0])->GetBoundRadius()))
			{
				continue;
			}

			(*m_constantBuffers)[static_cast<uint8>(CONSTANT_BUFFER_TYPE::TRANSFORM)]->PushGraphicsData(&transformParams, sizeof(transformParams));

			get<2>(vec[0])->GetMaterialDesc().intParams[0] = 0;
			get<2>(vec[0])->SetMaterialPramsForDesc(get<2>(vec[0])->GetMaterialDesc());
			get<2>(vec[0])->PushGraphicsData();
			get<1>(vec[0])->Render(false ,1, get<3>(vec[0]));
		}
		else
		{
			uint64 instanceID = iter.first;

			for (size_t i = 0; i < vec.size(); i++)
			{
				//if (!frustum->ContainSphere(get<0>(vec[i]), get<1>(vec[i])->GetBoundRadius()))
				//{
				//	continue;
				//}

				InstancingParams params;
				params.matWorld = get<0>(vec[i]);
				params.matWV = params.matWorld * CameraManager::GetInstance().GetCamera(0)->matView;
				params.matWVP = params.matWV * CameraManager::GetInstance().GetCamera(0)->matPerspectiveProjection;
				params.matProj = CameraManager::GetInstance().GetCamera(0)->matPerspectiveProjection;

				AddInstanceParams(instanceID, params);
			}

			m_buffers[instanceID]->PushData();

			get<2>(vec[0])->GetMaterialDesc().intParams[0] = 1;
			get<2>(vec[0])->SetMaterialPramsForDesc(get<2>(vec[0])->GetMaterialDesc());
			get<2>(vec[0])->PushGraphicsData();
			get<1>(vec[0])->Render(m_buffers[instanceID], get<3>(vec[0]));
		}
	}
}

void RenderSystem::Render_Effect()
{
	// Effect Texture Animation
	for (auto& i : m_textureAnim)
	{
		Frustum* frustum = CameraManager::GetInstance().GetCamera(0)->GetFrustum();

		unsigned __int64 id = i.animatorID;
		__int32 frame = i.curFrame;
		__int32 nextFrame = i.nextFrame;
		float frameRatio = i.ratio;
		wstring curAnimName = i.curAnimationName;
		Matrix wtm = i.WTM;
		wstring meshName = i.meshName;
		MATERIAL_DESC desc = i.materialDesc;
		wstring& materialName = i.materialName;
		int materialIndex = i.materialIndex;

		Mesh* mesh = ResourceManager::GetInstance()->GetMesh(meshName);
		Material* material = ResourceManager::GetInstance()->GetMaterial(desc, materialName);

		if (!frustum->ContainSphere(wtm, mesh->GetBoundRadius()))
		{
			continue;
		}

		map<uint64, pair<Material*, StructuredBuffer*>>& mapAnimData = ResourceManager::GetInstance()->GetAnimData();

		auto iter = mapAnimData.find(id);

		if (iter == mapAnimData.end())
		{
			Shader* shader = ResourceManager::GetInstance()->GetShader(L"ComputeAnimation");
			Material* material = new Material(m_device, m_constantBuffers,
				ResourceManager::GetInstance()->GetGraphicsDescriptorHeap(),
				ResourceManager::GetInstance()->GetComputeDescriptorHeap(),
				ResourceManager::GetInstance()->GetComputeCommandQueue());

			material->SetShader(shader);
			mapAnimData.insert({ id, {material, new StructuredBuffer()} });
		}

		uint32 boneCount = mesh->GetBoneInfo().size();

		if (mapAnimData[id].second->GetElementCount() < boneCount)
		{
			mapAnimData[id].second->Init(sizeof(Matrix), boneCount, m_device,
				ResourceManager::GetInstance()->GetGraphicsDescriptorHeap(),
				ResourceManager::GetInstance()->GetComputeDescriptorHeap());
		}

		FFMeshData& meshData = ResourceManager::GetInstance()->GetMeshDataFromMeshName(meshName);

		uint32 animIndex = 0;
		for (size_t i = 0; i < meshData.vecAnimClip.size(); ++i)
		{
			if (curAnimName == meshData.vecAnimClip[i].animName)
			{
				animIndex = i;
				break;
			}
		}

		mesh->GetBoneFrameDataBuffer(animIndex)->PushComputeSRVData(SRV_REGISTER::t8);
		mesh->GetBoneOffsetBuffer()->PushComputeSRVData(SRV_REGISTER::t9);

		mapAnimData[id].second->PushComputeUAVData(UAV_REGISTER::u0);

		mapAnimData[id].first->SetInt(0, boneCount);
		mapAnimData[id].first->SetInt(1, frame);
		mapAnimData[id].first->SetInt(2, nextFrame);
		mapAnimData[id].first->SetFloat(0, frameRatio);

		uint32 groupCount = (boneCount / 256) + 1;
		mapAnimData[id].first->Dispatch(groupCount, 1, 1);

		mapAnimData[id].second->PushGraphicsData(SRV_REGISTER::t7);

		TransformParams transformParams = {};
		transformParams.matWorld = wtm;
		transformParams.matView = CameraManager::GetInstance().GetCamera(0)->matView;
		transformParams.matProjection = CameraManager::GetInstance().GetCamera(0)->matPerspectiveProjection;
		transformParams.matWV = transformParams.matWorld * transformParams.matView;
		transformParams.matWVP = transformParams.matWV * transformParams.matProjection;
		transformParams.matViewInv = CameraManager::GetInstance().GetCamera(0)->matView.Invert();

		(*m_constantBuffers)[static_cast<uint8>(CONSTANT_BUFFER_TYPE::TRANSFORM)]->PushGraphicsData(&transformParams, sizeof(transformParams));

		material->GetMaterialDesc().floatParams[1] = TimeManager::GetInstance()->GetAccTIme();
		material->SetMaterialPramsForDesc(material->GetMaterialDesc());
		material->PushGraphicsData();
		mesh->Render(false, 1, materialIndex);
	}

	// Sub Effect Skinning Animation
	for (auto& i : m_effect)
	{
		Frustum* frustum = CameraManager::GetInstance().GetCamera(0)->GetFrustum();

		unsigned __int64 id = i.animatorID;
		__int32 frame = i.curFrame;
		__int32 nextFrame = i.nextFrame;
		float frameRatio = i.ratio;
		wstring curAnimName = i.curAnimationName;
		Matrix wtm = i.WTM;
		wstring meshName = i.meshName;
		MATERIAL_DESC desc = i.materialDesc;
		wstring& materialName = i.materialName;
		int materialIndex = i.materialIndex;

		Mesh* mesh = ResourceManager::GetInstance()->GetMesh(meshName);
		Material* material = ResourceManager::GetInstance()->GetMaterial(desc, materialName);

		if (!frustum->ContainSphere(wtm, mesh->GetBoundRadius()))
		{
			continue;
		}

		map<uint64, pair<Material*, StructuredBuffer*>>& mapAnimData = ResourceManager::GetInstance()->GetAnimData();

		auto iter = mapAnimData.find(id);

		if (iter == mapAnimData.end())
		{
			Shader* shader = ResourceManager::GetInstance()->GetShader(L"ComputeAnimation");
			Material* material = new Material(m_device, m_constantBuffers,
				ResourceManager::GetInstance()->GetGraphicsDescriptorHeap(),
				ResourceManager::GetInstance()->GetComputeDescriptorHeap(),
				ResourceManager::GetInstance()->GetComputeCommandQueue());

			material->SetShader(shader);
			mapAnimData.insert({ id, {material, new StructuredBuffer()} });
		}

		uint32 boneCount = mesh->GetBoneInfo().size();

		if (mapAnimData[id].second->GetElementCount() < boneCount)
		{
			mapAnimData[id].second->Init(sizeof(Matrix), boneCount, m_device,
				ResourceManager::GetInstance()->GetGraphicsDescriptorHeap(),
				ResourceManager::GetInstance()->GetComputeDescriptorHeap());
		}

		FFMeshData& meshData = ResourceManager::GetInstance()->GetMeshDataFromMeshName(meshName);

		uint32 animIndex = 0;
		for (size_t i = 0; i < meshData.vecAnimClip.size(); ++i)
		{
			if (curAnimName == meshData.vecAnimClip[i].animName)
			{
				animIndex = i;
				break;
			}
		}

		mesh->GetBoneFrameDataBuffer(animIndex)->PushComputeSRVData(SRV_REGISTER::t8);
		mesh->GetBoneOffsetBuffer()->PushComputeSRVData(SRV_REGISTER::t9);

		mapAnimData[id].second->PushComputeUAVData(UAV_REGISTER::u0);

		mapAnimData[id].first->SetInt(0, boneCount);
		mapAnimData[id].first->SetInt(1, frame);
		mapAnimData[id].first->SetInt(2, nextFrame);
		mapAnimData[id].first->SetFloat(0, frameRatio);

		uint32 groupCount = (boneCount / 256) + 1;
		mapAnimData[id].first->Dispatch(groupCount, 1, 1);

		mapAnimData[id].second->PushGraphicsData(SRV_REGISTER::t7);

		TransformParams transformParams = {};
		transformParams.matWorld = wtm;
		transformParams.matView = CameraManager::GetInstance().GetCamera(0)->matView;
		transformParams.matProjection = CameraManager::GetInstance().GetCamera(0)->matPerspectiveProjection;
		transformParams.matWV = transformParams.matWorld * transformParams.matView;
		transformParams.matWVP = transformParams.matWV * transformParams.matProjection;
		transformParams.matViewInv = CameraManager::GetInstance().GetCamera(0)->matView.Invert();

		(*m_constantBuffers)[static_cast<uint8>(CONSTANT_BUFFER_TYPE::TRANSFORM)]->PushGraphicsData(&transformParams, sizeof(transformParams));

		material->PushGraphicsData();
		mesh->Render(false, 1, materialIndex);
	}
}

void RenderSystem::AddInstanceParams(uint64 p_instanceID, InstancingParams& p_params)
{
	if (m_buffers.find(p_instanceID) == m_buffers.end())
	{
		m_buffers[p_instanceID] = new InstancingBuffer(m_device);
	}

	m_buffers[p_instanceID]->AddParams(p_params);
}
