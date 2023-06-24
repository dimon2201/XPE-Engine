#include <d3dcompiler.h>

#include "d3d11_context.hpp"
#include "memory_pool.hpp"

void xpe::core::D3D11RenderingContext::Init(Window& window)
{
    _swapChain = nullptr;
    _swapChainTexture.Instance = nullptr;
    _device = nullptr;
    _immContext = nullptr;

    DXGI_MODE_DESC bufferDesc = {};
    bufferDesc.Width = window.GetWidth();
    bufferDesc.Height = window.GetHeight();
    bufferDesc.RefreshRate.Numerator = 60;
    bufferDesc.RefreshRate.Denominator = 1;
    bufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    bufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    bufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

    DXGI_SWAP_CHAIN_DESC swapChainDesc = {}; 
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
        D3D11_CREATE_DEVICE_DEBUG,
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
    _swapChainTexture = CreateTexture(_swapChainTexture.Instance, glm::ivec2(window.GetWidth(), window.GetHeight()));
    _rt = CreateRenderTarget(glm::ivec2(window.GetWidth(), window.GetHeight()), &_swapChainTexture, nullptr, nullptr, nullptr);
    _sampler = CreateSampler();
}

void xpe::core::D3D11RenderingContext::Free()
{
    FreeRenderTarget(_rt);
    ((ID3D11Texture2D*)_swapChainTexture.Instance)->Release();
    _immContext->Release();
    _device->Release();
    _swapChain->Release();
}

xpe::core::xRenderTarget xpe::core::D3D11RenderingContext::CreateRenderTarget(const glm::ivec2& dimensions, const GPUResource* colorTexture, const GPUResource* colorView, const GPUResource* depthTexture, const GPUResource* depthView)
{
    xRenderTarget rt = {};

    rt.ColorTexture = colorTexture == nullptr ? new GPUResource() : (GPUResource*)colorTexture;
    rt.DepthTexture = depthTexture == nullptr ? new GPUResource() : (GPUResource*)depthTexture;
    rt.Dimensions = dimensions;
    
    if (colorTexture == nullptr)
    {
        D3D11_TEXTURE2D_DESC colorTextureDesc = {};
        colorTextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
        colorTextureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        colorTextureDesc.Width = dimensions.x;
        colorTextureDesc.Height = dimensions.y;
        colorTextureDesc.ArraySize = 1;
        colorTextureDesc.SampleDesc.Count = 1;
        colorTextureDesc.MipLevels = 1;
        colorTextureDesc.Usage = D3D11_USAGE_DEFAULT;

        _device->CreateTexture2D(&colorTextureDesc, nullptr, (ID3D11Texture2D**)&rt.ColorTexture->Instance);
    
        D3D11_SHADER_RESOURCE_VIEW_DESC colorSRV = {};
        colorSRV.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        colorSRV.Texture2D.MipLevels = 1;
        colorSRV.Texture2D.MostDetailedMip = 0;
        colorSRV.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;

        _device->CreateShaderResourceView((ID3D11Resource*)rt.ColorTexture->Instance, &colorSRV, (ID3D11ShaderResourceView**)&rt.ColorTexture->ViewInstance);
    }

    if (depthTexture == nullptr)
    {
        D3D11_TEXTURE2D_DESC depthTextureDesc = {};
        depthTextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
        depthTextureDesc.Format = DXGI_FORMAT_R32_TYPELESS;
        depthTextureDesc.Width = dimensions.x;
        depthTextureDesc.Height = dimensions.y;
        depthTextureDesc.ArraySize = 1;
        depthTextureDesc.SampleDesc.Count = 1;
        depthTextureDesc.MipLevels = 1;
        depthTextureDesc.Usage = D3D11_USAGE_DEFAULT;

        _device->CreateTexture2D(&depthTextureDesc, nullptr, (ID3D11Texture2D**)&rt.DepthTexture->Instance);
    }

    if (colorView == nullptr)
    {
        D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
        rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        rtvDesc.Texture2D.MipSlice = 0;
        rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

        _device->CreateRenderTargetView((ID3D11Resource*)rt.ColorTexture->Instance, &rtvDesc, (ID3D11RenderTargetView**)&rt.ColorTargetInstance);
    }

    if (depthView == nullptr)
    {
        D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
        dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
        dsvDesc.Texture2D.MipSlice = 0;
        dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

        _device->CreateDepthStencilView((ID3D11Resource*)rt.DepthTexture->Instance, &dsvDesc, (ID3D11DepthStencilView**)&rt.DepthTargetInstance);
    }

    return rt;
}

