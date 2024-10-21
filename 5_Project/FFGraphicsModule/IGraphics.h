#pragma once

class IFFGraphics abstract
{
public:
	virtual void RenderBegin() abstract;
	virtual void RenderEnd() abstract;


	virtual void DrawMesh		(const FFMatrix& p_WTM, const wstring& p_mesh, const MATERIAL_DESC& p_material) abstract;
	virtual void DrawParticle	(const FFMatrix& p_WTM, const wstring& p_mesh, const MATERIAL_DESC& p_material) abstract;
	virtual void DrawCube		(const FFMatrix& p_WTM, const MATERIAL_DESC& p_material) abstract;
	virtual void DrawSphere		(const FFMatrix& p_WTM, const MATERIAL_DESC& p_material) abstract;
	virtual void DrawCapsule	(const FFMatrix& p_WTM, const MATERIAL_DESC& p_material) abstract;
	virtual void DrawLine		(const FFMatrix& p_WTM, const MATERIAL_DESC& p_material) abstract;
	virtual void DrawInstanced	(const FFMatrix& p_WTM, const MATERIAL_DESC& p_material, unsigned __int64 p_instanceID) abstract;


	virtual void DrawDeferredDebug() abstract;

	virtual void SetCameraInfo(const FFMatrix& p_VTM, const FFMatrix& p_PTM) abstract;
	virtual void SetLightData(void* p_buffer, unsigned __int32 p_size) abstract;

	virtual void ResizeWindow(__int32 p_width, __int32 p_height) abstract;

	//virtual void OMSetRenderTargets(const RENDER_TARGET_TYPE& p_type) abstract;
	//virtual void ClearRenderTargetViews(const RENDER_TARGET_TYPE& p_type) abstract;
	//virtual void WaitTargetToResource(const RENDER_TARGET_TYPE& p_type) abstract;

};

