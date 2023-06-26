#include <d3dcompiler.h>

#include "d3d11_context.hpp"
#include "memory_pool.hpp"
#include "../gltf/gltf.hpp"

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
        _immContext->OMSetRenderTargets(1, (ID3D11RenderTargetView**)&_boundRT->ColorTargetInstance, (ID3D11DepthStencilView*)_boundRT->DepthTargetInstance);
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

void xpe::core::D3D11RenderingContext::CreateShaderFromString(xShader& shader)
{
    if (shader.Type == xShader::eType::VERTEX_PIXEL)
    {
        // Vertex shader
        ID3DBlob* vertexShaderBlob = nullptr;
        ID3DBlob* vertexErrorBlob = nullptr;

        D3DCompile(
            shader.Sources[0],
            strlen(shader.Sources[0]),
            nullptr,
            nullptr,
            nullptr,
            shader.SourceEntryPoints[0],
            shader.SourceProfiles[0],
            (UINT)shader.SourceFlags[0],
            0,
            &vertexShaderBlob,
            &vertexErrorBlob
        );
        

        if (vertexErrorBlob != nullptr)
        {
            std::cout << (char*)vertexErrorBlob->GetBufferPointer() << std::endl;

            vertexErrorBlob->Release();
        }

        if (vertexShaderBlob != nullptr)
        {
            shader._VertexShaderByteCode = vertexShaderBlob->GetBufferPointer();
            shader._VertexShaderByteCodeSize = vertexShaderBlob->GetBufferSize();
            _device->CreateVertexShader(vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), nullptr, (ID3D11VertexShader**)&shader._VertexShader.Instance);
        }

        // Pixel shader
        ID3DBlob* pixelShaderBlob = nullptr;
        ID3DBlob* pixelErrorBlob = nullptr;

        D3DCompile(
            shader.Sources[1],
            strlen(shader.Sources[1]),
            nullptr,
            nullptr,
            nullptr,
            shader.SourceEntryPoints[1],
            shader.SourceProfiles[1],
            (UINT)shader.SourceFlags[1],
            0,
            &pixelShaderBlob,
            &pixelErrorBlob
        );
        

        if (pixelErrorBlob != nullptr)
        {
            std::cout << (char*)pixelErrorBlob->GetBufferPointer() << std::endl;

            pixelErrorBlob->Release();
        }

        if (pixelShaderBlob != nullptr)
        {
            _device->CreatePixelShader(pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize(), nullptr, (ID3D11PixelShader**)&shader._PixelShader.Instance);
            pixelShaderBlob->Release();
        }
    }
}

void xpe::core::D3D11RenderingContext::BindShader(const xShader* shader)
{
    _boundShader = (xShader*)shader;

    if (_boundShader->Type == xShader::eType::VERTEX_PIXEL)
    {
        _immContext->VSSetShader((ID3D11VertexShader*)_boundShader->_VertexShader.Instance, nullptr, 0);
        _immContext->PSSetShader((ID3D11PixelShader*)_boundShader->_PixelShader.Instance, nullptr, 0);
    }
}

