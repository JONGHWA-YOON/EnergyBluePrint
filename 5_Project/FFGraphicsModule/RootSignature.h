#pragma once

class Device;
class ComputeCommandQueue;

enum 
{
	DEFAULT,
	LINEAR,
	SHADOW,
	END
};

/// <summary>
/// DX12에 있는 루트 시그니처와 관련된 것들을 묶는 클래스
/// 
/// 샘플러에 대한것도 여기서 가지고 있어야 하는지는 고민중
/// 
/// 2023.06.16 김성찬 게임인재원
/// </summary>
class RootSignature
{
public:
	// 샘플러와 루트 시그니처를 만든다.
	void Init(Device* p_device, ComputeCommandQueue* p_computeCommandQueue);

#pragma region Getter
	ComPtr<ID3D12RootSignature> GetGraphicsRootSignature() { return m_graphicsRootSignature; }
	ComPtr<ID3D12RootSignature> GetComputeRootSignature() { return m_computeRootSignature; }
#pragma endregion

private:
	// 루트 시그니처를 만든다.
	void CreateGraphicsRootSignature();
	void CreateComputeRootSignature();


private:
	D3D12_STATIC_SAMPLER_DESC m_samlplerDesc;
	D3D12_STATIC_SAMPLER_DESC m_samlplerDescArr[END];
	ComPtr<ID3D12RootSignature> m_graphicsRootSignature;
	
	ComPtr<ID3D12RootSignature> m_computeRootSignature;

private:
	// 참조용 변수들
	Device* m_device;
	ComputeCommandQueue* m_computeCommandQueue;
};

