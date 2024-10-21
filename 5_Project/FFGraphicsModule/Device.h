#pragma once

/// <summary>
/// DX12에 있는 Device와 관련된 것들을 묶는 클래스
/// 
/// 2023.06.16 김성찬 게임인재원
/// </summary>

class Device
{
public:
	void Init();

#pragma region Getter
	ComPtr<IDXGIFactory> GetDXGI() { return m_dxgi; }
	ComPtr<ID3D12Device> GetDevice() 
	{
		return m_device; 
	}
#pragma endregion

private:
	ComPtr<ID3D12Debug>		m_debugController;
	ComPtr<IDXGIFactory>	m_dxgi;		// 화면 관련 기능들
	ComPtr<ID3D12Device>	m_device;	// 각종 객체 생성
};