void xpe::core::D3D11RenderingContext::BindRenderTarget(const xRenderTarget* renderTarget)
{
    _boundRT = (xRenderTarget*)renderTarget;

    if (_boundRT != nullptr)
    {
        _immContext->OMSetRenderTargets(1, (ID3D11RenderTargetView**)&_boundRT->ColorTargetInstance, nullptr);
    }
    else
    {
        _boundRT = &_rt;
        _immContext->OMSetRenderTargets(1, (ID3D11RenderTargetView**)&_boundRT->ColorTargetInstance, nullptr);
    }
}

void xpe::core::D3D11RenderingContext::ClearRenderTarget(const glm::vec4& color, const f32 depth)
{
    if (_boundRT == nullptr) { return; }

    _immContext->ClearRenderTargetView((ID3D11RenderTargetView*)_boundRT->ColorTargetInstance, &color.x);
    _immContext->ClearDepthStencilView((ID3D11DepthStencilView*)_boundRT->DepthTargetInstance, D3D11_CLEAR_DEPTH, depth, 0);
}

glm::ivec2 xpe::core::D3D11RenderingContext::GetSwapChainDimensions()
{
    return _rt.Dimensions;
}

void xpe::core::D3D11RenderingContext::FreeRenderTarget(const xRenderTarget& renderTarget)
{
    if (renderTarget.ColorTexture != nullptr)
    {
        ((ID3D11Texture2D*)renderTarget.ColorTexture)->Release();
    }
    if (renderTarget.ColorTargetInstance != nullptr)
    {
        ((ID3D11RenderTargetView*)renderTarget.ColorTargetInstance)->Release();
    }
    if (renderTarget.DepthTexture != nullptr)
    {
        ((ID3D11Texture2D*)renderTarget.DepthTexture)->Release();
    }
    if (renderTarget.DepthTargetInstance != nullptr)
    {
        ((ID3D11DepthStencilView*)renderTarget.DepthTargetInstance)->Release();
    }
}

void xpe::core::D3D11RenderingContext::Present()
{
    _swapChain->Present(0, 0);
}

void xpe::core::D3D11RenderingContext::CreateShaderFromString(xShader& shader, const xShader::PrimitiveTopology& topology, const xShader::Type& type, const char* str, const char* funcName, const char* profile, const uword flags)
{
    shader.Topology = topology;

    ID3DBlob* shaderBlob = nullptr;
    ID3DBlob* errorBlob = nullptr;

    D3DCompile(
        str,
        strlen(str),
        nullptr,
        nullptr,
        nullptr,
        funcName,
        profile,
        (UINT)flags,
        0,
        &shaderBlob,
        &errorBlob
    );
    

    if (errorBlob != nullptr)
    {
        std::cout << (char*)errorBlob->GetBufferPointer() << std::endl;

        errorBlob->Release();
    }

    if (shaderBlob != nullptr)
    {
        if (type == xShader::Type::VERTEX)
        {
            shader.VertexShaderByteCode = shaderBlob->GetBufferPointer();
            shader.VertexShaderByteCodeSize = shaderBlob->GetBufferSize();
            _device->CreateVertexShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr, (ID3D11VertexShader**)&shader.VertexShader.Instance);
        }
        else if (type == xShader::Type::PIXEL)
        {
            _device->CreatePixelShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr, (ID3D11PixelShader**)&shader.PixelShader.Instance);
            shaderBlob->Release();
        }
    }
}

