#include "GraphicsPch.h"
#include "FBXLoader.h"
#include "Mesh.h"
#include "ResourceManager.h"
#include "Shader.h"
#include "Material.h"

FBXLoader::FBXLoader()
{
	
}

FBXLoader::~FBXLoader()
{
	if (_scene)
		_scene->Destroy();
	if (_manager)
		_manager->Destroy();
}

void FBXLoader::Init(Device* p_device, vector<ConstantBuffer*>* p_constantBuffers, GraphicsDescriptorHeap* p_graphicsDescriptorHeap, ComputeDescriptorHeap* p_computeDescriptorHeap, ComputeCommandQueue* p_computeCommandQueue)
{
	m_device = p_device;
	m_constantBuffers = p_constantBuffers;
	m_graphicsDescriptorHeap = p_graphicsDescriptorHeap;
	m_computeDescriptorHeap = p_computeDescriptorHeap;
	m_computeCommandQueue = p_computeCommandQueue;
}

void FBXLoader::LoadFbx(const wstring& path, const wstring& p_shaderPath)
{
	// 현재 로드하는 모델은 이 셰이더로 렌더한다.
	m_shaderPath = p_shaderPath;

	// 파일 데이터 로드
	Import(path);

	// Animation	
	// 여기선 FbxBoneInfo 구조체의 본 이름과 부모 본의 인덱스만 저장
	LoadBones(_scene->GetRootNode());
	

	// 로드된 데이터 파싱 (Mesh/Material/Skin)
	ParseNode(_scene->GetRootNode());

	// 엔진 구조에 맞게 Texture / Material 생성
	CreateTextures();
	CreateMaterials();
}

void FBXLoader::Import(const wstring& path)
{
	// FBX SDK 관리자 객체 생성
	_manager = FbxManager::Create();

	// IOSettings 객체 생성 및 설정
	FbxIOSettings* settings = FbxIOSettings::Create(_manager, IOSROOT);
	_manager->SetIOSettings(settings);

	// FbxImporter 객체 생성
	_scene = FbxScene::Create(_manager, "");

	// 나중에 Texture 경로 계산할 때 쓸 것
	_resourceDirectory = fs::path(path).parent_path().wstring() + L"\\" + fs::path(path).filename().stem().wstring() + L".fbm";

	_importer = FbxImporter::Create(_manager, "");

	string strPath = ws2s(path);
	_importer->Initialize(strPath.c_str(), -1, _manager->GetIOSettings());

	_importer->Import(_scene);

	_scene->GetGlobalSettings().SetAxisSystem(FbxAxisSystem::DirectX);

	// 씬 내에서 삼각형화 할 수 있는 모든 노드를 삼각형화 시킨다.
	FbxGeometryConverter geometryConverter(_manager);
	geometryConverter.Triangulate(_scene, true);

	_importer->Destroy();
}

void FBXLoader::ParseNode(FbxNode* node)
{
	FbxNodeAttribute* attribute = node->GetNodeAttribute();

	if (attribute)
	{
		switch (attribute->GetAttributeType())
		{
			// 메쉬를 로드한다.
			case FbxNodeAttribute::eMesh:
				LoadMesh(node->GetMesh());
				break;
		}
	}

	// Material 로드
	const uint32 materialCount = node->GetMaterialCount();
	for (uint32 i = 0; i < materialCount; ++i)
	{
		FbxSurfaceMaterial* surfaceMaterial = node->GetMaterial(i);
		LoadMaterial(surfaceMaterial);
	}

	// Tree 구조 재귀 호출 (계층구조)
	const int32 childCount = node->GetChildCount();
	for (int32 i = 0; i < childCount; ++i)
		ParseNode(node->GetChild(i));
}

