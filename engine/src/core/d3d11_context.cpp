#include "d3d11_context.hpp"

void xpe::core::D3D11RenderingContext::Init(Window& window)
{
    _swapChain = nullptr;
    _swapChainTexture = nullptr;
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

    _swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&_swapChainTexture);
}

void xpe::core::D3D11RenderingContext::Free()
{
    _swapChainTexture->Release();
    _immContext->Release();
    _device->Release();
    _swapChain->Release();
}

xpe::core::RenderTarget xpe::core::D3D11RenderingContext::CreateRenderTarget(GPUResource* texture)
{
    RenderTarget rt = {};

    _device->CreateRenderTargetView((ID3D11Resource*)texture, nullptr, (ID3D11RenderTargetView**)&rt.ColorTargetResource);

    return rt;
}