#pragma once

#include "IResource.h"

class Device;
class GraphicsCommandQueue;
class ComputeCommandQueue;
class RootSignature;

/// <summary>
/// 셰이더파일 로드와 파이프라인 상태를 만드는 클래스
/// 
/// 2023.06.16 김성찬 게임인재원
/// </summary>
/// 
 
class Shader : public IResource
{
public:
	Shader();
	virtual ~Shader();

	// 주어진 경로로 VS, PS 생성 / 파이프라인 상태 생성
	void CreateGraphicsShader(Device* p_device, RootSignature* p_rootSignature, GraphicsCommandQueue* p_graphicsCommandQueue, ComputeCommandQueue* p_computeCommandQueue, const wstring& p_path, ShaderInfo p_info = ShaderInfo());
	void CreateComputeShader(Device* p_device, RootSignature* p_rootSignature, GraphicsCommandQueue* p_graphicsCommandQueue, ComputeCommandQueue* p_computeCommandQueue, const wstring& p_path, const string& p_name, const string& p_version);

	// 매 업데이트마다 파이프라인 상태 갱신
	void Update();

	void SetShaderInfo(ShaderInfo p_info) { m_shaderInfo = p_info; };

	ShaderInfo GetShaderInfo() { return m_shaderInfo; }

	static D3D12_PRIMITIVE_TOPOLOGY_TYPE GetTopology(D3D_PRIMITIVE_TOPOLOGY p_topology);

private:
	// 셰이더 생성
	void CreateShader(const wstring& p_path, const string& p_name, const string& p_version, ComPtr<ID3DBlob>& p_blob, D3D12_SHADER_BYTECODE& p_shaderByteCode);

	// 버텍스 셰이더 생성
	void CreateVertexShader(const wstring& p_path, const string& p_name, const string& p_version);

	// 픽셀 셰이더 생성
	void CreatePixelShader(const wstring& p_path, const string& p_name, const string& p_version);

	// 지오메트리 셰이더 생성
	void CreateGeometryShader(const wstring& p_path, const string& p_name, const string& p_version);

	void SetPipelineState();

private:
	ShaderInfo m_shaderInfo = ShaderInfo();
	ComPtr<ID3D12PipelineState>			m_pipelineState;
	ComPtr<ID3D12PipelineState>			m_shadowPipelineState;

	// Graphics Shader
	ComPtr<ID3DBlob>					m_vsBlob;
	ComPtr<ID3DBlob>					m_psBlob;
	ComPtr<ID3DBlob>					m_gsBlob;
	ComPtr<ID3DBlob>					m_errBlob;
	D3D12_GRAPHICS_PIPELINE_STATE_DESC  m_pipelineDesc = {};
	D3D12_GRAPHICS_PIPELINE_STATE_DESC  m_shadowPipelineDesc = {};

	// Compute Shader
	ComPtr<ID3DBlob> m_csBlob;
	D3D12_COMPUTE_PIPELINE_STATE_DESC m_computePipelineDesc = {};

private:
	// 참조용 변수들
	Device* m_device;
	GraphicsCommandQueue* m_graphicsCommandQueue;
	ComputeCommandQueue* m_computeCommandQueue;
	RootSignature* m_rootSignature;
};