void xpe::core::D3D11RenderingContext::BindShader(const xShader* shader)
{
    _boundShader = (xShader*)shader;

    _immContext->VSSetShader((ID3D11VertexShader*)_boundShader->VertexShader.Instance, nullptr, 0);
    _immContext->PSSetShader((ID3D11PixelShader*)_boundShader->PixelShader.Instance, nullptr, 0);
}

void xpe::core::D3D11RenderingContext::FreeShader(const xShader& shader)
{
    if (shader.VertexShader.Instance != nullptr)
    {
        ((ID3D11VertexShader*)shader.VertexShader.Instance)->Release();
    }
    if (shader.VertexShaderByteCode != nullptr)
    {
        ((ID3DBlob*)shader.VertexShaderByteCode)->Release();
    }
    if (shader.PixelShader.Instance != nullptr)
    {
        ((ID3D11PixelShader*)shader.PixelShader.Instance)->Release();
    }
}

xpe::core::GPUResource xpe::core::D3D11RenderingContext::CreateTexture(const void* texture, const glm::ivec2& dimensions)
{
    GPUResource tex = {};

    if (texture == nullptr)
    {
        D3D11_TEXTURE2D_DESC textureDesc = {};
        textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
        textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        textureDesc.Width = dimensions.x;
        textureDesc.Height = dimensions.y;
        textureDesc.ArraySize = 1;
        textureDesc.SampleDesc.Count = 1;
        textureDesc.MipLevels = 1;
        textureDesc.Usage = D3D11_USAGE_DEFAULT;

        _device->CreateTexture2D(&textureDesc, nullptr, (ID3D11Texture2D**)&tex.Instance);
    }
    else
    {
        tex.Instance = (void*)texture;
    }

    return tex;
}

void xpe::core::D3D11RenderingContext::BindTexture(const GPUResource* texture, const xShader::Type& shaderType, const u32 slot)
{
    if (shaderType == xShader::Type::VERTEX)
    {
        _immContext->VSSetShaderResources(slot, 1, (ID3D11ShaderResourceView**)&texture->ViewInstance);
    }
    else if (shaderType == xShader::Type::PIXEL)
    {
        if (texture != nullptr)
        {
            _immContext->PSSetShaderResources(slot, 1, (ID3D11ShaderResourceView**)&texture->ViewInstance);
        }
        else
        {
            ID3D11ShaderResourceView* views = nullptr;
            _immContext->PSSetShaderResources(slot, 1, &views);
        }
    }
}

void xpe::core::D3D11RenderingContext::FreeTexture(const GPUResource* texture)
{
    if (texture->Instance != nullptr)
    {
        ((ID3D11Texture2D*)texture->Instance)->Release();
    }
}

xpe::core::GPUResource xpe::core::D3D11RenderingContext::CreateSampler()
{
    GPUResource sampler = {};

    D3D11_SAMPLER_DESC desc = {};
    desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
    desc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
    desc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
    desc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
    desc.BorderColor[0] = 1.0f;
    desc.BorderColor[1] = 0.0f;
    desc.BorderColor[2] = 0.0f;
    desc.BorderColor[3] = 1.0f;
    desc.ComparisonFunc = D3D11_COMPARISON_NEVER;

    _device->CreateSamplerState(&desc, (ID3D11SamplerState**)&sampler.Instance);

    return sampler;
}

void xpe::core::D3D11RenderingContext::BindSampler(const GPUResource* sampler)
{
    _immContext->PSSetSamplers(0, 1, (ID3D11SamplerState**)&sampler->Instance);
}

void xpe::core::D3D11RenderingContext::FreeSampler(const GPUResource* sampler)
{
    if (sampler->Instance != nullptr)
    {
        ((ID3D11SamplerState*)sampler->Instance)->Release();
    }
}

