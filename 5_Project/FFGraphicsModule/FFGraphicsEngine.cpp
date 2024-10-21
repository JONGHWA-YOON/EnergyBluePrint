#include "GraphicsPch.h"
#include "FFGraphicsEngine.h"

#include "Device.h"
#include "CommandQueue.h"
#include "SwapChain.h"
#include "RootSignature.h"
#include "ConstantBuffer.h"
#include "TableDescriptorHeap.h"
#include "Material.h"
#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"
#include "ResourceManager.h"
#include "RenderTargetGroup.h"
#include "Structuredbuffer.h"
#include "InstancingBuffer.h"

#include "RenderSystem.h"
#include "CameraManager.h"
#include "FFCamera.h"

#include "Frustum.h"

#include "TimeManager.h"

FFGraphicsEngine::~FFGraphicsEngine()
{
	delete m_device;
	delete m_graphicsCmdQueue;
	delete m_computeCmdQueue;
	delete m_swapChain;
	delete m_rootSignature;
	delete m_graphicsDescriptor;
	delete m_computeDescriptor;

	for (auto& i : m_constantBuffers)
	{
		delete i;
	}

	for (auto& i : m_renderTargetGroup)
	{
		delete i;
	}
}

void FFGraphicsEngine::Init(HWND p_hWnd, int p_width, int p_height, bool p_windowed)
{
	m_windowInfo = { p_hWnd, p_width, p_height, p_windowed };
	m_viewPort = { 0,0,static_cast<FLOAT>(m_windowInfo.width),static_cast<FLOAT>(m_windowInfo.height),0.0f,1.0f };
	m_scissorRect = CD3DX12_RECT(0, 0, m_windowInfo.width, m_windowInfo.height);

	m_device = new Device();
	m_graphicsCmdQueue = new GraphicsCommandQueue();
	m_computeCmdQueue = new ComputeCommandQueue();
	m_swapChain = new SwapChain();
	m_rootSignature = new RootSignature();
	m_graphicsDescriptor = new GraphicsDescriptorHeap();
	m_computeDescriptor = new ComputeDescriptorHeap();

	// Device와 DXGIFactory를 만든다.
	m_device->Init();

	// 커맨드큐, 커맨드 리스트, 커맨드 알로케이터를 만든다.
	// 스왑체인을 넘기는 이유는 스왑체인이 가지고 있는 백버퍼를 가져오기 위함이다.
	// 초기화 과정에는 쓰이지 않으며 RenderBegin, RenderEnd할 때 백버퍼의 상태를 변경하는 명령을
	// 커맨드 리스트에 추가하기 위함이다.
	m_graphicsCmdQueue->Init(m_device, &m_constantBuffers, m_graphicsDescriptor, m_swapChain,
		&m_renderTargetGroup, m_rootSignature);

	m_computeCmdQueue->Init(m_device, m_rootSignature);

	// info는 화면 해상도 및 창모드 관련으로 버퍼들을 만들기 위함이다.
	// Device를 넘기는 이유는 백버퍼를 서술하는 DescriptorHeap을 만들기 위함이다.
	// 만들어진 서술자 힙으로 렌더타겟뷰도 만든다.
	// 여기서 넘기는 DXGI는 Factory인데 스왑체인이 DXGI API의 일부이기 때문이다.
	m_swapChain->Init(m_device, m_graphicsCmdQueue,m_windowInfo);

	// 셰이더 코드에서 사용되는 (상수)버퍼들에 대해서 어떤식으로 쓸지만 설정
	m_rootSignature->Init(m_device, m_computeCmdQueue);

	m_graphicsDescriptor->Init(256, m_device, m_graphicsCmdQueue);
	m_computeDescriptor->Init(m_device, m_computeCmdQueue);

	CreateConstantBuffer(CBV_REGISTER::b0, sizeof(LightParams), 1);
	CreateConstantBuffer(CBV_REGISTER::b1, sizeof(TransformParams), 256);
	CreateConstantBuffer(CBV_REGISTER::b2, sizeof(MaterialParams), 256);

	//ResizeWindow(p_width, p_height);

	

	ResourceManager::GetInstance()->Init(m_device, m_graphicsCmdQueue, m_computeCmdQueue,
		m_rootSignature, m_graphicsDescriptor, m_computeDescriptor, &m_constantBuffers, &m_renderTargetGroup);

	CreateRenderTargetGroup();

	ResourceManager::GetInstance()->CrateDefaultResource();

	RenderSystem::GetInstance()->Init(m_device, m_swapChain, &m_renderTargetGroup, &m_constantBuffers);
}