void xpe::core::D3D11RenderingContext::FreeShader(const xShader& shader)
{
    if (shader._VertexShader.Instance != nullptr)
    {
        ((ID3D11VertexShader*)shader._VertexShader.Instance)->Release();
    }
    if (shader._VertexShaderByteCode != nullptr)
    {
        ((ID3DBlob*)shader._VertexShaderByteCode)->Release();
    }
    if (shader._PixelShader.Instance != nullptr)
    {
        ((ID3D11PixelShader*)shader._PixelShader.Instance)->Release();
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

void xpe::core::D3D11RenderingContext::BindTexture(const GPUResource* texture, const xShader::eType& shaderType, const u32 slot)
{
    if (shaderType == xShader::eType::VERTEX)
    {
        _immContext->VSSetShaderResources(slot, 1, (ID3D11ShaderResourceView**)&texture->ViewInstance);
    }
    else if (shaderType == xShader::eType::PIXEL)
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
    else if (bufferType == xBuffer::xType::CONSTANT)
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
    if (buffer->Type == xBuffer::xType::VERTEX)
    {
        UINT stride = xpe::gltf::cGLTFModel::k_vertexSize;
        UINT offset = 0;
        _immContext->IASetVertexBuffers(0, 1, (ID3D11Buffer**)&buffer->BufferResource.Instance, &stride, &offset);
    }
    else if (buffer->Type == xBuffer::xType::INDEX)
    {
        DXGI_FORMAT format = xpe::gltf::cGLTFModel::k_indexSize == 4 ? DXGI_FORMAT_R32_UINT : DXGI_FORMAT_R16_UINT;
        _immContext->IASetIndexBuffer((ID3D11Buffer*)buffer->BufferResource.Instance, format, 0);
    }
    else if (buffer->Type == xBuffer::xType::INSTANCE)
    {
        _immContext->VSSetShaderResources(0, 1, (ID3D11ShaderResourceView**)&buffer->BufferResource.ViewInstance);
    }
    else if (buffer->Type == xBuffer::xType::CONSTANT)
    {
        _immContext->VSSetConstantBuffers(0, 1, (ID3D11Buffer**)&buffer->BufferResource.Instance);
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
    usize byteOffset = 0;
    D3D11_INPUT_ELEMENT_DESC elements[16] = {};
    for (usize i = 0; i < inputLayout.EntryCount; i++)
    {
        elements[i].SemanticName = inputLayout.Entries[i].Name;
        if (inputLayout.Entries[i].Format == xInputLayout::xEntry::eFormat::VEC2)
        {
            elements[i].Format = DXGI_FORMAT_R32G32_FLOAT;
        }
        else if (inputLayout.Entries[i].Format == xInputLayout::xEntry::eFormat::VEC3)
        {
            elements[i].Format = DXGI_FORMAT_R32G32B32_FLOAT;
        }
        else if (inputLayout.Entries[i].Format == xInputLayout::xEntry::eFormat::VEC4)
        {
            elements[i].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
        }
        elements[i].AlignedByteOffset = byteOffset;
        elements[i].InstanceDataStepRate = D3D11_INPUT_PER_VERTEX_DATA;

        byteOffset += inputLayout.Entries[i].ByteSize;
    }

    _device->CreateInputLayout(&elements[0], elementCount, inputLayout._VertexShaderByteCode, inputLayout._VertexShaderByteCodeSize, (ID3D11InputLayout**)&inputLayout._InputLayout.Instance);
}

void xpe::core::D3D11RenderingContext::BindInputLayout(const xInputLayout* inputLayout)
{
    UINT stride = inputLayout->StrideByteSize;
    UINT offset = 0;

    if (inputLayout->PrimitiveTopology == ePrimitiveTopology::TRIANGLE_STRIP)
    {
        _immContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    }
    else if (inputLayout->PrimitiveTopology == ePrimitiveTopology::TRIANGLE_LIST)
    {
        _immContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    }
    _immContext->IASetInputLayout((ID3D11InputLayout*)_boundPipeline->InputLayout._InputLayout.Instance);
}

void xpe::core::D3D11RenderingContext::FreeInputLayout(const xInputLayout& inputLayout)
{
    if (inputLayout._InputLayout.Instance != nullptr)
    {
        ((ID3D11InputLayout*)inputLayout._InputLayout.Instance)->Release();
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

void xpe::core::D3D11RenderingContext::CreateRenderPipeline(xPipeline& pipeline)
{
    CreateShaderFromString(*pipeline.Shaders);
    
    pipeline.InputLayout._VertexShaderByteCode = pipeline.Shaders->_VertexShaderByteCode;
    pipeline.InputLayout._VertexShaderByteCodeSize = pipeline.Shaders->_VertexShaderByteCodeSize;
    CreateInputLayout(pipeline.InputLayout);

    CreateDepthStencilState(pipeline.DepthStencilState);
}

void xpe::core::D3D11RenderingContext::BindRenderPipeline(const xPipeline* pipeline) 
{
    _boundPipeline = (xPipeline*)pipeline;

    BindInputLayout(&_boundPipeline->InputLayout);
    BindBuffer(_boundPipeline->VertexBuffer);
    BindBuffer(_boundPipeline->IndexBuffer);
    BindBuffer(_boundPipeline->InstanceBuffer);
    BindBuffer(_boundPipeline->ConstantBuffer);
    BindShader(_boundPipeline->Shaders);
    BindRenderTarget(_boundPipeline->RenderTarget);
    BindDepthStencilState(&_boundPipeline->DepthStencilState);
}

void xpe::core::D3D11RenderingContext::FreeRenderPipeline(xPipeline& pipeline)
{
    if (pipeline.InputLayout._InputLayout.Instance != nullptr)
    {
        ((ID3D11InputLayout*)pipeline.InputLayout._InputLayout.Instance)->Release();
    }
}

void xpe::core::D3D11RenderingContext::CreateDepthStencilState(xDepthStencilState& state)
{
    D3D11_DEPTH_STENCIL_DESC dsDesc = {};
    dsDesc.DepthEnable = state.UseDepthTest == K_TRUE ? TRUE : FALSE;
    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
    dsDesc.StencilEnable = FALSE;

    _device->CreateDepthStencilState(&dsDesc, (ID3D11DepthStencilState**)&state.Instance.Instance);
}

void xpe::core::D3D11RenderingContext::BindDepthStencilState(const xDepthStencilState* state) 
{
    _immContext->OMSetDepthStencilState((ID3D11DepthStencilState*)state->Instance.Instance, 0);
}

void xpe::core::D3D11RenderingContext::FreeDepthStencilState(xDepthStencilState& state)
{
    if (state.Instance.Instance != nullptr)
    {
        ((ID3D11DepthStencilState*)state.Instance.Instance)->Release();
    }
}

void xpe::core::D3D11RenderingContext::DrawBatch(usize vertexOffset, usize indexOffset, usize indexCount, usize instanceCount)
{
    _immContext->DrawIndexedInstanced(indexCount, instanceCount, 0, vertexOffset, indexOffset);
}

void xpe::core::D3D11RenderingContext::DrawQuad()
{
    if (_boundShader->PrimitiveTopology == ePrimitiveTopology::TRIANGLE_STRIP)
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