void FBXLoader::LoadMesh(FbxMesh* mesh)
{
	_meshes.push_back(FbxMeshInfo());
	FbxMeshInfo& meshInfo = _meshes.back();

	// 메쉬이름 셋팅
	meshInfo.name = s2ws(mesh->GetName());

	// FbxMesh를 통해 버텍스 갯수를 가져와 MeshInfo에 있는 vertices와 boneWeights vector에 공간을 할당한다.
	const int32 vertexCount = mesh->GetControlPointsCount();

	///meshInfo.vertices.resize(vertexCount);
	meshInfo.boneWeights.resize(vertexCount);

	// Position
	FbxVector4* controlPoints = mesh->GetControlPoints();
	float maxRadius = 0.f;

	// FbxMesh에서 Node를 가져오고 가져 온 Node를 통해 Material Count를 알아낸다.
	const int32 materialCount = mesh->GetNode()->GetMaterialCount();
	meshInfo.indices.resize(materialCount);
	FbxGeometryElementMaterial* geometryElementMaterial = mesh->GetElementMaterial();

	const int32 polygonSize = mesh->GetPolygonSize(0);
	assert(polygonSize == 3);

	uint32 arrIdx[3];
	uint32 vertexCounter = 0; // 정점의 개수

	const int32 triCount = mesh->GetPolygonCount(); // 메쉬의 삼각형 개수를 가져온다

	//for (int32 i = 0; i < vertexCount; ++i)
	{
		for (int32 j = 0; j < triCount; j++) // 삼각형의 개수
		{
			for (int32 k = 0; k < 3; k++) // 삼각형은 세 개의 정점으로 구성
			{
				Vertex vertex;
				meshInfo.vertices.emplace_back(vertex);

				int32 controlPointIndex = mesh->GetPolygonVertex(j, k); // 제어점의 인덱스 추출

				// 순서가 0 2 1인 이유는 블렌더의 좌표계가 DX 좌표계와 다르기 때문 만일 모델링이 잘못나오면 이 부분을 의심하자.
				meshInfo.vertices.back().pos.x = static_cast<float>(controlPoints[controlPointIndex].mData[0]);
				meshInfo.vertices.back().pos.y = static_cast<float>(controlPoints[controlPointIndex].mData[2]);
				meshInfo.vertices.back().pos.z = static_cast<float>(controlPoints[controlPointIndex].mData[1]);
				
				arrIdx[k] = (j * 3) + k;

				GetNormal(mesh, &meshInfo, controlPointIndex, vertexCounter);
				GetTangent(mesh, &meshInfo, controlPointIndex, vertexCounter);
				GetUV(mesh, &meshInfo, controlPointIndex, mesh->GetTextureUVIndex(j, k));

				vertexCounter++;
				
				maxRadius = max(maxRadius, CalBoundRadius(meshInfo.vertices[(j * 3) + k].pos));

				_originVertexIndex[controlPointIndex].push_back(meshInfo.vertices.back());
			}

			const uint32 subsetIdx = geometryElementMaterial->GetIndexArray().GetAt(j);
			meshInfo.indices[subsetIdx].push_back(arrIdx[0]);
			meshInfo.indices[subsetIdx].push_back(arrIdx[2]);
			meshInfo.indices[subsetIdx].push_back(arrIdx[1]);
		}

		
	}
	meshInfo.boundRadius = maxRadius;

	// 여기선 FbxAnimClipInfo 구조체를 채운다
	LoadAnimationInfo(mesh);

	// Animation
	LoadAnimationData(mesh, &meshInfo);
}

void FBXLoader::LoadMaterial(FbxSurfaceMaterial* surfaceMaterial)
{
	FbxMaterialInfo material{};

	material.name = s2ws(surfaceMaterial->GetName());

	material.diffuse = GetMaterialData(surfaceMaterial, FbxSurfaceMaterial::sDiffuse, FbxSurfaceMaterial::sDiffuseFactor);
	material.ambient = GetMaterialData(surfaceMaterial, FbxSurfaceMaterial::sAmbient, FbxSurfaceMaterial::sAmbientFactor);
	material.specular = GetMaterialData(surfaceMaterial, FbxSurfaceMaterial::sSpecular, FbxSurfaceMaterial::sSpecularFactor);

	material.diffuseTexName = GetTextureRelativeName(surfaceMaterial, FbxSurfaceMaterial::sDiffuse);
	material.normalTexName = GetTextureRelativeName(surfaceMaterial, FbxSurfaceMaterial::sNormalMap);
	material.specularTexName = GetTextureRelativeName(surfaceMaterial, FbxSurfaceMaterial::sSpecular);

	_meshes.back().materials.push_back(material);
}