void FFGraphicsEngine::Init(long long p_hWnd, float p_width, float p_height, bool p_isWindowed)
{
	Init((HWND)p_hWnd,p_width, p_height, p_isWindowed);
}

void FFGraphicsEngine::RenderBegin(float p_deltaTime)
{
	m_isDrawDefereedDebug = false;
	RenderSystem::GetInstance()->ClearRenderObjects();
	RenderSystem::GetInstance()->ClearInstanceBuffer();
	ResourceManager::GetInstance()->ClearRefCount();
	m_graphicsCmdQueue->RenderBegin(&m_viewPort, &m_scissorRect);

	TimeManager::GetInstance()->SetDeltaTime(p_deltaTime);
}

void FFGraphicsEngine::RenderEnd()
{
	RenderSystem::GetInstance()->Render();

	if (m_isDrawDefereedDebug)
	{
		DrawDeferredDebugInfo();
	}

	m_graphicsCmdQueue->RenderEnd();

	ResourceManager::GetInstance()->CheckUsingMaterial();
}

void FFGraphicsEngine::ClearRenderTargetView(RENDER_TARGET_TYPE p_type)
{
	int8 backIndex = m_swapChain->GetBackBufferIndex();
	if (p_type == RENDER_TARGET_TYPE::SWAP_CHAIN)
	{
		m_renderTargetGroup[static_cast<uint32>(p_type)]->ClearRenderTargetView(backIndex);
	}
	else if (p_type == RENDER_TARGET_TYPE::G_BUFFER)
	{
		m_renderTargetGroup[static_cast<uint32>(p_type)]->ClearRenderTargetView();
	}
	else if (p_type == RENDER_TARGET_TYPE::LIGHTING)
	{
		m_renderTargetGroup[static_cast<uint32>(p_type)]->ClearRenderTargetView();
	}
}

void FFGraphicsEngine::OMSetRenderTargets(RENDER_TARGET_TYPE p_type)
{
	int8 backIndex = m_swapChain->GetBackBufferIndex();
	if (p_type == RENDER_TARGET_TYPE::SWAP_CHAIN)
	{
		m_renderTargetGroup[static_cast<uint32>(p_type)]->OMSetRenderTargets(1,backIndex);
	}
	else if (p_type == RENDER_TARGET_TYPE::G_BUFFER)
	{
		m_renderTargetGroup[static_cast<uint32>(p_type)]->OMSetRenderTargets();
	}
	else if (p_type == RENDER_TARGET_TYPE::LIGHTING)
	{
		m_renderTargetGroup[static_cast<uint32>(p_type)]->OMSetRenderTargets();
	}
}

void FFGraphicsEngine::WaitTargetToResource(RENDER_TARGET_TYPE p_type)
{
	if (p_type == RENDER_TARGET_TYPE::G_BUFFER)
	{
		m_renderTargetGroup[static_cast<uint32>(p_type)]->WaitTargetToResource();
	}
	else if (p_type == RENDER_TARGET_TYPE::LIGHTING)
	{
		m_renderTargetGroup[static_cast<uint32>(p_type)]->WaitTargetToResource();
	}
}

