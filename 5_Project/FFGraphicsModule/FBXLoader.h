#pragma once

// FBX에서 뽑아낸 Material정보를 묶은 구조체
struct FbxMaterialInfo
{
	Vec4			diffuse;
	Vec4			ambient;
	Vec4			specular;
	wstring			name;
	wstring			diffuseTexName;
	wstring			normalTexName;
	wstring			specularTexName;
};

// 스키닝 애니메이션을 할 때 필요한 본 인덱스, 가중치를 관리하는 구조체
struct BoneWeight
{
	// 인덱스, 가중치
	using Pair = pair<int32, double>;
	vector<Pair> boneWeights;

	void AddWeights(uint32 index, double weight)
	{
		if (weight <= 0.f)
			return;

		auto findIt = std::find_if(boneWeights.begin(), boneWeights.end(),
			[=](const Pair& p) { return p.second < weight; });

		if (findIt != boneWeights.end())
			boneWeights.insert(findIt, Pair(index, weight));
		else
			boneWeights.push_back(Pair(index, weight));

		// 가중치는 최대 4개
		if (boneWeights.size() > 4)
			boneWeights.pop_back();
	}

	void Normalize()
	{
		double sum = 0.f;
		std::for_each(boneWeights.begin(), boneWeights.end(), [&](Pair& p) { sum += p.second; });
		std::for_each(boneWeights.begin(), boneWeights.end(), [=](Pair& p) { p.second = p.second / sum; });
	}
};

// FBX에서 로드한 Mesh 정보 
struct FbxMeshInfo
{
	wstring								name;			// 메쉬 이름
	vector<Vertex>						vertices;		// 버텍스 배열
	vector<vector<uint32>>				indices;		// 다중 인덱스 배열
	vector<FbxMaterialInfo>				materials;		// 현재 메쉬에 적용될 머테리얼 정보
	vector<BoneWeight>					boneWeights;	// 뼈 가중치
	bool								hasAnimation;	// 애니메이션이 있는 메쉬인지
	float								boundRadius;	// 바운딩 반지름
};

// FBX에서 로드한 애니메이션 프레임 정보
struct FbxKeyFrameInfo
{
	FbxAMatrix  matTransform;	// 현재 프레임의 WTM
	double		time;			// 몇 프레임인지
};

// FBX에서 로드한 본 정보
struct FbxBoneInfo
{
	wstring					boneName;		// 본 이름
	int32					parentIndex;	// 부모 본의 인덱스
	FbxAMatrix				matOffset;		// Offset WTM
};

// FBX에서 로드한 애니메이션 클립 정보
struct FbxAnimClipInfo
{
	wstring			name;							// 애니메이션 이름
	FbxTime			startTime;						// 시작 시간
	FbxTime			endTime;						// 끝나는 시간
	FbxTime::EMode	mode;		

	// Bone0	Bone1	Bone2...
	// Mat		Mat		Mat...
	// Mat		Mat		Mat...
	vector<vector<FbxKeyFrameInfo>>	keyFrames;		// 프레임정보 다중 배열
};

class Device;
class ConstantBuffer;
class GraphicsDescriptorHeap;
class ComputeDescriptorHeap;
class ComputeCommandQueue;


class FBXLoader
{
public:
	FBXLoader();
	~FBXLoader();

public:
	void Init(Device* p_device, vector<ConstantBuffer*>* p_constantBuffers, GraphicsDescriptorHeap* p_graphicsDescriptorHeap, ComputeDescriptorHeap* p_computeDescriptorHeap, ComputeCommandQueue* p_computeCommandQueue);

	void LoadFbx(const wstring& path, const wstring& p_shaderPath);

public:
	int32 GetMeshCount() { return static_cast<int32>(_meshes.size()); }
	const FbxMeshInfo& GetMesh(int32 idx) { return _meshes[idx]; }
	vector<FbxBoneInfo*>& GetBones() { return _bones; }
	vector<FbxAnimClipInfo*>& GetAnimClip() { return _animClips; }

private:
	// 임포트 할 때 FBXManager, Scene, Texture경로, Importer 만든다.
	void Import(const wstring& path);

	// 인자로 들어온 노드에 대한걸 파싱한다.
	// 여기서 Mesh랑 Material을 로드한다.
	void ParseNode(FbxNode* root);

	// ParseNode에서 쓰이는 함수 
	// 여기서는 버텍스의 Position, Normal, UV, 인덱스정보를 로드한다.
	void LoadMesh(FbxMesh* mesh);

	// 여기서 Material정보를 로드한다.
	void LoadMaterial(FbxSurfaceMaterial* surfaceMaterial);

	void		GetNormal(FbxMesh* mesh, FbxMeshInfo* container, int32 idx, int32 vertexCounter);
	void		GetTangent(FbxMesh* mesh, FbxMeshInfo* container, int32 idx, int32 vertexCounter);
	void		GetUV(FbxMesh* mesh, FbxMeshInfo* container, int32 idx, int32 vertexCounter);
	Vec4		GetMaterialData(FbxSurfaceMaterial* surface, const char* materialName, const char* factorName);
	wstring		GetTextureRelativeName(FbxSurfaceMaterial* surface, const char* materialProperty);

	void CreateTextures();
	void CreateMaterials();

	// Animation
	void LoadBones(FbxNode* node) { LoadBones(node, 0, -1); }
	void LoadBones(FbxNode* node, int32 idx, int32 parentIdx);
	void LoadAnimationInfo(FbxMesh* mesh);

	void LoadAnimationData(FbxMesh* mesh, FbxMeshInfo* meshInfo);
	void LoadBoneWeight(FbxCluster* cluster, int32 boneIdx, FbxMeshInfo* meshInfo);
	void LoadOffsetMatrix(FbxCluster* cluster, const FbxAMatrix& matNodeTransform, int32 boneIdx, FbxMeshInfo* meshInfo);
	void LoadKeyframe(int32 animIndex, FbxNode* node, FbxCluster* cluster, const FbxAMatrix& matNodeTransform, int32 boneIdx, FbxMeshInfo* container);

	int32 FindBoneIndex(string name);
	FbxAMatrix GetTransform(FbxNode* node);

	void FillBoneWeight(FbxMesh* mesh, FbxMeshInfo* meshInfo);

private:
	FbxManager*		_manager = nullptr;
	FbxScene*		_scene = nullptr;
	FbxImporter*	_importer = nullptr;
	wstring			_resourceDirectory;

	vector<FbxMeshInfo>					_meshes;
	vector<FbxBoneInfo*>				_bones;
	vector<FbxAnimClipInfo*>			_animClips;
	FbxArray<FbxString*>				_animNames;
	map<int, vector<Vertex>>			_originVertexIndex;

private:
	// 참조용 변수들 
	wstring m_shaderPath;
	Device* m_device;
	vector<ConstantBuffer*>* m_constantBuffers;
	GraphicsDescriptorHeap* m_graphicsDescriptorHeap;
	ComputeDescriptorHeap* m_computeDescriptorHeap;
	ComputeCommandQueue* m_computeCommandQueue;
};