xpe::core::xBuffer xpe::core::D3D11RenderingContext::CreateBuffer(const xBuffer::xType& bufferType, usize byteSize, boolean duplicate)
{
    xBuffer buffer = {};
    buffer.Type = bufferType;
    buffer.AppendOffset = 0;

    if (duplicate == K_TRUE)
    {
        buffer.CPUMemory = GetMemoryPool(0)->Allocate(byteSize);
    }
    else
    {
        buffer.CPUMemory = nullptr;
    }

    D3D11_BUFFER_DESC bufferDesc = {};

    if (bufferType == xBuffer::xType::VERTEX)
    {
        bufferDesc.ByteWidth = (UINT)byteSize;
        bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
        bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    }
    else if (bufferType == xBuffer::xType::INDEX)
    {
        bufferDesc.ByteWidth = (UINT)byteSize;
        bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
        bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    }
    else if (bufferType == xBuffer::xType::INSTANCE)
    {
        bufferDesc.ByteWidth = (UINT)byteSize;
        bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
        bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
        bufferDesc.StructureByteStride = sizeof(xRenderInstance);
    }
    else if (bufferType == xBuffer::xType::SHADER)
    {
        bufferDesc.ByteWidth = (UINT)byteSize;
        bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
        bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    }

    _device->CreateBuffer(&bufferDesc, nullptr, (ID3D11Buffer**)&buffer.BufferResource.Instance);

    if (bufferType == xBuffer::xType::INSTANCE)
    {
        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.Format = DXGI_FORMAT_UNKNOWN;
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
        srvDesc.BufferEx.FirstElement = 0;
        srvDesc.BufferEx.NumElements = BatchManager::k_instanceBufferInstanceCount;

        _device->CreateShaderResourceView((ID3D11Resource*)buffer.BufferResource.Instance, &srvDesc, (ID3D11ShaderResourceView**)&buffer.BufferResource.ViewInstance);
    }

    return buffer;
}

void xpe::core::D3D11RenderingContext::BindBuffer(const xBuffer* buffer)
{
    if (buffer->Type == xBuffer::xType::INSTANCE)
    {
        _immContext->VSSetShaderResources(0, 1, (ID3D11ShaderResourceView**)&buffer->BufferResource.ViewInstance);
    }
    else if (buffer->Type == xBuffer::xType::SHADER)
    {
        _immContext->VSSetConstantBuffers(0, 1, (ID3D11Buffer**)&buffer->BufferResource.ViewInstance);
    }
}