void FFGraphicsEngine::CreateStructuredBuffer(uint32 p_id, uint32 p_size, uint32 p_count)
{
	ResourceManager::GetInstance()->CreateStructureBuffer(p_id, p_size, p_count);
}

void FFGraphicsEngine::PushComputeUAVData(uint32 p_id, UAV_REGISTER p_reg)
{
	ResourceManager::GetInstance()->GetStructuredBuffer(p_id)->PushComputeUAVData(p_reg);
}

void FFGraphicsEngine::PushGraphicsData(uint32 p_id, SRV_REGISTER p_reg)
{
	ResourceManager::GetInstance()->GetStructuredBuffer(p_id)->PushGraphicsData(p_reg);
}

void FFGraphicsEngine::Dispatch(MATERIAL_DESC m_materialDesc, uint32 p_x, uint32 p_y, uint32 p_z)
{
	Material* temp = ResourceManager::GetInstance()->GetMaterial(m_materialDesc, m_materialDesc.materialName);
	temp->Dispatch(p_x, p_y, p_z);
}

bool FFGraphicsEngine::GetInstancingBuffer(uint64 p_instanceID)
{
	auto& mapInstancingBuffer = ResourceManager::GetInstance()->GetMapInstancingBuffer(); 
	auto iter = mapInstancingBuffer.find(p_instanceID);
	if (iter == mapInstancingBuffer.end())
	{
		return false;
	}

	return true;
}

void FFGraphicsEngine::PushInstancingBufferData(uint64 p_id, InstancingParams& p_data)
{
	auto& mapInstancingBuffer = ResourceManager::GetInstance()->GetMapInstancingBuffer();
	mapInstancingBuffer[p_id]->AddParams(p_data);
}

void FFGraphicsEngine::ClearInstancingBuffer()
{

}

void FFGraphicsEngine::DrawFinal()
{
	ResourceManager::GetInstance()->GetMaterial(L"FinalMaterial")->PushGraphicsData();
	ResourceManager::GetInstance()->GetMesh(L"Rectangle")->Render(true);
}

void FFGraphicsEngine::CreateConstantBuffer(CBV_REGISTER p_reg, uint32 p_bufferSize, uint32 p_count)
{
	uint8 typeInt = static_cast<uint8>(p_reg);
	assert(m_constantBuffers.size() == typeInt);

	ConstantBuffer* buffer = new ConstantBuffer();
	buffer->Init(p_reg, p_bufferSize, p_count, m_device, m_graphicsDescriptor, m_computeDescriptor, m_graphicsCmdQueue);
	m_constantBuffers.push_back(buffer);
}

