#include "GraphicsPch.h"
#include "Mesh.h"

//#include "FFGraphicsEngine.h"
#include "Device.h"
#include "CommandQueue.h"
#include "TableDescriptorHeap.h"
#include "ConstantBuffer.h"
#include "Material.h"
#include "ResourceManager.h"
#include "InstancingBuffer.h"

#include "FBXLoader.h"
#include "StructuredBuffer.h"

Mesh::Mesh(Device* p_device, GraphicsCommandQueue* p_graphicsCommandQueue, GraphicsDescriptorHeap* p_graphicsDescriptorHeap)
	: IResource(RESOURCE_TYPE::MESH)
{
	m_device = p_device;
	m_graphicsCommandQueue = p_graphicsCommandQueue;
	m_graphicsDescriptorHeap = p_graphicsDescriptorHeap;
}

Mesh::~Mesh()
{
	/*m_vertexBuffer->Release();
	m_vertexBuffer = 0;
	m_indexBuffer->Release();*/
}

void Mesh::Init(vector<Vertex>& p_vertexBuffer, const vector<uint32>& p_indexBuffer)
{
	CreateVertexBuffer(p_vertexBuffer);
	CreateIndexBuffer(p_indexBuffer);
}

void Mesh::Render(bool p_isFinal, uint32 p_instanceCount, uint32 idx)
{
	m_graphicsCommandQueue->GetGraphicsCmdList()->IASetVertexBuffers(0, 1, &m_vertexBufferView); 
	m_graphicsCommandQueue->GetGraphicsCmdList()->IASetIndexBuffer(&m_vecIndexInfo[idx].bufferView);

	m_graphicsDescriptorHeap->CommitTable(p_isFinal);

	m_graphicsCommandQueue->GetGraphicsCmdList()->DrawIndexedInstanced(m_vecIndexInfo[idx].count, p_instanceCount, 0, 0, 0);
}

void Mesh::Render(InstancingBuffer* p_buffer, uint32 idx)
{
	D3D12_VERTEX_BUFFER_VIEW bufferViews[] = { m_vertexBufferView, p_buffer->GetBufferView() };

	m_graphicsCommandQueue->GetGraphicsCmdList()->IASetVertexBuffers(0, 2, bufferViews);
	m_graphicsCommandQueue->GetGraphicsCmdList()->IASetIndexBuffer(&m_vecIndexInfo[idx].bufferView);

	m_graphicsDescriptorHeap->CommitTable();

	m_graphicsCommandQueue->GetGraphicsCmdList()->DrawIndexedInstanced(m_vecIndexInfo[idx].count, p_buffer->GetCount(), 0, 0, 0);
}

void Mesh::CreateFromFBX(const FbxMeshInfo* meshInfo, FBXLoader& p_loader)
{
	m_boundRadius = meshInfo->boundRadius;

	CreateVertexBuffer(meshInfo->vertices);

	for (const vector<uint32>& buffer : meshInfo->indices)
	{
		if (buffer.empty())
		{
			// FBX 파일이 이상하다. IndexBuffer가 없으면 에러 나니까 임시 처리
			vector<uint32> defaultBuffer{ 0 };
			CreateIndexBuffer(defaultBuffer);
		}
		else
		{
			CreateIndexBuffer(buffer);
		}
	}

	if (meshInfo->hasAnimation)
	{
		CreateBonesAndAnimations(p_loader);
	}

	m_name = meshInfo->name;
}

void Mesh::CreateVertexBuffer(const vector<Vertex>& p_buffer)
{
	m_vertexCount = static_cast<uint32>(p_buffer.size());
	uint32 bufferSize = m_vertexCount * sizeof(Vertex);

	D3D12_HEAP_PROPERTIES heapProperty = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

	m_device->GetDevice()->CreateCommittedResource(
		&heapProperty,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&m_vertexBuffer));

	void* vertexDataBuffer = nullptr;
	CD3DX12_RANGE readRange(0, 0); 
	m_vertexBuffer->Map(0, &readRange, &vertexDataBuffer);
	::memcpy(vertexDataBuffer, &p_buffer[0], bufferSize);
	m_vertexBuffer->Unmap(0, nullptr);

	m_vertexBufferView.BufferLocation = m_vertexBuffer->GetGPUVirtualAddress();
	m_vertexBufferView.StrideInBytes = sizeof(Vertex); // 정점 1개 크기
	m_vertexBufferView.SizeInBytes = bufferSize; // 버퍼의 크기	
}

void Mesh::CreateIndexBuffer(const vector<uint32>& p_buffer)
{
	uint32 _indexCount = static_cast<uint32>(p_buffer.size());
	uint32 bufferSize = _indexCount * sizeof(uint32);

	D3D12_HEAP_PROPERTIES heapProperty = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

	ComPtr<ID3D12Resource> indexBuffer;
	m_device->GetDevice()->CreateCommittedResource(
		&heapProperty,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&indexBuffer));

	void* indexDataBuffer = nullptr;
	CD3DX12_RANGE readRange(0, 0); 
	indexBuffer->Map(0, &readRange, &indexDataBuffer);
	::memcpy(indexDataBuffer, &p_buffer[0], bufferSize);
	indexBuffer->Unmap(0, nullptr);

	D3D12_INDEX_BUFFER_VIEW	indexBufferView;
	indexBufferView.BufferLocation = indexBuffer->GetGPUVirtualAddress();
	indexBufferView.Format = DXGI_FORMAT_R32_UINT;
	indexBufferView.SizeInBytes = bufferSize;

	indexBufferView.BufferLocation = indexBuffer->GetGPUVirtualAddress();
	indexBufferView.Format = DXGI_FORMAT_R32_UINT;
	indexBufferView.SizeInBytes = bufferSize;

	IndexBufferInfo info =
	{
		indexBuffer,
		indexBufferView,
		DXGI_FORMAT_R32_UINT,
		_indexCount
	};

	m_vecIndexInfo.push_back(info);
}

