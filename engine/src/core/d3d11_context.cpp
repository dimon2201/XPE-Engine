#include "d3d11_context.hpp"

void xpe::core::D3D11RenderingContext::Init(Window& window)
{
    _swapChain = nullptr;
    _swapChainTexture.Instance = nullptr;
    _device = nullptr;
    _immContext = nullptr;

    DXGI_MODE_DESC bufferDesc = {};
    bufferDesc.Width = 4096;
    bufferDesc.Height = 4096;
    bufferDesc.RefreshRate.Numerator = 60;
    bufferDesc.RefreshRate.Denominator = 1;
    bufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    bufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    bufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

    DXGI_SWAP_CHAIN_DESC swapChainDesc; 
    swapChainDesc.BufferDesc = bufferDesc;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.BufferCount = 1;
    swapChainDesc.OutputWindow = (HWND)window.GetWin32HWND();
    swapChainDesc.Windowed = TRUE;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        0,
        0,
        nullptr,
        0,
        D3D11_SDK_VERSION,
        &swapChainDesc,
        (IDXGISwapChain**)&_swapChain,
        (ID3D11Device**)&_device,
        nullptr,
        (ID3D11DeviceContext**)&_immContext
    );

    _swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&_swapChainTexture.Instance);
}

void xpe::core::D3D11RenderingContext::Free()
{
    ((ID3D11Texture2D*)_swapChainTexture.Instance)->Release();
    _immContext->Release();
    _device->Release();
    _swapChain->Release();
}

xpe::core::RenderTarget xpe::core::D3D11RenderingContext::CreateRenderTarget(GPUResource* color, GPUResource* depth)
{
    RenderTarget rt = {};
    rt.ColorTexture = color;
    rt.DepthTexture = depth;

    D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
    rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    rtvDesc.Texture2D.MipSlice = 0;
    rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

    _device->CreateRenderTargetView((ID3D11Resource*)color, &rtvDesc, (ID3D11RenderTargetView**)&rt.ColorTargetInstance);

    D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
    dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
    dsvDesc.Texture2D.MipSlice = 0;

    _device->CreateDepthStencilView((ID3D11Resource*)depth, &dsvDesc, (ID3D11DepthStencilView**)&rt.DepthTargetInstance);

    return rt;
}

void xpe::core::D3D11RenderingContext::FreeRenderTarget(const RenderTarget& renderTarget)
{
    if (renderTarget.ColorTargetInstance != nullptr) {
        ((ID3D11RenderTargetView*)renderTarget.ColorTargetInstance)->Release();
    }
}

void xpe::core::D3D11RenderingContext::ClearRenderTarget(const RenderTarget& renderTarget, const glm::vec4& color, const f32 depth)
{
    _immContext->ClearRenderTargetView((ID3D11RenderTargetView*)renderTarget.ColorTargetInstance, &color.x);
    _immContext->ClearDepthStencilView((ID3D11DepthStencilView*)renderTarget.DepthTargetInstance, D3D11_CLEAR_DEPTH, depth, 0);
}