void FFGraphicsEngine::CreateRenderTargetGroup()
{
	// DepthStencil
	m_dsTexture = ResourceManager::GetInstance()->CreateTexture(L"DepthStencil",
		DXGI_FORMAT_D32_FLOAT, m_windowInfo.width,m_windowInfo.height,
		CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);

	// SwapChain
	{
		vector<RenderTarget> rtVec(SWAP_CHAIN_BUFFER_COUNT);

		for (uint32 i = 0; i < SWAP_CHAIN_BUFFER_COUNT; ++i)
		{
			wstring name = L"SwapChainTarget_" + to_wstring(i);

			ComPtr<ID3D12Resource> resource;
			m_swapChain->GetSwapChain()->GetBuffer(i, IID_PPV_ARGS(&resource));
			rtVec[i].texture = ResourceManager::GetInstance()->CreateTextureFromResource(name, resource);
		}

		m_renderTargetGroup[static_cast<uint8>(RENDER_TARGET_TYPE::SWAP_CHAIN)] = new RenderTargetGroup();
		m_renderTargetGroup[static_cast<uint8>(RENDER_TARGET_TYPE::SWAP_CHAIN)]->Create(RENDER_TARGET_TYPE::SWAP_CHAIN, rtVec, m_dsTexture, m_device, m_graphicsCmdQueue);
	}

	// Shadow
	{
		vector<RenderTarget> rtVec(SHADOW_MEMBER_COUNT);

		rtVec[0].texture = ResourceManager::GetInstance()->CreateTexture(L"ShadowTarget",
			DXGI_FORMAT_R32G32B32A32_FLOAT, 1080, 1080,
			CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);

		Texture* shadowDepthTexture = ResourceManager::GetInstance()->CreateTexture(L"ShadowDepthStencil",
			DXGI_FORMAT_D32_FLOAT, 1080, 1080,
			CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);

		m_renderTargetGroup[static_cast<uint8>(RENDER_TARGET_TYPE::SHADOW)] = new RenderTargetGroup();
		m_renderTargetGroup[static_cast<uint8>(RENDER_TARGET_TYPE::SHADOW)]->Create(RENDER_TARGET_TYPE::SHADOW, rtVec, shadowDepthTexture, m_device, m_graphicsCmdQueue);
	}

	// Deferred
	{
		vector<RenderTarget> rtVec(G_BUFFER_MEMBER_COUNT);

		rtVec[0].texture = ResourceManager::GetInstance()->CreateTexture(L"PositionTarget",
			DXGI_FORMAT_R32G32B32A32_FLOAT, m_windowInfo.width, m_windowInfo.height,
			CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);

		rtVec[1].texture = ResourceManager::GetInstance()->CreateTexture(L"NormalTarget",
			DXGI_FORMAT_R32G32B32A32_FLOAT, m_windowInfo.width, m_windowInfo.height,
			CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);

		rtVec[2].texture = ResourceManager::GetInstance()->CreateTexture(L"DiffuseTarget",
			DXGI_FORMAT_R8G8B8A8_UNORM, m_windowInfo.width, m_windowInfo.height,
			CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);

		rtVec[3].texture = ResourceManager::GetInstance()->CreateTexture(L"DepthTarget",
			DXGI_FORMAT_R8G8B8A8_UNORM, m_windowInfo.width, m_windowInfo.height,
			CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);

		m_renderTargetGroup[static_cast<uint8>(RENDER_TARGET_TYPE::G_BUFFER)] = new RenderTargetGroup();
		m_renderTargetGroup[static_cast<uint8>(RENDER_TARGET_TYPE::G_BUFFER)]->Create(RENDER_TARGET_TYPE::G_BUFFER, rtVec, m_dsTexture ,m_device, m_graphicsCmdQueue);
	}

	// Light
	{
		vector<RenderTarget> rtVec(LIGHTING_GROUP_MEMBER_COUNT);

		rtVec[0].texture = ResourceManager::GetInstance()->CreateTexture(L"DiffuseLightTarget",
			DXGI_FORMAT_R8G8B8A8_UNORM, m_windowInfo.width, m_windowInfo.height,
			CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);

		rtVec[1].texture = ResourceManager::GetInstance()->CreateTexture(L"SpecularLightTarget",
			DXGI_FORMAT_R8G8B8A8_UNORM, m_windowInfo.width, m_windowInfo.height,
			CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);

		m_renderTargetGroup[static_cast<uint8>(RENDER_TARGET_TYPE::LIGHTING)] = new RenderTargetGroup();
		m_renderTargetGroup[static_cast<uint8>(RENDER_TARGET_TYPE::LIGHTING)]->Create(RENDER_TARGET_TYPE::LIGHTING,rtVec, m_dsTexture, m_device, m_graphicsCmdQueue);
	}
}

