// 이미터 파티클과 일반 파티클을 구분하기 위한 매크로
#define PT_EMITTER 0
#define PT_FLARE 1

// 매 프레임 갱신되는 상수 버퍼
cbuffer cbPerFrame
{
	float3 gEyePosW;
	
	float3 gEmitPosW;
	float3 gEmitDirW;

	float gGameTime;
	float gTimeStep;
	
	float4x4 gViewProj;
	
	float gParticleInterval;
	float gParticleSize;
	float gParticleLife;
	
	float gRandomVectorIntensity;
	float gRandomVectorRange;
	
	float gAccelIntensity;
};

// 고정 값을 가진 상수 버퍼
cbuffer cbFixed
{
	float3 gAccelW = { 0.0f, 7.8f, 0.0f };
	
	float2 gQuadTexC[4] =
	{
		float2(1.0f, 1.0f),
		float2(1.0f, 0.0f),
		float2(0.0f, 1.0f),
		float2(0.0f, 0.0f)
	};
};

// 파티클 텍스쳐
Texture2D gTexArray;

// 샘플러 스테이트
SamplerState samLinear
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = WRAP;
	AddressV = WRAP;
};

// SO 전용 테크닉에서 뎁스 버퍼를 비활성화하기 위한 DSS
DepthStencilState DisableDepth
{
	DepthEnable = FALSE;
	DepthWriteMask = ZERO;
};

// 그리기 테크닉에서 뎁스 버퍼를 활성화하기 위한 DSS
DepthStencilState EnableDepth
{
	DepthEnable = TRUE;
	DepthWriteMask = ZERO;
	DepthFunc = LESS;
};

// 알파 블렌딩을 위한 블렌드 스테이트
BlendState AdditiveBlending
{
	AlphaToCoverageEnable = FALSE;
	BlendEnable[0] = TRUE;
	SrcBlend = SRC_ALPHA;
	DestBlend = ONE;
	BlendOp = ADD;
	SrcBlendAlpha = ZERO;
	BlendOpAlpha = SUBTRACT;
	RenderTargetWriteMask[0] = 0x0F;
};

// 난수 생성 함수
float random(float2 p)
{
	float2 K1 = float2(23.14069263277926, 2.665144142690225);
	return frac(cos(dot(p, K1)) * 12345.6789);
}


//////////////////////////////////////////////////////////////////////
/// 스트림 출력 전용 테크닉
//////////////////////////////////////////////////////////////////////

struct Particle
{
	float3 InitialPosW : POSITION;
	float3 InitialVelW : VELOCITY;
	float2 SizeW : SIZE;
	float Age : AGE;
	uint Type : TYPE;
};

Particle StreamOutVS(Particle vin)
{
	return vin;
}

[maxvertexcount(2)]
void StreamOutGS(point Particle gin[1], inout PointStream<Particle> ptStream)
{
	gin[0].Age += gTimeStep; // TimeStep?
	
	// 이미터 파티클일 경우
	if(gin[0].Type == PT_EMITTER)
	{
		if(gin[0].Age > gParticleInterval) // 파티클이 생성되는 주기
		{
			// 랜덤 벨로시티를 주기 위해 랜덤한 3차원 벡터를 만들어냄
			float2 s1 = float2(gGameTime + 0.1f, gGameTime - 0.1f);
			float2 s2 = float2(gGameTime - 0.1f, gGameTime + 0.1f);
			float2 s3 = float2(gGameTime + 0.5f, gGameTime + 0.5f);
			
			float3 vRandom = float3(random(s1) * 2 - 1, random(s2), random(s3));
			vRandom.x *= gRandomVectorRange;
			vRandom.z *= gRandomVectorRange;

			// 파티클을 만들어 스트림 출력 단계로 넘긴다
			Particle p;
			p.InitialPosW = gEmitPosW.xyz;
			p.InitialVelW = gRandomVectorIntensity * vRandom + gEmitDirW; // 파티클이 퍼져나가는 방향
			p.SizeW = float2(gParticleSize, gParticleSize); // 파티클 크기
			p.Age = 0.0f;
			p.Type = PT_FLARE;
			
			// 스트림 출력 단계를 통과한 파티클들은 SO용 VB로 들어간다
			ptStream.Append(p);
			
			// 파티클을 방출한 후 Emitter 파티클의 Age 초기화
			gin[0].Age = 0.0f;
		}
		ptStream.Append(gin[0]);
	}
	// 일반 파티클일 경우
	else
	{
		// 파티클이 살아있는 동안 출력한다 (SO를 통해 VB로 넘긴다)
		if(gin[0].Age <= gParticleLife) // 파티클 유지 시간
		{
			ptStream.Append(gin[0]);
		}
	}
}

