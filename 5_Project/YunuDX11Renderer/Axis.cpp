#include "Axis.h"
#include <DirectXColors.h>	// 이미 많은 부분 DX11과 겹친다.


render_object::Helper::Axis::Axis(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, ID3D11RasterizerState* pRS)
	: md3dDevice(pDevice), md3dDeviceContext(pDeviceContext), m_pRenderstate(pRS),
	mVB(nullptr), mIB(nullptr), mFX(nullptr), mTech(nullptr), mfxWorldViewProj(nullptr), mInputLayout(nullptr),
	mWorld(), mView(), mProj()
{

}

render_object::Helper::Axis::~Axis()
{
	mFX->Release();
	mInputLayout->Release();

	mVB->Release();
	mIB->Release();
}


void render_object::Helper::Axis::Initialize()
{
	BuildGeometryBuffers();
	BuildFX();
	BuildVertexLayout();
}

void render_object::Helper::Axis::Update(const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& proj)
{
	mWorld = world;
	mView = view;
	mProj = proj;
}

void render_object::Helper::Axis::RenderToTexture()
{
	// 입력 배치 객체 셋팅
	md3dDeviceContext->IASetInputLayout(mInputLayout);
	md3dDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	// 인덱스버퍼와 버텍스버퍼 셋팅
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	md3dDeviceContext->IASetVertexBuffers(0, 1, &mVB, &stride, &offset);
	md3dDeviceContext->IASetIndexBuffer(mIB, DXGI_FORMAT_R32_UINT, 0);

	/// WVP TM등을 셋팅
	// Set constants
	XMMATRIX worldViewProj = mWorld * mView * mProj;
	mfxWorldViewProj->SetMatrix(reinterpret_cast<float*>(&worldViewProj));

	// 렌더스테이트
	md3dDeviceContext->RSSetState(m_pRenderstate);

	// 테크닉은...
	D3DX11_TECHNIQUE_DESC techDesc;
	mTech->GetDesc(&techDesc);

	// 렌더패스는...
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		mTech->GetPassByIndex(p)->Apply(0, md3dDeviceContext);

		// 6개의 인덱스로 축을 그린다.
		md3dDeviceContext->DrawIndexed(6, 0, 0);
	}
}

void render_object::Helper::Axis::BuildGeometryBuffers()
{
	// 정점 버퍼를 생성한다. 
	// 각 축에 맞도록 6개의 정점을 만들었다.
	Vertex vertices[] =
	{
		{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT4((const float*)&Colors::Red)  },	// x축 (빨강)
		{ XMFLOAT3(10.0f, 0.0f, 0.0f), XMFLOAT4((const float*)&Colors::Red)  },	

		{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT4((const float*)&Colors::Green)},	// y축 (초록)
		{ XMFLOAT3(0.0f, 10.0f, 0.0f), XMFLOAT4((const float*)&Colors::Green)},

		{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT4((const float*)&Colors::Blue)	},	// z축 (파랑)
		{ XMFLOAT3(0.0f, 0.0f, 10.0f), XMFLOAT4((const float*)&Colors::Blue) }
	};

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex) * 6;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = vertices;
	HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mVB));


	// 인덱스 버퍼를 생성한다.
	// 역시 3개의 축을 나타내도록 했다.

	UINT indices[] = {
		// x축
		0, 1,

		// y축
		2, 3,

		// z축
		4, 5,
	};

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * 6;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = indices;
	HR(md3dDevice->CreateBuffer(&ibd, &iinitData, &mIB));
}

void render_object::Helper::Axis::BuildFX()
{
	std::ifstream fin("fx/color.fxo", std::ios::binary);

	fin.seekg(0, std::ios_base::end);
	int size = (int)fin.tellg();
	fin.seekg(0, std::ios_base::beg);
	std::vector<char> compiledShader(size);

	fin.read(&compiledShader[0], size);
	fin.close();

	HR(D3DX11CreateEffectFromMemory(&compiledShader[0], size,
		0, md3dDevice, &mFX));

	mTech = mFX->GetTechniqueByName("ColorTech");
	mfxWorldViewProj = mFX->GetVariableByName("gWorldViewProj")->AsMatrix();
}

void render_object::Helper::Axis::BuildVertexLayout()
{
	// Create the vertex input layout.
	D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	// Create the input layout
	D3DX11_PASS_DESC passDesc;
	mTech->GetPassByIndex(0)->GetDesc(&passDesc);
	HR(md3dDevice->CreateInputLayout(vertexDesc, 2, passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize, &mInputLayout));
}
