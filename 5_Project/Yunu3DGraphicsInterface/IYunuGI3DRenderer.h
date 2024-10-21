#pragma once

#include <Windows.h>
#include "IUnknown.h"

class ID3D11Device;
class ID3D11DeviceContext;
class ID3D11ShaderResourceView;
class D3D12_GPU_DESCRIPTOR_HANDLE;

// 렌더러는 출력화면을 제어하고, 이미지를 렌더링 해주는 역할을 한다.
// 렌더러는 아무런 렌더링 객체를 생성하지도 않고, 피킹과 같은 이벤트 처리를 해주지도 않는다.
namespace yunuGI
{
    struct I3DRendererDesc
    {
        unsigned int width = 1024;
        unsigned int height = 768;
    };
    class I3DRenderer : public yunuGI::IUnknown
    {
    public:
        // 렌더러를 초기화하는 부분은 모두 여기서 처리한다.
        I3DRenderer() {};
        virtual ~I3DRenderer() {}

        // DX를 초기화한다.
        //virtual bool Initialize() = 0;

        // 그림을 그릴 대상 윈도우를 정함, 이 함수를 사용하면
        // 오직 그래픽스엔진만이 해당 윈도우에 이미지를 그리는 것으로 가정한다.
        virtual void SetOutputWindow(UINT64 hWnd) = 0;
        // 시간에 따라 상태가 변화되어야 하는 애니메이션, 파티클의 경우에 적용된다.
        virtual void Update(float deltaTime) = 0;
        // 렌더링 시작
        //virtual void BeginRender() = 0;
        virtual void Render() = 0;

        // 렌더링 마무리. 그릴 것 다 그린 상태에서 Present
        //virtual void EndRender() = 0;

        // 창 크기가 변했을 때 처리해야 할 것들
        //virtual void OnResize() = 0;
        // 해상도 변경을 요청하는 함수
        virtual void ResizeResolution(unsigned int width, unsigned int height) = 0;

        // 혹여나 내부 그래픽스 구조를 알아야 할 때
        //virtual ID3D11Device* QueryD3d11Device() { return nullptr; }
        //virtual ID3D11DeviceContext* QueryD3dDeviceContext() { return nullptr; }
        // void 포인터로 렌더 출력물을 전달해주기 위한 쿼리
        virtual void* QuerySharedOutputHandle() = 0;
        //virtual ID3D11ShaderResourceView* QueryD3D11RenderOutputSRV(ID3D11Device* requesting) { return nullptr; }
        // imgui에 렌더 출력물을 전달해주기 위한 쿼리
        //virtual D3D12_GPU_DESCRIPTOR_HANDLE* QueryD3D12RenderOutputGDH() { return nullptr; }
    };
}

