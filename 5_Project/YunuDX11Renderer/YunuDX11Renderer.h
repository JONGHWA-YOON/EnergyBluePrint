#pragma once

//#include "IYunuGI3DRenderer.h"
#include <Windows.h>
//#include <wrl.h>
//#include "DebugSphere.h"
//#include "DebugCone.h"
//#include "DebugArrow.h"
//#include "TextureImage.h"
#include "LazyObject.h"
//#include "GameTimer.h"		// Timer인데 이제 외부에서 받는다. (설계변경)
#include "d3dUtil.h"
#include "DeferredBuffers.h"

#include "Vertex.h"	
#include "InputLayout.h"	
#include <vector>

class DXTKFont;
class Camera;
class ASEParser;
class DefferedBuffers;
//class DebugSphere;

namespace yunuGIAdapter { class RendererAdapter; }
namespace render_object
{
    namespace Helper
    {
        class Axis;
        class Grid;
        class DebugSphere;
        class DebugArrow;
        class DebugCone;
    }
    //class MeshObject;
    class SkinnedMesh;
    class Box;
    class Crate;
    class TextureImage;
    //class SiegeTank;
}

#ifdef YunuDX11DLL_EXPORTS
#define YunuDX11API __declspec(dllexport)
#else
#define YunuDX11API __declspec(dllimport)
#endif
using namespace render_object;
using namespace render_object::Helper;


class YunuDX11Renderer
{
public:
    ~YunuDX11Renderer();

    // DX를 초기화한다.
    bool Initialize();
    // 그림을 그릴 대상 윈도우를 정함
    void SetOutputWindow(HWND hWnd);
    void ResizeResolution(unsigned int width, unsigned int height);

    // 렌더링 시작
    void BeginRender();
    void RenderToTexture(std::vector<D3D11_VIEWPORT>& viewPorts);
    // 렌더링 마무리. 그릴 것 다 그린 상태에서 Present
    void EndRender();
    // 엔진을 종료한다.
    //void Release();
    void SetSwapChainAsRenderTarget();


    void Update(float deltaTime);
    static LazyObject<YunuDX11Renderer> SingleInstance;
    friend LazyObject<YunuDX11Renderer>;
protected:
    // 창 크기가 변했을 때 처리해야 할 것들
    void OnResize();
    // 화면에 디버깅 정보 출력
private:
    YunuDX11Renderer();
    void Draw_Status();

    float GetAspectRatio() const;


    // 시간 관련
private:
    float m_deltaTime;


    /// DirectX 11 관련 멤버

    // 초기화
private:
    //HINSTANCE mhAppInst;
    HWND      mhMainWnd;				/// 윈도 핸들
    bool      mAppPaused;
    bool      mMinimized;
    bool      mMaximized;
    bool      mResizing;				/// 이런 것들을 위해서 Win32App등의 윈도 관련 클래스를 만들면 더 깔끔해진다.
    UINT      m4xMsaaQuality;


    ID3D11Device* md3dDevice;						// D3D11 디바이스
    ID3D11DeviceContext* md3dDeviceContext;		// 디바이스 컨텍스트
    IDXGISwapChain* mSwapChain;						// 스왑체인
    ID3D11Texture2D* mDepthStencilBuffer;			// 뎁스 스탠실 버퍼
    // 백 버퍼를 만드는 것이 렌더 타겟 뷰,
    ID3D11RenderTargetView* mRenderTargetView;		// 랜더 타겟 뷰
    ID3D11DepthStencilView* mDepthStencilView;		// 뎁스 스탠실 뷰
    // 출력의 거의 마지막에 관여하는 것이 뷰 포트.
    D3D11_VIEWPORT mScreenViewport;					// 뷰포트


    /// 봐서 WinMain으로 보낼 것은 보내고 여기 남길 것은 남기자.
    // Derived class should set these in derived constructor to customize starting values.
    std::wstring mMainWndCaption;		/// 이런것이나
    D3D_DRIVER_TYPE md3dDriverType;

public:
    int mClientWidth;					///
    int mClientHeight;					/// 이런 것들

private:
    bool mEnable4xMsaa;


    // 초기화 관련 구조체들
    D3D_FEATURE_LEVEL featureLevel;


    // DXTKFont
private:
    DXTKFont* m_pFont;

    // Camera
private:
    Camera* m_pCamera;

    // 	float mTheta;
    // 	float mPhi;
    // 	float mRadius;

    POINT mLastMousePos;

    void OnMouseDown(int x, int y);
    void OnMouseUp(int x, int y);
    void OnMouseMove(int btnState, int x, int y);

    // Mesh (Box)
private:
    ID3D11Buffer* mBoxVB;
    ID3D11Buffer* mBoxIB;

    ID3DX11Effect* mFX;
    ID3DX11EffectTechnique* mTech;
    ID3DX11EffectMatrixVariable* mfxWorldViewProj;

    ID3D11InputLayout* mInputLayout;

    XMFLOAT4X4 mWorld;	// Transform Matrix
    XMFLOAT4X4 mView;
    XMFLOAT4X4 mProj;

private:
    //void BuildGeometryBuffers();
    //void BuildFX();
    //void BuildFX_Create();
    //void BuildVertexLayout();


    /// Render State
    // 미리 여러 세트를 만들어두고 스위칭한다.
private:
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mRenderTargetSRV;
    ID3D11RasterizerState* mWireframeRS;
    ID3D11RasterizerState* mSolidRS;

    // 폰트때문에 뎁스스탠실 스테이트가 강제가 됐다.
    ID3D11DepthStencilState* NormalDSS;


private:
    void CreateRenderStates();




    /// 어댑터 정보
private:
    DXGI_ADAPTER_DESC1 m_AdapterDesc1;
    HRESULT GetAdapterInfo();


    /// Axis와 Grid등 helper그리기
private:
    Axis* m_WorldAxes;
    Grid* m_Grid;
    /// 해골 그리기. 메쉬 오브젝트. Object를 따로 만들어서 Render하는 예시
private:
    SkinnedMesh* m_MeshObject;
    SkinnedMesh* m_MeshObject2;
    SkinnedMesh* m_MeshObject3;
    TextureImage* m_deferredDebugTextures[DeferredBuffers::GBUFFER_COUNT];
    //MeshObject* m_MeshObject2;


    /// 예시용 박스
private:
    Box* m_TestBox;

    Crate* m_TestCrate;
    //SiegeTank* m_SiegeTank;
    DebugSphere* m_debugObj_Point;
    DebugCone* m_debugObj_Spot;
    DebugArrow* m_debugObj_Spot2;
    DebugArrow* m_debugObj_Dir;
    Axis* m_debugAxis;

    ASEParser* m_pASEParser;
private:
    // changes as assignment changes
    void Update_LightsControl(float delta);
    bool m_isDirectionalLightActive = true;
    bool m_isPointLightActive = false;
    bool m_isSpotLightActive = false;
    bool m_isWhiteLight = false;
    DirectionalLight mDirLights[3];
    PointLight mPointLights[3];
    SpotLight mSpotLights[3];
    template<int key>
    bool WasKeyJustPressed()
    {
        static bool keyPressed = false;
        if (GetAsyncKeyState(key) & 0x8000)
        {
            if (keyPressed)
                return false;
            keyPressed = true;
            return true;
        }
        else
        {
            keyPressed = false;
            return false;
        }
    }
    friend yunuGIAdapter::RendererAdapter;
};
