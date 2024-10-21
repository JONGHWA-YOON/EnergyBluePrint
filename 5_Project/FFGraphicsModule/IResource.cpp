#include "GraphicsPch.h"
#include "IResource.h"

IResource::IResource(RESOURCE_TYPE p_type)
	: m_type(p_type)
{
	static uint32 id = 1;
	m_id = id;
	id++;
}

IResource::~IResource()
{

}
