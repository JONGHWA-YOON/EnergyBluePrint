// pch.h: 미리 컴파일된 헤더 파일입니다.
// 아래 나열된 파일은 한 번만 컴파일되었으며, 향후 빌드에 대한 빌드 성능을 향상합니다.
// 코드 컴파일 및 여러 코드 검색 기능을 포함하여 IntelliSense 성능에도 영향을 미칩니다.
// 그러나 여기에 나열된 파일은 빌드 간 업데이트되는 경우 모두 다시 컴파일됩니다.
// 여기에 자주 업데이트할 파일을 추가하지 마세요. 그러면 성능이 저하됩니다.

#ifndef PCH_H
#define PCH_H

// std::byte 사용안함
#define _HAS_STD_BYTE 0

#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.



// include들
#include <windows.h>
#include <tchar.h>
#include <memory>
#include <string>
#include <vector>
#include <array>
#include <list>
#include <map>
#include <unordered_set>
#include <tuple>
#include <algorithm>
#include <type_traits>
using namespace std;

#include <filesystem>
namespace fs = std::filesystem;

#include "d3dx12.h"

#include <wrl.h>

#include <d3d12.h>
#include <d3dcompiler.h>
#include <dxgi.h>
#include "SimpleMath.h"
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>

using namespace DirectX;
using namespace DirectX::PackedVector;


// lib들
#pragma comment(lib,"d3d12")
#pragma comment(lib,"dxgi")
#pragma comment(lib,"dxguid")
#pragma comment(lib,"d3dcompiler")

using namespace Microsoft::WRL;

#include "DirecX12AndTypedef.h"
using namespace FF;

#include <DirectXTex/DirectXTex.h>
#include <DirectXTex/DirectXTex.inl>

#ifdef _DEBUG
#pragma comment(lib,"DirectXTex\\DirectXTex_debug.lib")
#else
#pragma comment(lib,"DirectXTex\\DirectXTex_release")
#endif

#include "fbxsdk.h"

#ifdef _DEBUG
#pragma comment(lib,"FBX\\debug\\libfbxsdk-md.lib")
#pragma comment(lib,"FBX\\debug\\libxml2-md.lib")
#pragma comment(lib,"FBX\\debug\\zlib-md.lib")
#else
#pragma comment(lib,"FBX\\release\\libfbxsdk-md.lib")
#pragma comment(lib,"FBX\\release\\libxml2-md.lib")
#pragma comment(lib,"FBX\\release\\zlib-md.lib")
#endif

class Texture;
struct RenderTarget
{
	Texture* texture;
	float clearColor[4]{0.f,0.f,0.f,0.f};
};

using Vec2 = DirectX::SimpleMath::Vector2;
using Vec3 = DirectX::SimpleMath::Vector3;
using Vec4 = DirectX::SimpleMath::Vector4;
using Matrix = DirectX::SimpleMath::Matrix;



struct WindowInfo
{
	HWND	hwnd;		// 출력 윈도우
	FF::int32	width;	// 너비
	FF::int32	height;	// 높이
	bool	windowed;	// 창모드 or 전체화면
};

struct Vertex
{
	Vertex() {}

	Vertex(Vec3 p, Vec2 u, Vec3 n, Vec3 t)
		:pos(p), uv(u), normal(n), tangent(t)
	{

	}

	Vec3 pos;
	Vec2 uv;
	Vec3 normal;
	Vec3 tangent;
	Vec4 weights;
	Vec4 indices;

	bool operator ==(const Vertex& rhs)
	{
		if ((this->pos == rhs.pos)&&
			(this->uv == rhs.uv)&&
			(this->normal == rhs.normal)&&
			(this->tangent == rhs.tangent)&&
			(this->weights == rhs.weights)&&
			(this->indices == rhs.indices))
		{
			return true;
		}
		return false;
	}

	bool operator !=(const Vertex& rhs)
	{
		if ((this->pos == rhs.pos) &&
			(this->uv == rhs.uv) &&
			(this->normal == rhs.normal) &&
			(this->tangent == rhs.tangent) &&
			(this->weights == rhs.weights) &&
			(this->indices == rhs.indices))
		{
			return false;
		}
		return true;
	}
};


struct TransformParams
{
	Matrix matWorld;
	Matrix matView;
	Matrix matProjection;
	Matrix matWV;
	Matrix matWVP;
	Matrix matViewInv;
	Matrix matLightVP;
};

struct MATERIAL_DESC
{
	wstring materialName;
	wstring shader;
	array<wstring, 4> texture;
	array<int, 4> intParams;
	array<float, 4> floatParams;
	array<Vec2, 4> vec2Params;
	array<Vec4, 4> vec4Params;
	Vec4 color = {1.f,1.f,1.f,1.f};