void FBXLoader::GetNormal(FbxMesh* mesh, FbxMeshInfo* container, int32 idx, int32 vertexCounter)
{
	if (mesh->GetElementNormalCount() == 0)
		return;

	FbxGeometryElementNormal* normal = mesh->GetElementNormal();
	uint32 normalIdx = 0;

	if (normal->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
	{
		if (normal->GetReferenceMode() == FbxGeometryElement::eDirect)
			normalIdx = vertexCounter;
		else
			normalIdx = normal->GetIndexArray().GetAt(vertexCounter);
	}
	else if (normal->GetMappingMode() == FbxGeometryElement::eByControlPoint)
	{
		if (normal->GetReferenceMode() == FbxGeometryElement::eDirect)
			normalIdx = idx;
		else
			normalIdx = normal->GetIndexArray().GetAt(idx);
	}

	FbxVector4 vec = normal->GetDirectArray().GetAt(normalIdx);
	container->vertices.back().normal.x = static_cast<float>(vec.mData[0]);
	container->vertices.back().normal.y = static_cast<float>(vec.mData[2]);
	container->vertices.back().normal.z = static_cast<float>(vec.mData[1]);
}

void FBXLoader::GetTangent(FbxMesh* mesh, FbxMeshInfo* meshInfo, int32 idx, int32 vertexCounter)
{
	if (mesh->GetElementTangentCount() == 0)
	{
		// FBX파일 안에 탄젠트 정보가 없다면 임의로 값을 넣어줌
		// 원래는 어떤 알고리즘을 써서 만들어줘야함
		// 아트한테는 탄젠트 정보를 꼭 포함해서 Export하라고하자.
		meshInfo->vertices.back().tangent.x = 1.f;
		meshInfo->vertices.back().tangent.y = 0.f;
		meshInfo->vertices.back().tangent.z = 0.f;
		return;
	}

	FbxGeometryElementTangent* tangent = mesh->GetElementTangent();
	uint32 tangentIdx = 0;

	if (tangent->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
	{
		if (tangent->GetReferenceMode() == FbxGeometryElement::eDirect)
			tangentIdx = vertexCounter;
		else
			tangentIdx = tangent->GetIndexArray().GetAt(vertexCounter);
	}
	else if (tangent->GetMappingMode() == FbxGeometryElement::eByControlPoint)
	{
		if (tangent->GetReferenceMode() == FbxGeometryElement::eDirect)
			tangentIdx = idx;
		else
			tangentIdx = tangent->GetIndexArray().GetAt(idx);
	}

	FbxVector4 vec = tangent->GetDirectArray().GetAt(tangentIdx);
	meshInfo->vertices.back().tangent.x = static_cast<float>(vec.mData[0]);
	meshInfo->vertices.back().tangent.y = static_cast<float>(vec.mData[2]);
	meshInfo->vertices.back().tangent.z = static_cast<float>(vec.mData[1]);
}

void FBXLoader::GetUV(FbxMesh* mesh, FbxMeshInfo* meshInfo, int32 idx, int32 uvIndex)
{
	FbxVector2 uv = mesh->GetElementUV()->GetDirectArray().GetAt(uvIndex);
	meshInfo->vertices.back().uv.x = static_cast<float>(uv.mData[0]);
	meshInfo->vertices.back().uv.y = 1.0f - static_cast<float>(uv.mData[1]);
}

Vec4 FBXLoader::GetMaterialData(FbxSurfaceMaterial* surface, const char* materialName, const char* factorName)
{
	FbxDouble3  material;
	FbxDouble	factor = 0.f;

	FbxProperty materialProperty = surface->FindProperty(materialName);
	FbxProperty factorProperty = surface->FindProperty(factorName);

	if (materialProperty.IsValid() && factorProperty.IsValid())
	{
		material = materialProperty.Get<FbxDouble3>();
		factor = factorProperty.Get<FbxDouble>();
	}

	Vec4 ret = Vec4(
		static_cast<float>(material.mData[0] * factor),
		static_cast<float>(material.mData[1] * factor),
		static_cast<float>(material.mData[2] * factor),
		static_cast<float>(factor));

	return ret;
}

wstring FBXLoader::GetTextureRelativeName(FbxSurfaceMaterial* surface, const char* materialProperty)
{
	string name;

	FbxProperty textureProperty = surface->FindProperty(materialProperty);
	if (textureProperty.IsValid())
	{
		uint32 count = textureProperty.GetSrcObjectCount();

		if (1 <= count)
		{
			FbxFileTexture* texture = textureProperty.GetSrcObject<FbxFileTexture>(0);
			if (texture)
				name = texture->GetRelativeFileName();
		}
	}

	return s2ws(name);
}

void FBXLoader::CreateTextures()
{
	for (size_t i = 0; i < _meshes.size(); i++)
	{
		for (size_t j = 0; j < _meshes[i].materials.size(); j++)
		{
			// DiffuseTexture
			{
				wstring relativePath = _meshes[i].materials[j].diffuseTexName.c_str();
				wstring filename = fs::path(relativePath).filename();
				wstring fullPath = _resourceDirectory + L"\\" + filename;
				if (filename.empty() == false)
				{
					ResourceManager::GetInstance()->CreateTexture(fullPath);// ->Load<Texture>(filename, fullPath);
				}
			}

			// NormalTexture
			{
				wstring relativePath = _meshes[i].materials[j].normalTexName.c_str();
				wstring filename = fs::path(relativePath).filename();
				wstring fullPath = _resourceDirectory + L"\\" + filename;
				if (filename.empty() == false)
				{
					ResourceManager::GetInstance()->CreateTexture(fullPath); //->Load<Texture>(filename, fullPath);
				}
			}

			// SpecularTexture
			{
				wstring relativePath = _meshes[i].materials[j].specularTexName.c_str();
				wstring filename = fs::path(relativePath).filename();
				wstring fullPath = _resourceDirectory + L"\\" + filename;
				if (filename.empty() == false)
				{
					ResourceManager::GetInstance()->CreateTexture(fullPath);//)->Load<Texture>(filename, fullPath);
				}
			}
		}
	}
}

void FBXLoader::CreateMaterials()
{
	for (size_t i = 0; i < _meshes.size(); i++)
	{
		for (size_t j = 0; j < _meshes[i].materials.size(); j++)
		{
			Material* material = new Material(m_device, m_constantBuffers, m_graphicsDescriptorHeap, m_computeDescriptorHeap, m_computeCommandQueue);

			MATERIAL_DESC& materialDesc = material->GetMaterialDesc();

			wstring key = _meshes[i].materials[j].name;
			materialDesc.materialName = key;
			material->SetName(key);
			material->SetShader(ResourceManager::GetInstance()->GetShader(m_shaderPath));
			materialDesc.shader = m_shaderPath;

			{
				wstring diffuseName = _meshes[i].materials[j].diffuseTexName.c_str();
				wstring filename = fs::path(diffuseName).filename();
				wstring key = filename;
				wstring fullpath = _resourceDirectory + L"\\" + filename;
				Texture* diffuseTexture = ResourceManager::GetInstance()->GetTexture(fullpath);
				if (diffuseTexture)
				{
					material->SetTexture(0, diffuseTexture);
					materialDesc.texture[0] = fullpath;
				}
			}

			{
				wstring normalName = _meshes[i].materials[j].normalTexName.c_str();
				wstring filename = fs::path(normalName).filename();
				wstring key = filename;
				wstring fullpath = _resourceDirectory + L"\\" + filename;
				Texture* normalTexture = ResourceManager::GetInstance()->GetTexture(fullpath);
				if (normalTexture)
				{
					material->SetTexture(1, normalTexture);
					materialDesc.texture[1] = fullpath;
				}
			}

			{
				wstring specularName = _meshes[i].materials[j].specularTexName.c_str();
				wstring filename = fs::path(specularName).filename();
				wstring key = filename;
				wstring fullpath = _resourceDirectory + L"\\" + filename;
				Texture* specularTexture = ResourceManager::GetInstance()->GetTexture(fullpath);
				if (specularTexture)
				{
					material->SetTexture(2, specularTexture);
					materialDesc.texture[2] = fullpath;
				}
			}

			ResourceManager::GetInstance()->AddMaterial(material->GetName(), material);
		}
	}
}

void FBXLoader::LoadBones(FbxNode* node, int32 idx, int32 parentIdx)
{
	FbxNodeAttribute* attribute = node->GetNodeAttribute();

	if (attribute && attribute->GetAttributeType() == FbxNodeAttribute::eSkeleton)
	{
		FbxBoneInfo* bone = new FbxBoneInfo();
		bone->boneName = s2ws(node->GetName());
		bone->parentIndex = parentIdx;
		_bones.push_back(bone);
	}

	const int32 childCount = node->GetChildCount();
	for (int32 i = 0; i < childCount; i++)
		LoadBones(node->GetChild(i), static_cast<int32>(_bones.size()), idx);
}

void FBXLoader::LoadAnimationInfo(FbxMesh* mesh)
{
	_scene->FillAnimStackNameArray(OUT _animNames);

	const int32 animCount = _animNames.GetCount();
	for (int32 i = 0; i < animCount; i++)
	{
		FbxAnimStack* animStack = _scene->FindMember<FbxAnimStack>(_animNames[i]->Buffer());
		if (animStack == nullptr)
			continue;

		FbxAnimClipInfo* animClip = new FbxAnimClipInfo();
		animClip->name = s2ws(animStack->GetName());
		animClip->keyFrames.resize(_bones.size()); // 키프레임은 본의 개수만큼

		FbxTakeInfo* takeInfo = _scene->GetTakeInfo(animStack->GetName());
		animClip->startTime = takeInfo->mLocalTimeSpan.GetStart();
		animClip->endTime = takeInfo->mLocalTimeSpan.GetStop();
		animClip->mode = _scene->GetGlobalSettings().GetTimeMode();

		_animClips.push_back(animClip);
	}
}

void FBXLoader::LoadAnimationData(FbxMesh* mesh, FbxMeshInfo* meshInfo)
{
	const int32 skinCount = mesh->GetDeformerCount(FbxDeformer::eSkin);
	if (skinCount <= 0 || _animClips.empty())
		return;

	meshInfo->hasAnimation = true;

	for (int32 i = 0; i < skinCount; i++)
	{
		FbxSkin* fbxSkin = static_cast<FbxSkin*>(mesh->GetDeformer(i, FbxDeformer::eSkin));

		if (fbxSkin)
		{
			FbxSkin::EType type = fbxSkin->GetSkinningType();
			if (FbxSkin::eRigid == type || FbxSkin::eLinear)
			{
				const int32 clusterCount = fbxSkin->GetClusterCount();
				for (int32 j = 0; j < clusterCount; j++)
				{
					FbxCluster* cluster = fbxSkin->GetCluster(j);
					if (cluster->GetLink() == nullptr)
						continue;

					int32 boneIdx = FindBoneIndex(cluster->GetLink()->GetName());
					assert(boneIdx >= 0);

					FbxAMatrix matNodeTransform = GetTransform(mesh->GetNode());
					LoadBoneWeight(cluster, boneIdx, meshInfo);
					LoadOffsetMatrix(cluster, matNodeTransform, boneIdx, meshInfo);

					const int32 animCount = _animNames.Size();
					for (int32 k = 0; k < animCount; k++)
						LoadKeyframe(k, mesh->GetNode(), cluster, matNodeTransform, boneIdx, meshInfo);
				}
			}
		}
	}

	FillBoneWeight(mesh, meshInfo);
}


void FBXLoader::FillBoneWeight(FbxMesh* mesh, FbxMeshInfo* meshInfo)
{
	const int32 size = static_cast<int32>(meshInfo->boneWeights.size());
	for (int32 v = 0; v < size; v++)
	{
		BoneWeight& boneWeight = meshInfo->boneWeights[v];
		boneWeight.Normalize();

		float animBoneIndex[4] = {};
		float animBoneWeight[4] = {};

		const int32 weightCount = static_cast<int32>(boneWeight.boneWeights.size());
		for (int32 w = 0; w < weightCount; w++)
		{
			animBoneIndex[w] = static_cast<float>(boneWeight.boneWeights[w].first);
			animBoneWeight[w] = static_cast<float>(boneWeight.boneWeights[w].second);
		}

		for (size_t i = 0; i < _originVertexIndex[v].size(); ++i)
		{
			auto iter = find(meshInfo->vertices.begin(), meshInfo->vertices.end(), _originVertexIndex[v][i]);

			int index = iter - meshInfo->vertices.begin();

			memcpy(&meshInfo->vertices[index].indices, animBoneIndex, sizeof(Vec4));
			memcpy(&meshInfo->vertices[index].weights, animBoneWeight, sizeof(Vec4));
		}
	}
}

void FBXLoader::LoadBoneWeight(FbxCluster* cluster, int32 boneIdx, FbxMeshInfo* meshInfo)
{
	const int32 indicesCount = cluster->GetControlPointIndicesCount();
	for (int32 i = 0; i < indicesCount; i++)
	{
		double weight = cluster->GetControlPointWeights()[i];
		int32 vtxIdx = cluster->GetControlPointIndices()[i];
		meshInfo->boneWeights[vtxIdx].AddWeights(boneIdx, weight);
	}
}

void FBXLoader::LoadOffsetMatrix(FbxCluster* cluster, const FbxAMatrix& matNodeTransform, int32 boneIdx, FbxMeshInfo* meshInfo)
{
	FbxAMatrix matClusterTrans;
	FbxAMatrix matClusterLinkTrans;
	// The transformation of the mesh at binding time 
	cluster->GetTransformMatrix(matClusterTrans);
	// The transformation of the cluster(joint) at binding time from joint space to world space 
	cluster->GetTransformLinkMatrix(matClusterLinkTrans);

	FbxVector4 V0 = { 1, 0, 0, 0 };
	FbxVector4 V1 = { 0, 0, 1, 0 };
	FbxVector4 V2 = { 0, 1, 0, 0 };
	FbxVector4 V3 = { 0, 0, 0, 1 };

	FbxAMatrix matReflect;
	matReflect[0] = V0;
	matReflect[1] = V1;
	matReflect[2] = V2;
	matReflect[3] = V3;

	FbxAMatrix matOffset;
	matOffset = matClusterLinkTrans.Inverse() * matClusterTrans;
	matOffset = matReflect * matOffset * matReflect;

	_bones[boneIdx]->matOffset = matOffset.Transpose();
}

void FBXLoader::LoadKeyframe(int32 animIndex, FbxNode* node, FbxCluster* cluster, const FbxAMatrix& matNodeTransform, int32 boneIdx, FbxMeshInfo* meshInfo)
{
	if (_animClips.empty())
		return;

	FbxVector4	v1 = { 1, 0, 0, 0 };
	FbxVector4	v2 = { 0, 0, 1, 0 };
	FbxVector4	v3 = { 0, 1, 0, 0 };
	FbxVector4	v4 = { 0, 0, 0, 1 };
	FbxAMatrix	matReflect;
	matReflect.mData[0] = v1;
	matReflect.mData[1] = v2;
	matReflect.mData[2] = v3;
	matReflect.mData[3] = v4;

	FbxTime::EMode timeMode = _scene->GetGlobalSettings().GetTimeMode();

	// 애니메이션 골라줌
	FbxAnimStack* animStack = _scene->FindMember<FbxAnimStack>(_animNames[animIndex]->Buffer());
	_scene->SetCurrentAnimationStack(OUT animStack);

	FbxLongLong startFrame = _animClips[animIndex]->startTime.GetFrameCount(timeMode);
	FbxLongLong endFrame = _animClips[animIndex]->endTime.GetFrameCount(timeMode);

	for (FbxLongLong frame = startFrame; frame < endFrame; frame++)
	{
		FbxKeyFrameInfo keyFrameInfo = {};
		FbxTime fbxTime = 0;

		fbxTime.SetFrame(frame, timeMode);

		FbxAMatrix matFromNode = node->EvaluateGlobalTransform(fbxTime);
		FbxAMatrix matTransform = matFromNode.Inverse() * cluster->GetLink()->EvaluateGlobalTransform(fbxTime);
		matTransform = matReflect * matTransform * matReflect;

		keyFrameInfo.time = fbxTime.GetSecondDouble();
		keyFrameInfo.matTransform = matTransform;

		_animClips[animIndex]->keyFrames[boneIdx].push_back(keyFrameInfo);
	}
}

int32 FBXLoader::FindBoneIndex(string name)
{
	wstring boneName = wstring(name.begin(), name.end());

	for (UINT i = 0; i < _bones.size(); ++i)
	{
		if (_bones[i]->boneName == boneName)
			return i;
	}

	return -1;
}

FbxAMatrix FBXLoader::GetTransform(FbxNode* node)
{
	const FbxVector4 translation = node->GetGeometricTranslation(FbxNode::eSourcePivot);
	const FbxVector4 rotation = node->GetGeometricRotation(FbxNode::eSourcePivot);
	const FbxVector4 scaling = node->GetGeometricScaling(FbxNode::eSourcePivot);
	return FbxAMatrix(translation, rotation, scaling);
}

