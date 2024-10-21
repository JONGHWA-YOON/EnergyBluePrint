#include "GraphicsPch.h"
#include "Frustum.h"

#include "FFCamera.h"
#include "CameraManager.h"

void Frustum::UpdateFrustum()
{
	// 일단은 카메라를 하나만 사용하기로 해서 임시로 이렇게 행렬을 받아온다.
	Matrix matViewInv = CameraManager::GetInstance().GetCamera(0)->matView.Invert();
	Matrix matProjInv = CameraManager::GetInstance().GetCamera(0)->matPerspectiveProjection.Invert();
	Matrix matInv = matProjInv * matViewInv;

	vector<Vec3> worldPos =
	{
		::XMVector3TransformCoord(Vec3(-1.f,1.f,0.f), matInv),
		::XMVector3TransformCoord(Vec3(1.f,1.f,0.f), matInv),
		::XMVector3TransformCoord(Vec3(1.f,-1.f,0.f), matInv),
		::XMVector3TransformCoord(Vec3(-1.f,-1.f,0.f), matInv),

		::XMVector3TransformCoord(Vec3(-1.f,1.f,1.f), matInv),
		::XMVector3TransformCoord(Vec3(1.f,1.f,1.f), matInv),
		::XMVector3TransformCoord(Vec3(1.f,-1.f,1.f), matInv),
		::XMVector3TransformCoord(Vec3(-1.f,-1.f,1.f), matInv),
	};

	m_planes[PLANE_FRONT] = ::XMPlaneFromPoints(worldPos[0], worldPos[1], worldPos[2]); // CW
	m_planes[PLANE_BACK] = ::XMPlaneFromPoints(worldPos[4], worldPos[7], worldPos[5]); // CCW
	m_planes[PLANE_UP] = ::XMPlaneFromPoints(worldPos[4], worldPos[5], worldPos[1]); // CW
	m_planes[PLANE_DOWN] = ::XMPlaneFromPoints(worldPos[7], worldPos[3], worldPos[6]); // CCW
	m_planes[PLANE_LEFT] = ::XMPlaneFromPoints(worldPos[4], worldPos[0], worldPos[7]); // CW
	m_planes[PLANE_RIGHT] = ::XMPlaneFromPoints(worldPos[5], worldPos[6], worldPos[1]); // CCW
}

bool Frustum::ContainSphere(Matrix& p_WTM, float p_radius)
{
	Vec3 translation;
	DirectX::SimpleMath::Quaternion rotation;
	Vec3 scale;

	p_WTM.Decompose(scale , rotation, translation);
	
	float scales[3] = { scale.x, scale.y, scale.z };

	float maxScale = 0.f;

	for (int i = 0; i < 3; ++i)
	{
		maxScale = max(maxScale, scales[i]);
	}

	p_radius *= maxScale;

	for (const Vec4& plane : m_planes)
	{
		// n = (a,b,c)
		Vec3 normal = Vec3(plane.x, plane.y, plane.z);

		// ax + by + cz + d > radius
		if (normal.Dot(translation) + plane.w > p_radius)
		{
			return false;
		}
	}

	return true;
}