void Mesh::CreateBonesAndAnimations(class FBXLoader& loader)
{
#pragma region AnimClip
	uint32 frameCount = 0;
	vector<FbxAnimClipInfo*>& animClips = loader.GetAnimClip();
	for (FbxAnimClipInfo*& ac : animClips)
	{
		AnimClipInfo info = {};

		info.animName = ac->name;

		info.duration = ac->endTime.GetSecondDouble() - ac->startTime.GetSecondDouble();

		int32 startFrame = static_cast<int32>(ac->startTime.GetFrameCount(ac->mode));
		int32 endFrame = static_cast<int32>(ac->endTime.GetFrameCount(ac->mode));

		info.frameCount = endFrame - startFrame;

		info.keyFrames.resize(ac->keyFrames.size());

		const int32 boneCount = static_cast<int32>(ac->keyFrames.size());
		for (int32 b = 0; b < boneCount; b++)
		{
			auto& vec = ac->keyFrames[b];

			const int32 size = static_cast<int32>(vec.size());
			frameCount = max(frameCount, static_cast<uint32>(size));
			info.keyFrames[b].resize(size);

			for (int32 f = 0; f < size; f++)
			{
				FbxKeyFrameInfo& kf = vec[f];
				// FBX에서 파싱한 정보들로 채워준다
				KeyFrameInfo& kfInfo = info.keyFrames[b][f];
				kfInfo.time = kf.time;
				kfInfo.frame = static_cast<int32>(size);
				kfInfo.scale.x = static_cast<float>(kf.matTransform.GetS().mData[0]);
				kfInfo.scale.y = static_cast<float>(kf.matTransform.GetS().mData[1]);
				kfInfo.scale.z = static_cast<float>(kf.matTransform.GetS().mData[2]);
				kfInfo.rotation.x = static_cast<float>(kf.matTransform.GetQ().mData[0]);
				kfInfo.rotation.y = static_cast<float>(kf.matTransform.GetQ().mData[1]);
				kfInfo.rotation.z = static_cast<float>(kf.matTransform.GetQ().mData[2]);
				kfInfo.rotation.w = static_cast<float>(kf.matTransform.GetQ().mData[3]);
				kfInfo.translate.x = static_cast<float>(kf.matTransform.GetT().mData[0]);
				kfInfo.translate.y = static_cast<float>(kf.matTransform.GetT().mData[1]);
				kfInfo.translate.z = static_cast<float>(kf.matTransform.GetT().mData[2]);
			}
		}

		_animClips.push_back(info);
	}
#pragma endregion

#pragma region Bones
	vector<FbxBoneInfo*>& bones = loader.GetBones();
	for (FbxBoneInfo*& bone : bones)
	{
		BoneInfo boneInfo = {};
		boneInfo.parentIdx = bone->parentIndex;
		boneInfo.matOffset = GetMatrix(bone->matOffset);
		boneInfo.boneName = bone->boneName;
		_bones.push_back(boneInfo);
	}
#pragma endregion

#pragma region SkinData
	if (IsAnimMesh())
	{
		// BoneOffet 행렬
		const int32 boneCount = static_cast<int32>(_bones.size());
		vector<Matrix> offsetVec(boneCount);
		for (size_t b = 0; b < boneCount; b++)
			offsetVec[b] = _bones[b].matOffset;

		// OffsetMatrix StructuredBuffer 세팅
		_offsetBuffer = new StructuredBuffer();
		_offsetBuffer->Init(
			sizeof(Matrix), 
			static_cast<uint32>(offsetVec.size()),
			m_device, 
			ResourceManager::GetInstance()->GetGraphicsDescriptorHeap(),
			ResourceManager::GetInstance()->GetComputeDescriptorHeap(),
			offsetVec.data());

		const int32 animCount = static_cast<int32>(_animClips.size());
		for (int32 i = 0; i < animCount; i++)
		{
			AnimClipInfo& animClip = _animClips[i];

			// 애니메이션 프레임 정보
			vector<AnimFrameParams> frameParams;
			frameParams.resize(_bones.size() * animClip.frameCount);

			for (int32 b = 0; b < boneCount; b++)
			{
				const int32 keyFrameCount = static_cast<int32>(animClip.keyFrames[b].size());
				for (int32 f = 0; f < keyFrameCount; f++)
				{
					int32 idx = static_cast<int32>(boneCount * f + b);

					frameParams[idx] = AnimFrameParams
					{ 
						Vec4(animClip.keyFrames[b][f].scale), 
						animClip.keyFrames[b][f].rotation, // Quaternion
						Vec4(animClip.keyFrames[b][f].translate)
					};
				}
			}

			// StructuredBuffer 세팅
			_frameBuffer.push_back(new StructuredBuffer());
			_frameBuffer.back()->Init(
				sizeof(AnimFrameParams), 
				static_cast<uint32>(frameParams.size()),
				m_device,
				ResourceManager::GetInstance()->GetGraphicsDescriptorHeap(),
				ResourceManager::GetInstance()->GetComputeDescriptorHeap(),
				frameParams.data());
		}
	}
#pragma endregion
}

Matrix Mesh::GetMatrix(FbxAMatrix& matrix)
{
	Matrix mat;

	for (int32 y = 0; y < 4; ++y)
		for (int32 x = 0; x < 4; ++x)
			mat.m[y][x] = static_cast<float>(matrix.Get(y, x));

	return mat;
}
