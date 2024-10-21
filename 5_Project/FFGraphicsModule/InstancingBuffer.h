#pragma once

class Device;

class InstancingBuffer
{
public:
	InstancingBuffer(Device* p_device);
	~InstancingBuffer();

	void Init(uint8 p_maxCount = 10);

	void Clear();
	void AddParams(InstancingParams& p_params);
	void PushData();

public:
	uint32 GetCount() { return static_cast<uint32>(m_data.size()); }
	ComPtr<ID3D12Resource> GetBuffer() { return m_buffer; }
	D3D12_VERTEX_BUFFER_VIEW GetBufferView() { return m_bufferView; }

	void SetID(uint64 p_instanceID) { m_instanceID = p_instanceID; }
	uint64 GetID() { return m_instanceID; }

private:
	uint64 m_instanceID = 0;
	ComPtr<ID3D12Resource> m_buffer;
	D3D12_VERTEX_BUFFER_VIEW m_bufferView;

	uint32 m_maxCount = 0;
	vector<InstancingParams> m_data;

private:
	// 참조용 변수
	Device* m_device;
};

