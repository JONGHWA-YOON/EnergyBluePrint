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
	float3 gAccelW = { 0.0f, -9.8f, 0.0f };
	
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

[maxvertexcount(100)]
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
			
			float3 vRandom = float3(random(s2) * 2.0f - 1.0f, 0.0f, random(s3) * 2.0f - 1.0f);

			
			// 파티클을 만들어 스트림 출력 단계로 넘긴다
			Particle p;
			p.InitialPosW = gEyePosW + vRandom * 30.0f + float3(0.0f, 50.0f, 0.0f);
			p.InitialVelW = 0.1f * float3(random(s2) * 2.0f - 1.0f, -1.0f, random(s2) * 2.0f - 1.0f); // 파티클이 퍼져나가는 방향
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
	float2 s1 = float2(gGameTime + 0.1f, gGameTime - 0.1f);
	float2 s2 = float2(gGameTime - 0.1f, gGameTime + 0.1f);
	float2 s3 = float2(gGameTime + 0.5f, gGameTime + 0.5f);

	VertexOut vout;
	
	// 파티클이 생성된 후 지난 시간 (Age)
	float t = vin.Age;
	
	// 파티클의 초기 위치, 이동 속도, 방향
	vout.PosW = 50.0f * (t * t * gAccelW + t * vin.InitialVelW) + vin.InitialPosW;
	
	// 파티클은 시간에 따라 투명해진다
	float opacity = 0.7f - (smoothstep(0.0f, 1.0f, t * 0.8f)) * 1.0f; // 투명도
	vout.Color = float4(1.0f, 1.0f, 1.0f, opacity);
	
	vout.SizeW = vin.SizeW;
	vout.Type = vin.Type;

	return vout;
}

struct GeoOut
{
	float4 posH : SV_Position;
	float4 Color : COLOR;
};

[maxvertexcount(2)]
void DrawGS(point VertexOut gin[1], inout LineStream<GeoOut> lineStream)
{
	if(gin[0].Type != PT_EMITTER)
	{
		// 사각형을 구성하는 버텍스 계산
		float3 v[2];
		v[0] = gin[0].PosW;
		v[1] = gin[0].PosW + gAccelW * 1.0f;

		// point에서 quad로 확장
		GeoOut gout;
		
		gout.posH = mul(float4(v[0], 1.0f), gViewProj);
		gout.Color = gin[0].Color * float4(1.0f, 1.0f, 1.0f, 0.0f);
		lineStream.Append(gout);
		
		gout.posH = mul(float4(v[1], 1.0f), gViewProj);
		gout.Color = gin[0].Color * float4(1.0f, 1.0f, 1.0f, 0.8f);
		lineStream.Append(gout);

	}
}

float4 DrawPS(GeoOut pin) : SV_Target
{
	// 만들어진 quad에 텍스쳐를 입혀 출력한다
	return pin.Color;
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