#include "GraphicsPch.h"
#include "CameraManager.h"

#include "FFCamera.h"

CameraManager::~CameraManager()
{
	for (auto& camera : m_cameras)
	{
		delete camera.second;
	}
}

FFCamera* CameraManager::GetCamera(FF::uint32 p_cameraIndex)
{
	auto iter = m_cameras.find(p_cameraIndex);
	if (iter != m_cameras.end())
	{
		return iter->second;
	}

	FFCamera* newCamera = new FFCamera();
	m_cameras.insert({ p_cameraIndex, newCamera });
	return newCamera;
}
