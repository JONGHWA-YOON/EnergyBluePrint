#pragma once

class FFCamera;

class CameraManager
{
private:
	CameraManager() {};
	~CameraManager();

public:
	static CameraManager& GetInstance()
	{
		static CameraManager inst;
		return inst;
	}

public:
	FFCamera* GetCamera(FF::uint32 p_cameraIndex);

private:
	unordered_map<FF::uint32, FFCamera*> m_cameras;

};

