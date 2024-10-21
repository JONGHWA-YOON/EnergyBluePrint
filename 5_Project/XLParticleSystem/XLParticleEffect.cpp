#pragma once

#include "XLParticleEffect.h"
#include "XLParticleSystem.h"

#include "DDSTextureLoader.h"

#include "d3dx11effect.h"

extern ID3D11Device* s_pDevice;
extern ID3D11DeviceContext* s_pDeviceContext;
extern UINT s_MaxParticles;

XLParticleEffect::XLParticleEffect()
	:m_pFX(),
	StreamOutTech(),
	DrawTech(),
	m_pDevice(), m_pDeviceContext(),
	mFirstRun(true),
	m_pTexSRV()
{

}

XLParticleEffect::~XLParticleEffect()
{

}

void XLParticleEffect::Initialize(ID3DX11Effect* _pFX, ID3D11ShaderResourceView* _pTexSRV)
{
	m_pFX = _pFX;
	StreamOutTech = m_pFX->GetTechniqueByName("StreamOutTech");
	DrawTech = m_pFX->GetTechniqueByName("DrawTech");

	StreamOutTech->GetDesc(&SOtechDesc);
	DrawTech->GetDesc(&DrawtechDesc);

	m_pTexSRV = _pTexSRV;
}

void XLParticleEffect::Initialize()
{

}

void XLParticleEffect::Start()
{

}

void XLParticleEffect::Update()
{
	SendValuesToFX();

}

void XLParticleEffect::SendValuesToFX()
{
	SetShaderVariable("gViewProj", m_VP);
	SetShaderVariable("gGameTime", m_GameTime);
	SetShaderVariable("gTimeStep", m_TimeStep);
	SetShaderVariable("gEyePosW", m_EyePosW);
	SetShaderVariable("gEmitPosW", m_EmitPosW);
	SetShaderVariable("gEmitDirW", m_EmitDirW);
	SetShaderVariable("gTexArray", m_pTexSRV);
	SetShaderVariable("gParticleInterval", m_ParticleInterval);
	SetShaderVariable("gParticleSize", m_ParticleSize);
	SetShaderVariable("gParticleLife", m_ParticleLife);
	SetShaderVariable("gRandomVectorIntensity", m_RandomVectorIntensity);
	SetShaderVariable("gRandomVectorRange", m_RandomVectorRange);
	SetShaderVariable("gAccelIntensity", m_AccelIntensity);
}