void FFGraphicsEngine::DrawDeferredDebugInfo()
{
	for (int i = 0; i < 7; i++)
	{
		Matrix matSclae = Matrix::CreateScale(Vec3(m_windowInfo.width / 10.f, m_windowInfo.height / 10.f, 1.f));
		Matrix matRotation = Matrix::CreateRotationX(0.f);
		matRotation *= Matrix::CreateRotationY(0.f);
		matRotation *= Matrix::CreateRotationZ(0.f);
		Matrix matTranslation = Matrix::CreateTranslation(Vec3(-865.f + (i * 200), 466.f, 500.f));
		Matrix wtm = matSclae * matRotation * matTranslation;

		if (i == 0)
			ResourceManager::GetInstance()->GetMaterial(L"DeferredPosition")->PushGraphicsData();
		else if (i == 1)
			ResourceManager::GetInstance()->GetMaterial(L"DeferredNormal")->PushGraphicsData();
		else if (i == 2)
			ResourceManager::GetInstance()->GetMaterial(L"DeferredColor")->PushGraphicsData();
		else if (i == 3)
			ResourceManager::GetInstance()->GetMaterial(L"DeferredDepth")->PushGraphicsData();
		else if (i == 4)
			ResourceManager::GetInstance()->GetMaterial(L"DeferredDiffuseLight")->PushGraphicsData();
		else if (i == 5)
			ResourceManager::GetInstance()->GetMaterial(L"DeferredSpecularLight")->PushGraphicsData();
		else if (i == 6)
		{
			matSclae = Matrix::CreateScale(Vec3(m_windowInfo.width / 10.f, m_windowInfo.width / 10.f, 1.f));
			matTranslation = Matrix::CreateTranslation(Vec3(-865.f + (i * 200), 424.f, 500.f));
			wtm = matSclae * matRotation * matTranslation;
			ResourceManager::GetInstance()->GetMaterial(L"ShadowTarget")->PushGraphicsData();
		}

		TransformParams params = {};
		params.matWVP = wtm * Matrix::Identity * CameraManager::GetInstance().GetCamera(0)->matOrthographicProjection;

		GetConstantBuffer(CONSTANT_BUFFER_TYPE::TRANSFORM)->PushGraphicsData(&params, sizeof(params));
		ResourceManager::GetInstance()->GetMesh(L"Rectangle")->Render();
	}
}

void FFGraphicsEngine::DrawMesh(const FFMatrix& p_WTM, const wstring& p_mesh, const FFMATERIAL_DESC& p_material, wstring& m_materiaName, int p_materialIndex)
{
	Matrix _wtm = {
		p_WTM._11, p_WTM._12, p_WTM._13, p_WTM._14,
		p_WTM._21, p_WTM._22, p_WTM._23, p_WTM._24,
		p_WTM._31, p_WTM._32, p_WTM._33, p_WTM._34,
		p_WTM._41, p_WTM._42, p_WTM._43, p_WTM._44,
	};

	MATERIAL_DESC _desc = {};
	_desc.ChangeFromFFMATERIAL_DESC(p_material);

	ResourceManager::GetInstance()->AddRefCount(m_materiaName);
	//
	ResourceManager::GetInstance()->SetMaterialDescForMapMaterial(_desc, m_materiaName);

	RenderSystem::GetInstance()->PushData(_wtm, p_mesh, _desc, m_materiaName, p_materialIndex);
}