void xpe::core::D3D11RenderingContext::WriteBuffer(const xBuffer& buffer, const void* data, usize dataByteSize)
{
    D3D11_MAPPED_SUBRESOURCE mappedResource = {};

    _immContext->Map((ID3D11Resource*)buffer.BufferResource.Instance, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    memcpy(mappedResource.pData, data, dataByteSize);
    _immContext->Unmap((ID3D11Resource*)buffer.BufferResource.Instance, 0);
}

void xpe::core::D3D11RenderingContext::WriteBufferOffset(const xBuffer& buffer, usize offset, const void* data, usize dataByteSize)
{
}

void xpe::core::D3D11RenderingContext::WriteBufferAppend(xBuffer& buffer, const void* data, usize dataByteSize)
{
    if (buffer.CPUMemory == nullptr)
    {
        return;
    }

    memcpy((void*)((usize)buffer.CPUMemory + (usize)buffer.AppendOffset), data, dataByteSize);
    buffer.AppendOffset += dataByteSize;

    WriteBuffer(buffer, buffer.CPUMemory, buffer.AppendOffset);
}

void xpe::core::D3D11RenderingContext::FreeBuffer(const xBuffer& buffer)
{
    if (buffer.BufferResource.Instance != nullptr)
    {
        ((ID3D11Buffer*)buffer.BufferResource.Instance)->Release();
    }
}

void xpe::core::D3D11RenderingContext::CreateInputLayout(xInputLayout& inputLayout)
{
    usize elementCount = inputLayout.EntryCount;
    D3D11_INPUT_ELEMENT_DESC elements[16] = {};
    for (usize i = 0; i < inputLayout.EntryCount; i++)
    {
        elements[i].SemanticName = inputLayout.Entries[i].Name;
        if (inputLayout.Entries[i].Format == xInputLayout::xEntry::eFormat::VEC3)
        {
            elements[i].Format = DXGI_FORMAT_R32G32B32_FLOAT;
        }
        elements[i].InputSlot = i;
        elements[i].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
    }

    _device->CreateInputLayout(&elements[0], elementCount, inputLayout.VertexShaderByteCode, inputLayout.VertexShaderByteCodeSize, (ID3D11InputLayout**)&inputLayout.InputLayout.Instance);
}

void xpe::core::D3D11RenderingContext::FreeInputLayout(const xInputLayout& inputLayout)
{
    if (inputLayout.InputLayout.Instance != nullptr)
    {
        ((ID3D11InputLayout*)inputLayout.InputLayout.Instance)->Release();
    }
}

void xpe::core::D3D11RenderingContext::BindViewport(const glm::vec4& coords, f32 minDepth, f32 maxDepth)
{
    D3D11_VIEWPORT viewport = {};
    viewport.TopLeftX = coords.x;
    viewport.TopLeftY = coords.y;
    viewport.Width = coords.z;
    viewport.Height = coords.w;
    viewport.MinDepth = minDepth;
    viewport.MaxDepth = maxDepth;

    _immContext->RSSetViewports(1, &viewport);
}

void xpe::core::D3D11RenderingContext::BindRenderPipeline(const xPipeline* pipeline) 
{
    _boundPipeline = (xPipeline*)pipeline;

    UINT stride = 12;
    UINT offset = 0;

    if (_boundPipeline->Shaders->Topology == xShader::PrimitiveTopology::TRIANGLE_STRIP)
    {
        _immContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    }
    else if (_boundPipeline->Shaders->Topology == xShader::PrimitiveTopology::TRIANGLE_LIST)
    {
        _immContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    }
    _immContext->IASetInputLayout((ID3D11InputLayout*)_boundPipeline->InputLayout.InputLayout.Instance);
    _immContext->IASetVertexBuffers(0, 1, (ID3D11Buffer**)&_boundPipeline->InputVertexBuffer->BufferResource.Instance, &stride, &offset);
    _immContext->IASetIndexBuffer((ID3D11Buffer*)_boundPipeline->InputIndexBuffer->BufferResource.Instance, DXGI_FORMAT_R32_UINT, 0);
    BindBuffer(_boundPipeline->InputInstanceBuffer);
    BindBuffer(_boundPipeline->InputShaderBuffer);
    BindShader(_boundPipeline->Shaders);
    BindRenderTarget(_boundPipeline->RenderTarget);
}

void xpe::core::D3D11RenderingContext::DrawBatch(usize vertexOffset, usize indexOffset, usize indexCount, usize instanceCount)
{
    _immContext->DrawIndexedInstanced(indexCount, instanceCount, 0, vertexOffset, indexOffset);
}

void xpe::core::D3D11RenderingContext::DrawQuad()
{
    if (_boundShader->Topology == xShader::PrimitiveTopology::TRIANGLE_STRIP)
    {
        _immContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    }

    _immContext->PSSetSamplers(0, 1, (ID3D11SamplerState**)&_sampler.Instance);
    _immContext->Draw(4, 1);
}

void xpe::core::D3D11RenderingContext::OutputErrors()
{
    ID3D11Debug *d3dDebug = nullptr;
    _device->QueryInterface(__uuidof(ID3D11Debug), (void**)&d3dDebug);

    ID3D11InfoQueue *d3dInfoQueue = nullptr;
    d3dDebug->QueryInterface(__uuidof(ID3D11InfoQueue), (void**)&d3dInfoQueue);

    auto numStoredMessages = d3dInfoQueue->GetNumStoredMessages();
    for (int i = 0; i < numStoredMessages; i++)
    {
        SIZE_T messageByteLength = 0;
        d3dInfoQueue->GetMessageA(i, nullptr, &messageByteLength);

        D3D11_MESSAGE message = {};
        d3dInfoQueue->GetMessageA(i, &message, &messageByteLength);

        std::cout << message.pDescription << std::endl;
    }

    d3dInfoQueue->Release();
    d3dDebug->Release();
}