void XLParticleEffect::BuildVB()
{
	// Emitter Particle을 위한 InitVB 생성
	D3D11_BUFFER_DESC InitVBDesc;
	InitVBDesc.Usage = D3D11_USAGE_DEFAULT;
	InitVBDesc.ByteWidth = sizeof(Particle);
	InitVBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_STREAM_OUTPUT;	// Bind Flag 지정
	InitVBDesc.CPUAccessFlags = 0;
	InitVBDesc.MiscFlags = 0;

	Particle p{};

	D3D11_SUBRESOURCE_DATA InitVBData;
	InitVBData.pSysMem = &(p);
	InitVBData.SysMemPitch = 0;
	InitVBData.SysMemSlicePitch = 0;

	hr = s_pDevice->CreateBuffer(&InitVBDesc, 0, &mInitVB);

	// InitVB 사용 이후 핑퐁 방식으로 사용할 두 개의 VB 생성
	D3D11_BUFFER_DESC PingPoingVBDesc;
	PingPoingVBDesc.Usage = D3D11_USAGE_DEFAULT;
	PingPoingVBDesc.ByteWidth = sizeof(Particle) * XLParticleSystem::GetMaxParticles();
	PingPoingVBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_STREAM_OUTPUT;	// Bind Flag 지정
	PingPoingVBDesc.CPUAccessFlags = 0;
	PingPoingVBDesc.MiscFlags = 0;

	hr = s_pDevice->CreateBuffer(&PingPoingVBDesc, 0, &mStreamOutVB);
	hr = s_pDevice->CreateBuffer(&PingPoingVBDesc, 0, &mDrawVB);

	// InputLayout 생성
	// TODO: ieDesc를 ParticleEffect 클래스로 이동
	D3DX11_PASS_DESC passDesc;
	D3D11_INPUT_ELEMENT_DESC ieDesc[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"VELOCITY", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"SIZE", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"AGE", 0, DXGI_FORMAT_R32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TYPE", 0, DXGI_FORMAT_R32_UINT, 0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	StreamOutTech->GetPassByIndex(0)->GetDesc(&passDesc);
	hr = s_pDevice->CreateInputLayout(ieDesc, 5, passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &(ParticleInputLayout));

}

void XLParticleEffect::SetShaderVariable(LPCSTR variableName, const float& value)
{
	m_pFX->GetVariableByName(variableName)->AsScalar()->SetFloat(value);
}

void XLParticleEffect::SetShaderVariable(LPCSTR variableName, const XMMATRIX& value)
{
	m_pFX->GetVariableByName(variableName)->AsMatrix()->SetMatrix(reinterpret_cast<const float*>(&value));
}

void XLParticleEffect::SetShaderVariable(LPCSTR variableName, const XMFLOAT3& value)
{
	m_pFX->GetVariableByName(variableName)->AsVector()->SetFloatVector(reinterpret_cast<const float*>(&value));
}

void XLParticleEffect::SetShaderVariable(LPCSTR variableName, ID3D11ShaderResourceView* value)
{
	ID3DX11EffectShaderResourceVariable* Tex = m_pFX->GetVariableByName(variableName)->AsShaderResource();
	hr = Tex->SetResource(value);
}

void FireParticleEffect::Initialize()
{
	m_pFX = XLParticleSystem::CompileFX(L"../../../XLParticleSystem/FX/QuadParticleEffect.fx");
	m_pTexSRV = XLParticleSystem::LoadTexture(L"../../../XLParticleSystem/Textures/flare0.dds");

	XLParticleEffect::Initialize(m_pFX, m_pTexSRV);

	SetParticleInterval(0.0001f);
	SetEmitPosW(XMFLOAT3(0.0f, 0.0f, 0.0f));
	SetEmitDirW(XMFLOAT3(0.0f, 0.0f, 0.0f));
	SetParticleSize(4.0f);
	SetParticleLife(6.0f);
	SetRandomVectorIntensity(2.0f);
	SetRandomVectorRange(0.3f);
	SetAccelIntensity(0.2f);
}

void SmokeParticleEffect::Initialize()
{
	m_pFX = XLParticleSystem::CompileFX(L"../../../XLParticleSystem/FX/QuadParticleEffect.fx");
	m_pTexSRV = XLParticleSystem::LoadTexture(L"../../../XLParticleSystem/Textures/dust.dds");

	XLParticleEffect::Initialize(m_pFX, m_pTexSRV);

	SetParticleInterval(0.001f);
	SetEmitPosW(XMFLOAT3(0.0f, 0.7f, 0.0f));
	SetEmitDirW(XMFLOAT3(0.0f, 1.0f, 0.0f));
	SetParticleSize(2.0f);
	SetParticleLife(8.0f);
	SetRandomVectorIntensity(1.0f);
	SetRandomVectorRange(1.0f);
	SetAccelIntensity(0.2f);
}

void AshParticleEffect::Initialize()
{
	m_pFX = XLParticleSystem::CompileFX(L"../../../XLParticleSystem/FX/QuadParticleEffect.fx");
	m_pTexSRV = XLParticleSystem::LoadTexture(L"../../../XLParticleSystem/Textures/particle2.dds");

	XLParticleEffect::Initialize(m_pFX, m_pTexSRV);

	SetParticleInterval(0.1f);
	SetEmitPosW(XMFLOAT3(0.0f, 0.0f, 0.0f));
	SetEmitDirW(XMFLOAT3(0.0f, 0.0f, 0.0f));
	SetParticleSize(0.5f);
	SetParticleLife(6.0f);
	SetRandomVectorIntensity(1.5f);
	SetRandomVectorRange(1.0f);
	SetAccelIntensity(0.4f);
}

void RainParticleEffect::Initialize()
{
	m_pFX = XLParticleSystem::CompileFX(L"../../../XLParticleSystem/FX/LineParticleEffect.fx");
	m_pTexSRV = XLParticleSystem::LoadTexture(L"../../../XLParticleSystem/Textures/particle2.dds");

	XLParticleEffect::Initialize(m_pFX, m_pTexSRV);

	SetParticleInterval(0.001f);
	SetEmitPosW(XMFLOAT3(0.0f, 0.0f, 0.0f));
	SetParticleLife(0.8f);
}
