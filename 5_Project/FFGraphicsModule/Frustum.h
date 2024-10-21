#pragma once

enum PLANE_TYPE : uint8
{
	PLANE_FRONT,
	PLANE_BACK,
	PLANE_UP,
	PLANE_DOWN,
	PLANE_LEFT,
	PLANE_RIGHT,

	PLANE_END
};

class Frustum
{
public:
	// 여기서 카메라정보로 프러스텀을 갱신한다.
	void UpdateFrustum();
	bool ContainSphere(Matrix& p_WTM, float p_radius);
	
private:
	array<Vec4, PLANE_END> m_planes;

};

