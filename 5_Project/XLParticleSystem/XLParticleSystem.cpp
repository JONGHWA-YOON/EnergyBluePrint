#include <algorithm>

#include "XLParticleSystem.h"
#include "XLParticleEffect.h"


#include "DDSTextureLoader.h"

#include <dxgi1_2.h>
#include <d3d11.h>
#include <d3dcompiler.h>

ID3D11Device* s_pDevice;
ID3D11DeviceContext* s_pDeviceContext;
UINT s_MaxParticles;

XLParticleSystem::XLParticleSystem()
	:mGameTime(0), mTimeStep(0), mAge(0),
	mEyePosW(), mEmitPosW(), mEmitDirW(),
	m_Effects()
{

}

XLParticleSystem::~XLParticleSystem()
{

}


void XLParticleSystem::SetEyePos(const XMFLOAT3& eyePosW)
{
	mEyePosW = eyePosW;
}

void XLParticleSystem::SetEmitPos(const XMFLOAT3& emitPosW)
{
	mEmitPosW = emitPosW;
}

void XLParticleSystem::SetEmitDir(const XMFLOAT3& emitDirW)
{
	mEmitDirW = emitDirW;
}

void XLParticleSystem::Init(ID3D11Device* device, ID3D11DeviceContext* deviceContext, UINT maxParticles)
{
	s_pDevice = device;
	s_pDeviceContext = deviceContext;
	s_MaxParticles = maxParticles;

	m_pDevice = device;
	m_pDeviceContext = deviceContext;

}

void XLParticleSystem::Reset()
{
	for (auto& e : m_Effects)
	{
		e->mFirstRun = true;
	}

	mAge = 0.0f;
}

UINT XLParticleSystem::GetMaxParticles()
{
	return s_MaxParticles;
}

void XLParticleSystem::Update(float dt, XMMATRIX ViewProj, XMFLOAT3 CameraPosition)
{
	mTimeStep = dt;
	mGameTime += dt;

	for (auto& e : m_Effects)
	{
		e->SetViewProj(ViewProj);
		e->SetEyePosW(CameraPosition);
		e->SetGameTime(mGameTime);
		e->SetTimeStep(mTimeStep);

		e->Update();
	}
}

void XLParticleSystem::Render()
{
	/// 사용할 버텍스 버퍼 선택

	UINT stride = sizeof(Particle); // Size of One Particle in Byte
	UINT offset = 0;

	for (auto& e : m_Effects)
	{
		m_pDeviceContext->IASetInputLayout(e->ParticleInputLayout);
		m_pDeviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);

		// 최초 실행이면 초기화용 VB(Emitter Paricle) 사용, 그렇지 않으면 현재 파티클 목록을 담은 VB 사용
		if (e->mFirstRun)
		{
			m_pDeviceContext->IASetVertexBuffers(0, 1, &(e->mInitVB), &stride, &offset);
		}
		else
		{
			m_pDeviceContext->IASetVertexBuffers(0, 1, &(e->mDrawVB), &stride, &offset);
		}

		// 현재 입자 목록을 'SO 전용 테크닉'으로 그려서 입자 갱신
		// 갱신된 입자들은 mStreamOutVB에 저장된다
		m_pDeviceContext->SOSetTargets(1, &(e->mStreamOutVB), &offset);


		e->StreamOutTech->GetPassByIndex(0)->Apply(0, m_pDeviceContext);

		// 최초 실행이면 초기화용 VB(Emitter Paricle) 사용, 그렇지 않으면 현재 파티클 목록을 담은 VB 사용
		if (e->mFirstRun)
		{
			m_pDeviceContext->Draw(1, 0);
			e->mFirstRun = false;
		}
		else
		{
			m_pDeviceContext->DrawAuto(); // 드로우콜을 하면 스트림 출력 전용 테크닉으로 간다?
		}


		// SO 전용 패스가 끝나면 정점 버퍼를 unbinding
		ID3D11Buffer* bufferArray[1] = { 0 };
		m_pDeviceContext->SOSetTargets(1, bufferArray, &offset);

		// 정점 버터들을 맞바꾼다 (핑퐁)
		std::swap(e->mDrawVB, e->mStreamOutVB);

		// 방금 스트림 출력된, 갱신된 파티클들을 담은 VB를 화면에 그린다.
		m_pDeviceContext->IASetVertexBuffers(0, 1, &(e->mDrawVB), &stride, &offset);
		e->DrawTech->GetPassByIndex(0)->Apply(0, m_pDeviceContext);
		m_pDeviceContext->DrawAuto();


	}
}

void XLParticleSystem::AddParticleEffect(XLParticleEffect* effect)
{
	effect->Initialize();
	effect->BuildVB();
	// TODO: Effect 안에 있는 것이 맞는 것 같다. 왜? 파티클에 들어갈 정보가 달라지면 인풋레이아웃도 바뀌어야 하기 때문..
	//BuildVB(effect);
	m_Effects.emplace_back(effect);
}

ID3D11ShaderResourceView* XLParticleSystem::LoadTexture(LPCWSTR textureFilePath)
{
	/// Particle 텍스쳐 생성
	ID3D11Resource* ParticleTexture = nullptr;
	ID3D11ShaderResourceView* ParticleSRV = nullptr;

	// dds 파일로부터 Shader Resource View 생성
	HRESULT hr = CreateDDSTextureFromFile(s_pDevice, textureFilePath, &ParticleTexture, &ParticleSRV);

	return ParticleSRV;
}

ID3DX11Effect* XLParticleSystem::CompileFX(LPCWSTR fxFilePath)
{
	ID3DX11Effect* pFX = nullptr;

	ID3DBlob* compiledShader = nullptr;
	HRESULT hr = D3DCompileFromFile(fxFilePath, NULL, NULL, NULL, "fx_5_0", D3DCOMPILE_DEBUG, NULL, &compiledShader, NULL);

	// Effect 객체 생성
	hr = D3DX11CreateEffectFromMemory(compiledShader->GetBufferPointer(), compiledShader->GetBufferSize(), 0, s_pDevice, &(pFX));

	return pFX;
}