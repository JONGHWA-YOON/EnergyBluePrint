#pragma once

class Frustum;

class FFCamera
{
public:
	FFCamera();
	~FFCamera();

public:
	Matrix matView = {};
	Matrix matPerspectiveProjection = {};
	Matrix matOrthographicProjection = {};

private:
	float m_near = 1.f;
	float m_far = 1000.f;
	float m_fov = XM_PI / 4.f;
	float m_scale = 1.f;

	float m_width;
	float m_height;

	Frustum* m_frustum;

public:
	void SetCameraInfo(float p_near, float p_far, float p_fov, float p_scale);
	void SetResolution(float p_width, float p_height);
	void CalculateMatrix(const Matrix& p_WTM);

	Frustum* GetFrustum() { return m_frustum; }
};

