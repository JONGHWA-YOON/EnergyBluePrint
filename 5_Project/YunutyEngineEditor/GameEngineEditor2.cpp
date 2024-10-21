module;
#include <d3d11.h>
#include "imgui.h"
#include "backends/imgui_impl_vulkan.h"
#include "YunutyEngine.h"
#include "Walnut/Application.h"
#include "Walnut/Image.h"
#include "vulkan/vulkan.h"
#include "vulkan/vulkan_win32.h"
//import walnut;
module editor.GameEngineEditor;

#define MAX_UNDO_SIZE 20
#define MAX_REDO_SIZE 20

using namespace yunutyEngine;

VkDescriptorSet GameEngineEditor::testDescriptorSet{0};
VkImageView GameEngineEditor::testImageView{};
GameEngineEditor* GameEngineEditor::testInstance{nullptr};
/// 이윤우 : main 함수는 Walnut 헤더에서 정의되며, 에디터의 실질적인 진입 지점은 여기다.
#pragma comment(lib, "d3d11.lib")
unsigned int GetMemoryTypeIndex(unsigned int typeBits, VkMemoryPropertyFlags memoryPropertyFlags);
void GameEngineEditor::InitTestDX()
{
    D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, nullptr, 0, D3D11_SDK_VERSION, &testD3DDevice, nullptr, &testD3DDeviceContext);
    D3D11_TEXTURE2D_DESC desc
    {
        .Width = 100,
        .Height = 100,
        .MipLevels = 1,
        .ArraySize = 1,
        .Format = DXGI_FORMAT_R8G8B8A8_UNORM,
        .SampleDesc{.Count = 1},
        .Usage = D3D11_USAGE_DEFAULT,
        .BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET,
        .CPUAccessFlags = 0,
        .MiscFlags = D3D11_RESOURCE_MISC_SHARED,
    };
    testD3DDevice->CreateTexture2D(&desc, nullptr, &testD3DTexture);
    D3D11_RENDER_TARGET_VIEW_DESC rtvDesc
    {
        .Format = desc.Format,
        .ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D,
        .Texture2D{.MipSlice = 0}
    };
    testD3DDevice->CreateRenderTargetView(testD3DTexture, &rtvDesc, &testRTV);
    float color_red[]{ 0,1,0,1 };
    float color_blue[]{ 0,0,1,1 };
    testD3DDeviceContext->ClearRenderTargetView(testRTV, color_red);
    testD3DDeviceContext->Flush();
    HANDLE sharedHandle{ nullptr };
    IDXGIResource* dxgiResource{ nullptr };
    testD3DTexture->QueryInterface<IDXGIResource>(&dxgiResource);
    dxgiResource->GetSharedHandle(&sharedHandle);
    dxgiResource->Release();
    GetSharedHandleTest(sharedHandle);
}
void GameEngineEditor::GetSharedHandleTest(void* handle)
{
    //VkExternalMemoryHandleTypeFlagBits
    VkExternalMemoryImageCreateInfo externalMemoryImageInfo
    {
        .handleTypes = VK_EXTERNAL_MEMORY_HANDLE_TYPE_D3D11_TEXTURE_KMT_BIT,
    };
    VkImageCreateInfo imageCreateInfo
    {
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .pNext = &externalMemoryImageInfo,
        .imageType = VK_IMAGE_TYPE_2D,
        .format = VK_FORMAT_R8G8B8A8_UNORM,
        .extent{.width = 100,.height = 100,.depth = 1},
        .mipLevels = 1,
        .arrayLayers = 1,
        .samples = VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT,
        .tiling = VK_IMAGE_TILING_OPTIMAL,
        .usage = VkImageUsageFlagBits::VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
    };
    VkImage image;
    VkImageView imageView;
    VkMemoryRequirements memoryRequirements;
    vkCreateImage(Walnut::Application::GetDevice(), &imageCreateInfo, nullptr, &image);
    vkGetImageMemoryRequirements(Walnut::Application::GetDevice(), image, &memoryRequirements);
    VkImportMemoryWin32HandleInfoKHR importMemoryInfo
    {
        .handleType = VK_EXTERNAL_MEMORY_HANDLE_TYPE_D3D11_TEXTURE_KMT_BIT,
        .handle = handle,
    };
    VkMemoryAllocateInfo allocateInfo
    {
        .pNext = &importMemoryInfo,
        .allocationSize = memoryRequirements.size,
        .memoryTypeIndex = GetMemoryTypeIndex(memoryRequirements.memoryTypeBits,VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT),
    };
    VkDeviceMemory deviceMemory;
    vkAllocateMemory(Walnut::Application::GetDevice(), &allocateInfo, nullptr, &deviceMemory);
    vkBindImageMemory(Walnut::Application::GetDevice(), image, deviceMemory, 0);
    VkImageViewCreateInfo imageViewCreateInfo
    {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .image = image,
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format = VK_FORMAT_B8G8R8A8_UNORM,
        .components
        {
            .r = VK_COMPONENT_SWIZZLE_R,
            .g = VK_COMPONENT_SWIZZLE_G,
            .b = VK_COMPONENT_SWIZZLE_B,
            .a = VK_COMPONENT_SWIZZLE_A,
        },
        .subresourceRange
        {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1,
        },
    };
    auto result = vkCreateImageView(Walnut::Application::GetDevice(), &imageViewCreateInfo, nullptr, &imageView);
    // VkDescriptorSet 할당 
    VkSampler textureSampler;
    VkSamplerCreateInfo samplerInfo
    {
        .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
        .magFilter = VK_FILTER_LINEAR,
        .minFilter = VK_FILTER_LINEAR,
        .mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
        .addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT,
        .addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT,
        .addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT,
        .mipLodBias = 0.0f,
        .anisotropyEnable = VK_FALSE,
        .maxAnisotropy = 1.0f,
        .compareEnable = VK_FALSE,
        .compareOp = VK_COMPARE_OP_ALWAYS,
        .minLod = 0.0f,
        .maxLod = 0.0f,
        .borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK,
        .unnormalizedCoordinates = VK_FALSE,
    };

    if (vkCreateSampler(Walnut::Application::GetDevice(), &samplerInfo, nullptr, &textureSampler) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create texture sampler!");
    }
    testImageView = imageView;
    testDescriptorSet = ImGui_ImplVulkan_AddTexture(textureSampler, imageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}
unsigned int GetMemoryTypeIndex(unsigned int typeBits, VkMemoryPropertyFlags memoryPropertyFlags)
{
    VkPhysicalDeviceMemoryProperties memoryProperties;
    vkGetPhysicalDeviceMemoryProperties(Walnut::Application::GetPhysicalDevice(), &memoryProperties);

    for (unsigned int i = 0; i < memoryProperties.memoryTypeCount; i++)
    {
        if ((typeBits & (i << i)) != 0 && (memoryProperties.memoryTypes[i].propertyFlags & memoryPropertyFlags))
            return i;
    }
    throw "memory type not found";
}
