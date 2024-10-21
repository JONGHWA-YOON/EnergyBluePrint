#pragma once

union InstanceID
{
	struct  
	{
		unsigned __int32 meshID;
		unsigned __int32 materialID;
	};
	unsigned __int64 id;
};

struct SkinnedMeshData
{
	unsigned __int64 animatorID;
	__int32 curFrame;
	__int32 nextFrame;
	float ratio;
	wstring curAnimationName;
	Matrix WTM;
	wstring meshName;
	MATERIAL_DESC materialDesc;
	wstring& materialName;
	int materialIndex;

	SkinnedMeshData(
		unsigned __int64 p_animatorID,
		__int32 p_curFrame,
		__int32 p_nextFrame,
		float p_ratio,
		wstring p_curAnimationName,
		Matrix p_WTM,
		wstring p_meshName,
		MATERIAL_DESC p_desc,
		wstring& p_materialName,
		int p_materialIndex
	) 
		: animatorID(p_animatorID),
		curFrame(p_curFrame),
		nextFrame(p_nextFrame),
		ratio(p_ratio),
		curAnimationName(p_curAnimationName),
		WTM(p_WTM),
		meshName(p_meshName),
		materialName(p_materialName),
		materialIndex(p_materialIndex)
	{
		materialDesc = p_desc;
	}
};

class SwapChain;
class RenderTargetGroup;
class ConstantBuffer;
class InstancingBuffer;
class Device;

class RenderSystem
{
private:
	RenderSystem() {};
	~RenderSystem();

public:
	static RenderSystem* GetInstance()
	{
		static RenderSystem inst;
		return &inst;
	}

public:
	void Init(Device* p_device,SwapChain* p_swapChain, array<RenderTargetGroup*, RENDER_TARGET_TYPE_COUNT>* p_renderTargets,
		vector<ConstantBuffer*>* p_constantBuffers);

	void PushData(const Matrix& p_WTM, const wstring& p_mesh, const MATERIAL_DESC& p_materialDesc, wstring& p_materialName, int p_materialIndex);

	void Render();

	void ClearInstanceBuffer();
	void ClearRenderObjects();

	void SetLightParams(LightParams* p_params) { m_lightParams = p_params; }

	void DrawSkinnedMesh(unsigned __int64 p_id, __int32 p_frame, __int32 p_nextFrame, float p_frameRatio,const std::wstring& p_currentAnimationName, const Matrix& p_WTM, const std::wstring& p_mesh, const MATERIAL_DESC& p_material, std::wstring& p_materialName, int p_materialIndex);

	vector<tuple<Matrix, wstring, MATERIAL_DESC, wstring&,int>>& GetRenderObjects() { return m_renderObjects; }

private:
	void ClearRTV();
	void SortObject();
	void RenderShadow();
	void Render_Deferred();
	void Render_SkinnedMesh();
	void Render_Light();
	void Render_Final();
	void Render_Forward();
	void Render_UI();

	void Render_Object(vector<tuple<Matrix, wstring, MATERIAL_DESC, wstring&, int>>& p_renderObjects);

	void Render_Effect();

	void AddInstanceParams(uint64 p_instanceID, InstancingParams& p_params);

private:
	vector<tuple<Matrix, wstring, MATERIAL_DESC, wstring&, int>> m_renderObjects;
	vector<tuple<Matrix, wstring, MATERIAL_DESC, wstring&, int>> m_deferred;
	vector<tuple<Matrix, wstring, MATERIAL_DESC, wstring&, int>> m_forward;
	vector<tuple<Matrix, wstring, MATERIAL_DESC, wstring&, int>> m_lights;
	vector<tuple<Matrix, wstring, MATERIAL_DESC, wstring&, int>> m_particles;
	vector<tuple<Matrix, wstring, MATERIAL_DESC, wstring&, int>> m_ui;


	//vector<tuple<Matrix, wstring, MATERIAL_DESC, wstring&>> m_skinnedMesh;
	//vector<tuple<unsigned __int64, __int32 , __int32 , float ,const std::wstring& , Matrix , std::wstring, MATERIAL_DESC, std::wstring&>> m_skinnedMeshTest;
	vector<SkinnedMeshData> m_skinnedMeshTest;
	vector<SkinnedMeshData> m_textureAnim;
	vector<SkinnedMeshData> m_effect;

	map<uint64, InstancingBuffer*> m_buffers;

private:
	// 참조용 변수들
	Device* m_device;
	SwapChain* m_swapChain;
	array<RenderTargetGroup*, RENDER_TARGET_TYPE_COUNT>* m_renderTargets;
	vector<ConstantBuffer*>* m_constantBuffers;

	LightParams* m_lightParams;
};