//void FFGraphicsEngine::DrawSkinnedMesh(const unsigned __int64 p_id, const std::wstring& p_currentAnimationName,const FF::FFMatrix& p_WTM, const std::wstring& p_mesh, const FF::FFMATERIAL_DESC& p_material, std::wstring& p_materialName)
//{
//	// id가 0이라는건 스킨드 메쉬인데 애니메이터가 안붙은 상태기에 그냥 DrawMesh를 한다.
//	if (p_id == 0 || p_currentAnimationName == L"")
//	{
//		DrawMesh(p_WTM, p_mesh, p_material, p_materialName);
//	}
//	else
//	{
//#pragma region Animation 계산
//		// Animation 계산도 여기서 이루어진다.
//		Mesh* mesh = ResourceManager::GetInstance()->GetMesh(p_mesh);
//
//		uint32 boneCount = mesh->GetBoneInfo().size();
//		if (ResourceManager::GetInstance()->GetAnimationData(p_id).second->GetElementCount() < boneCount)
//		{
//			ResourceManager::GetInstance()->GetAnimationData(p_id).second->Init(sizeof(Matrix), boneCount, m_device, m_graphicsDescriptor, m_computeDescriptor);
//		}
//
//		FFMeshData& meshData = ResourceManager::GetInstance()->GetMeshDataFromMeshName(mesh->GetName());
//
//		uint32 animIndex = 0;
//		for (size_t i = 0; i < meshData.vecAnimClip.size(); ++i)
//		{
//			if (p_currentAnimationName == meshData.vecAnimClip[i].animName)
//			{
//				animIndex = i;
//				break;
//			}
//		}
//
//		mesh->GetBoneFrameDataBuffer(animIndex)->PushComputeSRVData(SRV_REGISTER::t8);
//		mesh->GetBoneOffsetBuffer()->PushComputeSRVData(SRV_REGISTER::t9);
//		ResourceManager::GetInstance()->GetAnimationData(p_id).second->PushComputeUAVData(UAV_REGISTER::u0);
//
//		Material* temp = ResourceManager::GetInstance()->GetAnimationData(p_id).first;
//
//		temp->SetInt(0, boneCount);
//
//		uint32 groupCount = (boneCount / 256) + 1;
//		ResourceManager::GetInstance()->GetAnimationData(p_id).first->Dispatch(groupCount, 1, 1);
//
//		ResourceManager::GetInstance()->GetAnimationData(p_id).second->PushGraphicsData(SRV_REGISTER::t7);
//
//#pragma endregion
//		Matrix _wtm = {
//			p_WTM._11, p_WTM._12, p_WTM._13, p_WTM._14,
//			p_WTM._21, p_WTM._22, p_WTM._23, p_WTM._24,
//			p_WTM._31, p_WTM._32, p_WTM._33, p_WTM._34,
//			p_WTM._41, p_WTM._42, p_WTM._43, p_WTM._44,
//		};
//
//		MATERIAL_DESC _desc = {};
//		_desc.ChangeFromFFMATERIAL_DESC(p_material);
//
//		ResourceManager::GetInstance()->AddRefCount(p_materialName);
//		//
//		ResourceManager::GetInstance()->SetMaterialDescForMapMaterial(_desc, p_materialName);
//
//		RenderSystem::GetInstance()->PushData(_wtm, p_mesh, _desc, p_materialName);
//	}
//}

void FFGraphicsEngine::DrawParticle(const FFMatrix& p_WTM, const wstring& p_mesh, const FFMATERIAL_DESC& p_material)
{
	Matrix _wtm = {
		p_WTM._11, p_WTM._12, p_WTM._13, p_WTM._14,
		p_WTM._21, p_WTM._22, p_WTM._23, p_WTM._24,
		p_WTM._31, p_WTM._32, p_WTM._33, p_WTM._34,
		p_WTM._41, p_WTM._42, p_WTM._43, p_WTM._44,
	};

	MATERIAL_DESC _desc = {};
	_desc.ChangeFromFFMATERIAL_DESC(p_material);

	//RenderSystem::GetInstance()->PushData(_wtm, p_mesh, _desc);
}

void FFGraphicsEngine::DrawDeferredDebug()
{
	m_isDrawDefereedDebug = true;
}