	bool operator==(MATERIAL_DESC& rhs)
	{
		if ((this->materialName == rhs.materialName)
			&& (this->shader == rhs.shader)
			&& (equal(begin(this->texture), end(this->texture), begin(rhs.texture), end(rhs.texture)))
			&& (equal(begin(this->intParams), end(this->intParams), begin(rhs.intParams), end(rhs.intParams)))
			&& (equal(begin(this->floatParams), end(this->floatParams), begin(rhs.floatParams), end(rhs.floatParams)))
			&& (equal(begin(this->vec2Params), end(this->vec2Params), begin(rhs.vec2Params), end(rhs.vec2Params)))
			&& (equal(begin(this->vec4Params), end(this->vec4Params), begin(rhs.vec4Params), end(rhs.vec4Params)))
			&& (this->color == rhs.color))
		{
			return true;
		}

		return false;
	}

	bool operator==(FFMATERIAL_DESC& rhs)
	{
		if ((this->materialName == rhs.materialName)
			&& (this->shader == rhs.shader)
			&& (equal(begin(this->texture), end(this->texture), begin(rhs.texture), end(rhs.texture)))
			&& (equal(begin(this->intParams), end(this->intParams), begin(rhs.intParams), end(rhs.intParams)))
			&& (equal(begin(this->floatParams), end(this->floatParams), begin(rhs.floatParams), end(rhs.floatParams))))
		{
			for (size_t i = 0; i < this->vec2Params.size(); ++i)
			{
				Vec2 temp = { rhs.vec2Params[i].x, rhs.vec2Params[i].y };
				if (this->vec2Params[i] != temp)
				{
					return false;
				}
			}
			for (size_t i = 0; i < this->vec4Params.size(); ++i)
			{
				Vec4 temp = { rhs.vec4Params[i].x, rhs.vec4Params[i].y, rhs.vec4Params[i].z, rhs.vec4Params[i].w };
				if (this->vec4Params[i] != temp)
				{
					return false;
				}
			}
			return true;
		}

		return false;
	}

	void ChangeFromFFMATERIAL_DESC(const FF::FFMATERIAL_DESC& p_desc)
	{
		this->materialName = p_desc.materialName;
		this->shader = p_desc.shader;
		this->texture = p_desc.texture;
		this->intParams = p_desc.intParams;
		this->floatParams = p_desc.floatParams;
		Vec4 _color = { p_desc.color.x, p_desc.color.y, p_desc.color.z, p_desc.color.w };
		this->color = _color;
		for (size_t i = 0; i < p_desc.vec2Params.size(); ++i)
		{
			Vec2 temp = { p_desc.vec2Params[i].x, p_desc.vec2Params[i].y };
			this->vec2Params[i] = temp;
		}
		for (size_t i = 0; i < p_desc.vec4Params.size(); ++i)
		{
			Vec4 temp = { p_desc.vec4Params[i].x, p_desc.vec4Params[i].y, 
			p_desc.vec4Params[i].z, p_desc.vec4Params[i].w };
			this->vec4Params[i] = temp;
		}
	}

	FF::FFMATERIAL_DESC ChangeToFFMATERIAL_DESC()
	{
		FFMATERIAL_DESC ffMaterialDesc = {};
		ffMaterialDesc.materialName = this->materialName;
		ffMaterialDesc.shader = this->shader;
		ffMaterialDesc.texture = this->texture;
		ffMaterialDesc.intParams = this->intParams;
		ffMaterialDesc.floatParams = this->floatParams;
		FFVec4 _color = { this->color.x,this->color.y, this->color.z, this->color.w };
		ffMaterialDesc.color = _color;
		for (size_t i = 0; i < this->vec2Params.size(); ++i)
		{
			Vec2 temp = { this->vec2Params[i].x, this->vec2Params[i].y };
			this->vec2Params[i] = temp;
		}
		for (size_t i = 0; i < this->vec4Params.size(); ++i)
		{
			Vec4 temp = { this->vec4Params[i].x, this->vec4Params[i].y,
			this->vec4Params[i].z, this->vec4Params[i].w };
			this->vec4Params[i] = temp;
		}

		return ffMaterialDesc;
	}
};

struct InstancingParams
{
	Matrix matWorld;
	Matrix matWV;
	Matrix matWVP;
	Matrix matProj;
};


class Mesh;
class Material;
struct MeshRenderInfo
{
	Mesh* mesh;
	bool hasAnimation;
	vector<Material*>	materials;
};

struct AnimFrameParams
{
	Vec4	scale;
	Vec4	rotation; // Quaternion
	Vec4	translation;
};

wstring s2ws(const string& s);
string ws2s(const wstring& s);

float CalBoundRadius(const Vec3& p_target);
#endif //PCH_H
