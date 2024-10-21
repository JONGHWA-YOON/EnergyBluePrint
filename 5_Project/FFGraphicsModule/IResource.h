#pragma once

enum class RESOURCE_TYPE : uint8
{
	MESH,
	SHADER,
	TEXTURE,
	MATERIAL,
	MESH_DATA,
	END,
};

enum
{
	RESOURCE_TYPE_COUNT = static_cast<uint8>(RESOURCE_TYPE::END),
};

class IResource
{
public:
	IResource(RESOURCE_TYPE p_type);
	virtual ~IResource();

	RESOURCE_TYPE GetType() { return m_type; }
	uint32 GetID() { return m_id; }

protected:
	virtual void Load(const wstring& p_path) {}
	virtual void Save(const wstring& p_path) {}
	

protected:
	RESOURCE_TYPE m_type = {};

protected:
	uint32 m_id;
};