void FFGraphicsEngine::SetCameraInfo(unsigned __int32 p_cameraIndex, float p_naer, float p_far, float p_fov, float p_scale, float p_width, float p_height, const FFMatrix& p_WTM)
{
	FFCamera* camera = CameraManager::GetInstance().GetCamera(p_cameraIndex);
	camera->SetCameraInfo(p_naer, p_far, p_fov, p_scale);
	camera->SetResolution(p_width, p_height);
	Matrix _wtm =
	{
		p_WTM._11, p_WTM._12, p_WTM._13, p_WTM._14,
		p_WTM._21, p_WTM._22, p_WTM._23, p_WTM._24,
		p_WTM._31, p_WTM._32, p_WTM._33, p_WTM._34,
		p_WTM._41, p_WTM._42, p_WTM._43, p_WTM._44,
	};
	camera->CalculateMatrix(_wtm);

	// 카메라 정보가 갱신되었을 때 카메라가 가지고 있는 프러스텀도 갱신해준다.
	camera->GetFrustum()->UpdateFrustum();
}

void FFGraphicsEngine::SetLightData(void* p_buffer, unsigned __int32 p_size)
{
	LightParams* params = reinterpret_cast<LightParams*>(p_buffer);
	RenderSystem::GetInstance()->SetLightParams(params);
	GetConstantBuffer(CONSTANT_BUFFER_TYPE::GLOBAL)->SetGlobalData(p_buffer, p_size);
}

void FFGraphicsEngine::ResizeWindow(__int32 p_width, __int32 p_height)
{

}

void FFGraphicsEngine::CreateMesh(const std::wstring& p_path)
{
	ResourceManager::GetInstance()->CreateMesh(p_path);
}

void FFGraphicsEngine::CreateShader(const std::wstring& p_path, const ShaderInfo& p_info)
{
	ResourceManager::GetInstance()->CreateShader(p_path, p_info);
}

void FFGraphicsEngine::CreateTexture(const std::wstring& p_path)
{
	ResourceManager::GetInstance()->CreateTexture(p_path);
}

void FFGraphicsEngine::CreateMaterial(const std::wstring& p_path, const FFMATERIAL_DESC& p_desc)
{
	ResourceManager::GetInstance()->CreateMaterial(p_path, p_desc);
}

void FFGraphicsEngine::LoadFBX(const std::wstring& p_path, const std::wstring& p_shaderPath)
{
	ResourceManager::GetInstance()->LoadFBX(p_path, p_shaderPath);
}

std::vector<FF::FFMeshData>& FFGraphicsEngine::GetMeshDataFromFBX(const std::wstring& p_path)
{
	return ResourceManager::GetInstance()->GetMeshDataFromFBX(p_path);
}

void FFGraphicsEngine::PushAnimationData(unsigned __int64 p_id, __int32 p_frame, __int32 p_nextFrame, float p_frameRatio)
{
	ResourceManager::GetInstance()->PushAnimationData(p_id, p_frame, p_nextFrame, p_frameRatio);
}

void FFGraphicsEngine::DrawSkinnedMesh(unsigned __int64 p_id, __int32 p_frame, __int32 p_nextFrame, float p_frameRatio, const std::wstring& p_currentAnimationName, const FF::FFMatrix& p_WTM, const std::wstring& p_mesh, const FF::FFMATERIAL_DESC& p_material, std::wstring& p_materialName, int p_materialIndex)
{
	Matrix _wtm = {
			p_WTM._11, p_WTM._12, p_WTM._13, p_WTM._14,
			p_WTM._21, p_WTM._22, p_WTM._23, p_WTM._24,
			p_WTM._31, p_WTM._32, p_WTM._33, p_WTM._34,
			p_WTM._41, p_WTM._42, p_WTM._43, p_WTM._44,
	};

	MATERIAL_DESC _desc = {};
	_desc.ChangeFromFFMATERIAL_DESC(p_material);

	wstring animationName = p_currentAnimationName;

	ResourceManager::GetInstance()->AddRefCount(p_materialName);

	RenderSystem::GetInstance()->DrawSkinnedMesh(p_id, p_frame, p_nextFrame, p_frameRatio, animationName, _wtm, p_mesh, _desc, p_materialName,p_materialIndex);
}

unsigned __int64 FFGraphicsEngine::GetGPUHandle()
{
	return m_graphicsDescriptor->GetGPUHandle();
}
