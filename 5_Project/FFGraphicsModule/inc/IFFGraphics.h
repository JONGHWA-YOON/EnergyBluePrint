#pragma once

#include "DirecX12AndTypedef.h"

class IFFGraphics abstract
{
public:
	virtual void Init(long long p_hWnd, float p_width, float p_height, bool p_isWindowed) abstract;

	virtual void RenderBegin(float p_deltaTime) abstract;
	virtual void RenderEnd() abstract;

	virtual void DrawMesh		(const FF::FFMatrix& p_WTM, const std::wstring& p_mesh, const FF::FFMATERIAL_DESC& p_material, std::wstring& p_materialName, int p_materialIndex) abstract;
	//virtual void DrawSkinnedMesh(const unsigned __int64 p_id, const std::wstring& p_currentAnimationName ,const FF::FFMatrix& p_WTM, const std::wstring& p_mesh, const FF::FFMATERIAL_DESC& p_material, std::wstring& p_materialName) abstract;
	virtual void DrawParticle	(const FF::FFMatrix& p_WTM, const std::wstring& p_mesh, const FF::FFMATERIAL_DESC& p_material) abstract;

	virtual void DrawDeferredDebug() abstract;

	virtual void SetCameraInfo(unsigned __int32 p_cameraIndex ,float p_near, float p_far, float p_fov, float p_scale, float p_width, float p_height ,const FF::FFMatrix& p_WTM) abstract;
	virtual void SetLightData(void* p_buffer, unsigned __int32 p_size) abstract;

	virtual void ResizeWindow(__int32 p_width, __int32 p_height) abstract;

	virtual void CreateMesh(const std::wstring& p_path) abstract;
	virtual void CreateShader(const std::wstring& p_path, const FF::ShaderInfo& p_info) abstract;
	virtual void CreateTexture(const std::wstring& p_path)abstract;
	virtual void CreateMaterial(const std::wstring& p_path, const FF::FFMATERIAL_DESC& p_desc) abstract;
	virtual void LoadFBX(const std::wstring& p_path, const std::wstring& p_shaderPath) abstract;

	virtual std::vector<FF::FFMeshData>& GetMeshDataFromFBX(const std::wstring& p_path) abstract;

	virtual void PushAnimationData(unsigned __int64 p_id, __int32 p_frame, __int32 p_nextFrame, float p_frameRatio) abstract;

	virtual void DrawSkinnedMesh(unsigned __int64 p_id, __int32 p_frame, __int32 p_nextFrame, float p_frameRatio, const std::wstring& p_currentAnimationName, const FF::FFMatrix& p_WTM, const std::wstring& p_mesh, const FF::FFMATERIAL_DESC& p_material, std::wstring& p_materialName, int p_materialIndex) abstract;

	// 에디터에 제공할 GPU_DESCRIPTOR_HANDLE값
	virtual unsigned __int64 GetGPUHandle() abstract;
};

IFFGraphics* CreateFFRenderer();
void DeleteFFRenderer(IFFGraphics** p_renderer);