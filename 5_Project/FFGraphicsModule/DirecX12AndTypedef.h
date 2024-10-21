#pragma once

namespace FF
{
	// typedef들
	using int8 = __int8;
	using int16 = __int16;
	using int32 = __int32;
	using int64 = __int64;
	using uint8 = unsigned __int8;
	using uint16 = unsigned __int16;
	using uint32 = unsigned __int32;
	using uint64 = unsigned __int64;

	struct FFMatrix
	{
		union
		{
			struct
			{
				float _11, _12, _13, _14;
				float _21, _22, _23, _24;
				float _31, _32, _33, _34;
				float _41, _42, _43, _44;
			};
			float m[4][4];
		};
	};

	struct FFVec2
	{
		float x;
		float y;

		bool operator == (const FFVec2& p_rhs)
		{
			if ((this->x == p_rhs.x) && (this->y == p_rhs.y))
			{
				return true;
			}
			return false;
		}
	};

	struct FFVec3
	{
		float x;
		float y;
		float z;

		bool operator == (const FFVec3& p_rhs)
		{
			if ((this->x == p_rhs.x) && (this->y == p_rhs.y) && (this->z == p_rhs.z))
			{
				return true;
			}
			return false;
		}
	};

	struct FFVec4
	{
		float x;
		float y;
		float z;
		float w;

		bool operator == (const FFVec4& p_rhs)
		{
			if ((this->x == p_rhs.x) && (this->y == p_rhs.y) && (this->z == p_rhs.z) && (this->w == p_rhs.w))
			{
				return true;
			}
			return false;
		}
	};

	enum class CBV_REGISTER : uint8
	{
		b0,
		b1,
		b2,
		b3,
		b4,

		END
	};

	enum class SRV_REGISTER : uint8
	{
		t0 = static_cast<uint8>(CBV_REGISTER::END),
		t1,
		t2,
		t3,
		t4,
		t5,
		t6,
		t7,
		t8,
		t9,

		END
	};

	enum class UAV_REGISTER : uint8
	{
		u0 = static_cast<uint8>(SRV_REGISTER::END),
		u1,
		u2,
		u3,
		u4,

		END,
	};

	enum
	{
		SWAP_CHAIN_BUFFER_COUNT = 2,
		CBV_REGISTER_COUNT = CBV_REGISTER::END,
		SRV_REGISTER_COUNT = static_cast<uint8>(SRV_REGISTER::END) - static_cast<uint8>(CBV_REGISTER_COUNT),
		CBV_SRV_REGISTER_COUNT = static_cast<uint8>(CBV_REGISTER_COUNT) + static_cast<uint8>(SRV_REGISTER_COUNT),
		UAV_REGISTER_COUNT = static_cast<uint8>(UAV_REGISTER::END) - CBV_SRV_REGISTER_COUNT,
		TOTAL_REGISTER_COUNT = CBV_SRV_REGISTER_COUNT + UAV_REGISTER_COUNT,
	};


#pragma region 셰이더에서 사용할 enum

	enum class SHADER_TYPE :uint8
	{
		DEFERRED,
		DEFERRED_AND_SKIN,
		FORWARD,
		LIGHTING,
		PARTICLE,
		COMPUTE,
		SHADOW,
		UI,
		EFFECT_ANIM,
		EFFECT
	};

	enum class RASTERIZER_TYPE
	{
		CULL_NONE,
		CULL_FRONT,
		CULL_BACK,
		WIREFRAME,
	};

	enum class DEPTH_STENCIL_TYPE
	{
		LESS,
		LESS_EQUAL,
		GREATER,
		GREATER_EQUAL,
		NO_DEPTH_TEST, // 깊이 테스트(x) + 깊이 기록(o)
		NO_DEPTH_TEST_NO_WRITE, // 깊이 테스트(x) + 깊이 기록(x)
		LESS_NO_WRITE, //깊이 테스트(o) + 깊이 기록(x)
	};

	enum class BLEND_TYPE :uint8
	{
		DEFAULT,
		ALPHA_BLEND,
		ONE_TO_ONE_BLEND,
		END,
	};

