#pragma once

class Device;
class GraphicsCommandQueue;

/// <summary>
/// DX12에 있는 SwapChain과 관련된 것들을 묶는 클래스
/// 
/// 지금은 더블 버퍼링용 백 버퍼와 프론트 버퍼만 있음
/// 나중에 디퍼드할 땐 어떤 식으로 변할지 모름
/// 
/// 2023.06.16 김성찬 게임인재원
/// </summary>
class SwapChain
{
public:
	// 스왑체인과 렌더타겟뷰를 만든다.
	void Init(Device* p_device, GraphicsCommandQueue* p_graphicsCommandQueue,const WindowInfo& p_info);

	// 실제로 백버퍼를 프론트버퍼로 바꿔준다.
	void Present();

	// Present가 되면 백버퍼와 프론트버퍼가 바뀌기 때문에 Index도 바꿔준다.
	void SwapIndex();

#pragma region Getter
	ComPtr<IDXGISwapChain> GetSwapChain() { return m_swapChain; }

	uint32 GetBackBufferIndex() { return m_backBufferIndex; }
#pragma endregion

private:
	// 화면 정보를 받아 SwapChain을 만든다.
	void CreateSwapChain(const WindowInfo& p_info);

private:
	ComPtr<IDXGISwapChain> m_swapChain;

	uint32 m_backBufferIndex = 0;

private:
	Device* m_device;
	GraphicsCommandQueue* m_graphicsCommandQueue;
};

