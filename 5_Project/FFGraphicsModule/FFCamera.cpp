#include "GraphicsPch.h"
#include "FFCamera.h"

#include "Frustum.h"

FFCamera::FFCamera()
{
	m_frustum = new Frustum;
}

FFCamera::~FFCamera()
{
	delete m_frustum;
}

void FFCamera::SetCameraInfo(float p_near, float p_far, float p_fov, float p_scale)
{
	m_near = p_near;
	m_far = p_far;
	m_fov = p_fov;
	m_scale = p_scale;
}

void FFCamera::SetResolution(float p_width, float p_height)
{
	m_width = p_width;
	m_height = p_height;
}

void FFCamera::CalculateMatrix(const Matrix& p_WTM)
{
	matView = p_WTM.Invert();
	matPerspectiveProjection = ::XMMatrixPerspectiveFovLH(m_fov, m_width / m_height, m_near, m_far);
	matOrthographicProjection = ::XMMatrixOrthographicLH(m_width * m_scale, m_height * m_scale, m_near, m_far);
}