	enum class FFTopology
	{
		D3D_PRIMITIVE_TOPOLOGY_POINTLIST = 1,
		D3D_PRIMITIVE_TOPOLOGY_LINELIST = 2,
		D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST = 4,
	};

	struct ShaderInfo
	{
		SHADER_TYPE shaderType = SHADER_TYPE::FORWARD;
		RASTERIZER_TYPE rasterizerType = RASTERIZER_TYPE::CULL_BACK;
		DEPTH_STENCIL_TYPE depthStencilType = DEPTH_STENCIL_TYPE::LESS;
		BLEND_TYPE blendType = BLEND_TYPE::DEFAULT;
		//D3D_PRIMITIVE_TOPOLOGY topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		FFTopology topology = FFTopology::D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		std::string vs = "VS_Main";
		std::string ps = "PS_Main";
		std::string gs = "";
		std::string cs = "";

		bool operator==(ShaderInfo& rhs)
		{
			if ((this->shaderType == rhs.shaderType)
				&& (this->rasterizerType == rhs.rasterizerType)
				&& (this->depthStencilType == rhs.depthStencilType)
				&& (this->blendType == rhs.blendType)
				&& (this->topology == rhs.topology)
				&& (this->vs == rhs.vs)
				&& (this->ps == rhs.ps))
			{
				return true;
			}

			return false;
		}
	};

	struct FFMATERIAL_DESC
	{
		std::wstring materialName;
		std::wstring shader;
		std::array<std::wstring, 4> texture;
		std::array<int, 4> intParams;
		std::array<float, 4> floatParams;
		std::array<FFVec2, 4> vec2Params;
		std::array<FFVec4, 4> vec4Params;
		FFVec4 color = { 1.f,1.f,1.f,1.f };

		bool operator==(FFMATERIAL_DESC& rhs)
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
	};
#pragma endregion


#pragma region 멀티 렌더 타겟용
	enum class RENDER_TARGET_TYPE : uint8
	{
		SWAP_CHAIN,
		G_BUFFER,
		LIGHTING,
		SHADOW,
		END,
	};

	enum
	{
		RENDER_TARGET_TYPE_COUNT = static_cast<uint8>(RENDER_TARGET_TYPE::END),
		SHADOW_MEMBER_COUNT = 1,
		G_BUFFER_MEMBER_COUNT = 4,
		LIGHTING_GROUP_MEMBER_COUNT = 2,
	};

	enum
	{
		POSITION,
		NORMAL,
		COLOR,
		DEPTH,
		DIFFUSELIGHT = 0,
		SPECULARLIGHT = 1,
		SHADOW = 0,
	};
#pragma endregion


	enum class LIGHT_TYPE : uint8
	{
		DIRECTIONAL_LIGHT,
		POINT_LIGHT,
		SPOT_LIGHT,
	};

	struct LightColor
	{
		FFVec4 diffuse;
		FFVec4 ambient;
		FFVec4 specular;
	};

	struct LightInfo
	{
		LightColor color;
		FFVec4 position;
		FFVec4 direction;
		int32 lightType;
		float range;
		float angle;
		int32 padding;
	};

	struct LightParams
	{
		uint32 lightCount;
		FFVec3 padding;
		LightInfo lights[50];
	};

#pragma region 파티클에서 쓸 정보
	struct ParticleInfo
	{
		FFVec3 worldPos;
		float curTime;
		FFVec3 worldDir;
		float lifeTime;
		int32 alive;
		int32 padding[3];
	};

	struct ComputeSharedInfo
	{
		int32 addCount;
		int32 padding[3];
	};
#pragma endregion

	enum class CONSTANT_BUFFER_TYPE : uint8
	{
		GLOBAL,
		TRANSFORM,
		MATERIAL,
		END
	};

	enum
	{
		CONSTANT_BUFFER_COUNT = static_cast<uint8>(CONSTANT_BUFFER_TYPE::END)
	};

	struct FFAnimationClip
	{
		std::wstring	animName;
		int32			frameCount;
		double			duration;
		bool			isLoop;
	};

	struct FFMeshData
	{
		std::wstring meshName;
		bool hasAnimation;
		std::vector<FFAnimationClip> vecAnimClip;
		std::vector<FFMATERIAL_DESC> vecMaterialDesc;
	};
}