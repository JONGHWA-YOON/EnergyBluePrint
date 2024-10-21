module;
#include <d3d11.h>
#include "YunutyEngine.h"
export module utility.engineGraphics;
import utility.DX11Utility;

export namespace utility
{
    namespace engineGraphics
    {
        inline ID3D11ShaderResourceView* GetRenderedOutput()
        {
            auto handle = yunutyEngine::graphics::Renderer::SingleInstance().QuerySharedOutputHandle();
            static std::map<void*, ID3D11ShaderResourceView*> srvs;
            if (srvs.find(handle) == srvs.end())
            {
                D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc
                {
                    .Format = DXGI_FORMAT_R32G32B32A32_FLOAT,
                    .ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D,
                    .Texture2D
                    {
                        .MostDetailedMip = 0,
                        .MipLevels = 1,
                    },
                };
                ID3D11Resource* receivedResource{ nullptr };
                ID3D11ShaderResourceView* srv{ nullptr };
                utility::dx11Utility::g_pd3dDevice->OpenSharedResource(handle, IID_PPV_ARGS(&receivedResource));
                utility::dx11Utility::g_pd3dDevice->CreateShaderResourceView(receivedResource, &srvDesc, &srv);
                srvs[handle] = srv;
            }
            return srvs[handle];

        }
    }
}