// 스트림 출력으로 사용하기 위한 지오메트리 쉐이더를 만든다
GeometryShader gsStreamOut = ConstructGSWithSO(
	CompileShader(gs_5_0, StreamOutGS()),
	"POSITION.xyz; VELOCITY.xyz; SIZE.xy; AGE.x; TYPE.x"
);

// 스트림 출력 테크닉
technique11 StreamOutTech
{
	pass P0
	{
		// 버텍스 쉐이더와 지오메트리 쉐이더를 설정한다
		SetVertexShader(CompileShader(vs_5_0, StreamOutVS()));
		SetGeometryShader(gsStreamOut);
		
		// 스트림 출력을 위해 픽셀 쉐이더와 깊이 버퍼를 비활성화
		SetPixelShader(NULL);
		SetDepthStencilState(DisableDepth, 0);
	}
}

//////////////////////////////////////////////////////////////////////
/// 그리기 테크닉
//////////////////////////////////////////////////////////////////////

struct VertexOut
{
	float3 PosW : POSITION;
	float2 SizeW : SIZE;
	float4 Color : COLOR;
	uint Type : TYPE;
};

VertexOut DrawVS(Particle vin)
{
	VertexOut vout;
	
	// 파티클이 생성된 후 지난 시간 (Age)
	float t = vin.Age;
	
	// 파티클의 초기 위치, 이동 속도, 방향
	vout.PosW = gAccelIntensity * t * t * gAccelW + t * vin.InitialVelW + vin.InitialPosW; 
	
	// 파티클은 시간에 따라 투명해진다
	float opacity = 1.0f - (smoothstep(0.0f, 1.0f, t * 0.4f)) * 1.0f; // 투명도
	vout.Color = float4(1.0f, 1.0f, 1.0f, opacity);
	
	vout.SizeW = vin.SizeW;
	vout.Type = vin.Type;
	
	return vout;
}

struct GeoOut
{
	float4 posH : SV_Position;
	float4 Color : COLOR;
	float2 Tex : TEXCOORD;
};

[maxvertexcount(4)]
void DrawGS(point VertexOut gin[1], inout TriangleStream<GeoOut> triStream)
{
	if(gin[0].Type != PT_EMITTER)
	{
		// 카메라를 향하는 빌보드를 만들기 위한 월드 행렬
		float3 look = normalize(gEyePosW.xyz - gin[0].PosW); // 파티클의 좌표에서 카메라로 향하는 벡터
		float3 right = normalize(cross(float3(0, 1, 0), look)); // local up과 look의 외적
		float3 up = cross(look, right); // world up
		
		float halfWidth = 0.5f * gin[0].SizeW.x;
		float halfHeight = 0.5f * gin[0].SizeW.y;
		
		// 사각형을 구성하는 버텍스 계산
		float4 v[4];
		v[0] = float4(gin[0].PosW + halfWidth * right - halfHeight * up, 1.0f);
		v[1] = float4(gin[0].PosW + halfWidth * right + halfHeight * up, 1.0f);
		v[2] = float4(gin[0].PosW - halfWidth * right - halfHeight * up, 1.0f);
		v[3] = float4(gin[0].PosW - halfWidth * right + halfHeight * up, 1.0f);

		// point에서 quad로 확장
		GeoOut gout;
		[unroll]
		for(int i = 0; i < 4; ++i)
		{
			gout.posH = mul(v[i], gViewProj);
			gout.Tex = gQuadTexC[i];
			gout.Color = gin[0].Color;
			triStream.Append(gout);
		}

	}
}

float4 DrawPS(GeoOut pin) : SV_Target
{
	// 만들어진 quad에 텍스쳐를 입혀 출력한다
	return gTexArray.Sample(samLinear, pin.Tex) * pin.Color * 0.5f;
	//return pin.Color * 0.5f * float4(1.0f, 1.0f, 1.0f, 0.015f);
	//return float4(1.0f, 0.2f, 0.0f, 0.5f);

}
technique11 DrawTech
{
	pass P0
	{
		// 쉐이더 설정
		SetVertexShader(CompileShader(vs_5_0, DrawVS()));
		SetGeometryShader(CompileShader(gs_5_0, DrawGS()));
		SetPixelShader(CompileShader(ps_5_0, DrawPS()));
		
		// 알파 블렌딩을 위해 BlendState 설정
		SetBlendState(AdditiveBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
		
		// DSS 설정
		SetDepthStencilState(EnableDepth, 0);
	}
}