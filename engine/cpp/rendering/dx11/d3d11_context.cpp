#include <d3dcompiler.h>

#include <rendering/dx11/d3d11_context.hpp>

#include <gltf/gltf.hpp>

namespace xpe {

    namespace render {

        void D3D11Context::Init(Window& window)
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
            CreateTexture(_swapChainTexture, _swapChainTexture.Instance, glm::ivec2(window.GetWidth(), window.GetHeight()));
            _rt = CreateRenderTarget(glm::ivec2(window.GetWidth(), window.GetHeight()), &_swapChainTexture, nullptr, nullptr, nullptr);
            CreateSampler(_sampler);
        }

        void D3D11Context::Free()
        {
            FreeRenderTarget(_rt);
            ((ID3D11Texture2D*)_swapChainTexture.Instance)->Release();
            _immContext->Release();
            _device->Release();
            _swapChain->Release();
        }

        RenderTarget D3D11Context::CreateRenderTarget(const glm::ivec2& dimensions, const GPUResource* colorTexture, const GPUResource* colorView, const GPUResource* depthTexture, const GPUResource* depthView)
        {
            RenderTarget rt = {};

            rt.ColorTexture = colorTexture == nullptr ? new Texture() : (Texture*)colorTexture;
            rt.DepthTexture = depthTexture == nullptr ? new Texture() : (Texture*)depthTexture;
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

        void D3D11Context::BindRenderTarget(const RenderTarget* renderTarget)
        {
            _boundRT = (RenderTarget*)renderTarget;

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

        void D3D11Context::ClearRenderTarget(const glm::vec4& color, const f32 depth)
        {
            if (_boundRT == nullptr) { return; }

            _immContext->ClearRenderTargetView((ID3D11RenderTargetView*)_boundRT->ColorTargetInstance, &color.x);
            _immContext->ClearDepthStencilView((ID3D11DepthStencilView*)_boundRT->DepthTargetInstance, D3D11_CLEAR_DEPTH, depth, 0);
        }

        glm::ivec2 D3D11Context::GetSwapChainDimensions()
        {
            return _rt.Dimensions;
        }

        void D3D11Context::FreeRenderTarget(const RenderTarget& renderTarget)
        {
            if (renderTarget.ColorTexture != nullptr)
            {
                ((ID3D11Texture2D*)renderTarget.ColorTexture->Instance)->Release();
            }
            if (renderTarget.ColorTargetInstance != nullptr)
            {
                ((ID3D11RenderTargetView*)renderTarget.ColorTargetInstance)->Release();
            }
            if (renderTarget.DepthTexture != nullptr)
            {
                ((ID3D11Texture2D*)renderTarget.DepthTexture->Instance)->Release();
            }
            if (renderTarget.DepthTargetInstance != nullptr)
            {
                ((ID3D11DepthStencilView*)renderTarget.DepthTargetInstance)->Release();
            }
        }

        void D3D11Context::Present()
        {
            _swapChain->Present(0, 0);
        }

        void D3D11Context::CreateShader(Shader& shader)
        {

            for (auto& stage : shader.Stages) {

                ID3DBlob* shaderBlob = nullptr;
                ID3DBlob* errorBlob = nullptr;

                D3DCompile(
                        stage.Source.c_str(),
                        stage.Source.length(),
                        nullptr,
                        nullptr,
                        nullptr,
                        stage.EntryPoint,
                        stage.Profile,
                        (UINT)stage.Flag,
                        0,
                        &shaderBlob,
                        &errorBlob
                );

                if (errorBlob != nullptr) {
                    LogError((char*)errorBlob->GetBufferPointer());
                    errorBlob->Release();
                }

                if (shaderBlob != nullptr) {
                    stage.Blob.Instance = shaderBlob;
                    stage.Blob.ByteCode = shaderBlob->GetBufferPointer();
                    stage.Blob.ByteCodeSize = shaderBlob->GetBufferSize();

                    switch (stage.Type) {

                        case eShaderType::VERTEX:
                            _device->CreateVertexShader(
                                    shaderBlob->GetBufferPointer(),
                                    shaderBlob->GetBufferSize(),
                                    nullptr,
                                    (ID3D11VertexShader**)&stage.Resource.Instance
                            );
                            shader.VertexBlob = &stage.Blob;
                            break;

                        case eShaderType::PIXEL:
                            _device->CreatePixelShader(
                                    shaderBlob->GetBufferPointer(),
                                    shaderBlob->GetBufferSize(),
                                    nullptr,
                                    (ID3D11PixelShader**)&stage.Resource.Instance
                            );
                            break;

                        case eShaderType::GEOMETRY:
                            _device->CreateGeometryShader(
                                    shaderBlob->GetBufferPointer(),
                                    shaderBlob->GetBufferSize(),
                                    nullptr,
                                    (ID3D11GeometryShader**)&stage.Resource.Instance
                            );
                            break;

                        case eShaderType::COMPUTE:
                            _device->CreateComputeShader(
                                    shaderBlob->GetBufferPointer(),
                                    shaderBlob->GetBufferSize(),
                                    nullptr,
                                    (ID3D11ComputeShader**)&stage.Resource.Instance
                            );
                            break;

                    }
                }

            }
        }

        void D3D11Context::BindShader(const Shader* shader)
        {
            _boundShader = (Shader*)shader;

            for (const auto& stage : shader->Stages) {

                switch (stage.Type) {

                    case eShaderType::VERTEX:
                        _immContext->VSSetShader((ID3D11VertexShader*)stage.Resource.Instance, nullptr, 0);
                        break;

                    case eShaderType::PIXEL:
                        _immContext->PSSetShader((ID3D11PixelShader*)stage.Resource.Instance, nullptr, 0);
                        break;

                    case eShaderType::GEOMETRY:
                        _immContext->GSSetShader((ID3D11GeometryShader*)stage.Resource.Instance, nullptr, 0);
                        break;

                    case eShaderType::COMPUTE:
                        _immContext->CSSetShader((ID3D11ComputeShader*)stage.Resource.Instance, nullptr, 0);
                        break;

                }

            }

        }

        void D3D11Context::FreeShader(Shader& shader)
        {
            for (const auto& stage : shader.Stages) {

                if (stage.Blob.Instance != nullptr) {
                    ((ID3DBlob*)stage.Blob.Instance)->Release();
                }

                if (stage.Resource.Instance != nullptr) {

                    switch (stage.Type) {

                        case eShaderType::VERTEX:
                            ((ID3D11VertexShader*)stage.Resource.Instance)->Release();
                            break;

                        case eShaderType::PIXEL:
                            ((ID3D11PixelShader*)stage.Resource.Instance)->Release();
                            break;

                        case eShaderType::GEOMETRY:
                            ((ID3D11GeometryShader*)stage.Resource.Instance)->Release();
                            break;

                        case eShaderType::COMPUTE:
                            ((ID3D11ComputeShader*)stage.Resource.Instance)->Release();
                            break;

                    }

                }

            }

            shader.Stages.clear();
        }

        void D3D11Context::BindTexture(const Texture* texture, const eShaderType& shaderType, const u32 slot)
        {
            if (shaderType == eShaderType::VERTEX)
            {
                _immContext->VSSetShaderResources(slot, 1, (ID3D11ShaderResourceView**)&texture->ViewInstance);
            }
            else if (shaderType == eShaderType::PIXEL)
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

        void D3D11Context::CreateTexture(Texture& texture, const void* instance, const glm::ivec2& dimension)
        {

            if (instance == nullptr) {

                D3D11_TEXTURE1D_DESC texture1DDesc = {};
                D3D11_TEXTURE2D_DESC texture2DDesc = {};
                D3D11_TEXTURE3D_DESC texture3DDesc = {};

                switch (texture.Type) {

                    case eTextureType::TEXTURE_1D:
                        texture1DDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
                        texture1DDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
                        texture1DDesc.Width = dimension.x;
                        texture1DDesc.ArraySize = 1;
                        texture1DDesc.MipLevels = 1;
                        texture1DDesc.Usage = D3D11_USAGE_DEFAULT;

                        _device->CreateTexture1D(&texture1DDesc, nullptr, (ID3D11Texture1D**)&texture.Instance);
                        break;

                    case eTextureType::TEXTURE_2D:
                        texture2DDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
                        texture2DDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
                        texture2DDesc.Width = dimension.x;
                        texture2DDesc.Height = dimension.y;
                        texture2DDesc.ArraySize = 1;
                        texture2DDesc.SampleDesc.Count = 1;
                        texture2DDesc.MipLevels = 1;
                        texture2DDesc.Usage = D3D11_USAGE_DEFAULT;

                        _device->CreateTexture2D(&texture2DDesc, nullptr, (ID3D11Texture2D**)&texture.Instance);
                        break;

                    case eTextureType::TEXTURE_3D:
                        texture3DDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
                        texture3DDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
                        texture3DDesc.Width = dimension.x;
                        texture3DDesc.Height = dimension.y;
                        texture3DDesc.MipLevels = 1;
                        texture3DDesc.Usage = D3D11_USAGE_DEFAULT;

                        _device->CreateTexture3D(&texture3DDesc, nullptr, (ID3D11Texture3D**)&texture.Instance);
                        break;

                }

            }

            else {
                texture.Instance = (void*)instance;
            }

        }

        void D3D11Context::FreeTexture(const Texture* texture)
        {
            if (texture->Instance != nullptr)
            {

                switch (texture->Type) {

                    case eTextureType::TEXTURE_1D:
                        ((ID3D11Texture1D*)texture->Instance)->Release();
                        break;

                    case eTextureType::TEXTURE_2D:
                        ((ID3D11Texture2D*)texture->Instance)->Release();
                        break;

                    case eTextureType::TEXTURE_3D:
                        ((ID3D11Texture3D*)texture->Instance)->Release();
                        break;

                }

            }
        }

        void D3D11Context::CreateSampler(TextureSampler& sampler)
        {
            D3D11_SAMPLER_DESC desc = {};
            desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
            desc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
            desc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
            desc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
            desc.BorderColor[0] = sampler.BorderColor[0];
            desc.BorderColor[1] = sampler.BorderColor[1];
            desc.BorderColor[2] = sampler.BorderColor[2];
            desc.BorderColor[3] = sampler.BorderColor[3];
            desc.ComparisonFunc = D3D11_COMPARISON_NEVER;

            _device->CreateSamplerState(&desc, (ID3D11SamplerState**)&sampler.Instance);
        }

        void D3D11Context::BindSampler(const TextureSampler* sampler)
        {
            _immContext->PSSetSamplers(0, 1, (ID3D11SamplerState**)&sampler->Instance);
        }

        void D3D11Context::FreeSampler(const TextureSampler* sampler)
        {
            if (sampler->Instance != nullptr)
            {
                ((ID3D11SamplerState*)sampler->Instance)->Release();
            }
        }

        void D3D11Context::CreateBuffer(Buffer& buffer, const eBufferType& bufferType, usize byteSize, boolean duplicate)
        {
            buffer.Type = bufferType;
            buffer.AppendOffset = 0;

            if (duplicate == K_TRUE)
            {
                buffer.CPUMemory = MemoryPoolManager::Allocate(byteSize);
            }
            else
            {
                buffer.CPUMemory = nullptr;
            }

            D3D11_BUFFER_DESC bufferDesc = {};

            if (bufferType == eBufferType::VERTEX)
            {
                bufferDesc.ByteWidth = (UINT)byteSize;
                bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
                bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
                bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
            }

            else if (bufferType == eBufferType::INDEX)
            {
                bufferDesc.ByteWidth = (UINT)byteSize;
                bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
                bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
                bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
            }

            else if (bufferType == eBufferType::INSTANCE)
            {
                bufferDesc.ByteWidth = (UINT)byteSize;
                bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
                bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
                bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
                bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
                bufferDesc.StructureByteStride = sizeof(RenderInstance);
            }

            else if (bufferType == eBufferType::CONSTANT)
            {
                bufferDesc.ByteWidth = (UINT)byteSize;
                bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
                bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
                bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
            }

            _device->CreateBuffer(&bufferDesc, nullptr, (ID3D11Buffer**)&buffer.Resource.Instance);

            if (bufferType == eBufferType::INSTANCE)
            {
                D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
                srvDesc.Format = DXGI_FORMAT_UNKNOWN;
                srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
                srvDesc.BufferEx.FirstElement = 0;
                srvDesc.BufferEx.NumElements = BatchManager::k_instanceBufferInstanceCount;

                _device->CreateShaderResourceView((ID3D11Resource*)buffer.Resource.Instance, &srvDesc, (ID3D11ShaderResourceView**)&buffer.Resource.ViewInstance);
            }

        }

        void D3D11Context::BindBuffer(const Buffer* buffer)
        {
            if (buffer->Type == eBufferType::VERTEX)
            {
                UINT stride = xpe::gltf::cGLTFModel::k_vertexSize;
                UINT offset = 0;
                _immContext->IASetVertexBuffers(0, 1, (ID3D11Buffer**)&buffer->Resource.Instance, &stride, &offset);
            }
            else if (buffer->Type == eBufferType::INDEX)
            {
                DXGI_FORMAT format = xpe::gltf::cGLTFModel::k_indexSize == 4 ? DXGI_FORMAT_R32_UINT : DXGI_FORMAT_R16_UINT;
                _immContext->IASetIndexBuffer((ID3D11Buffer*)buffer->Resource.Instance, format, 0);
            }
            else if (buffer->Type == eBufferType::INSTANCE)
            {
                _immContext->VSSetShaderResources(0, 1, (ID3D11ShaderResourceView**)&buffer->Resource.ViewInstance);
            }
            else if (buffer->Type == eBufferType::CONSTANT)
            {
                _immContext->VSSetConstantBuffers(0, 1, (ID3D11Buffer**)&buffer->Resource.Instance);
            }
        }

        void D3D11Context::WriteBuffer(const Buffer& buffer, const void* data, usize dataByteSize)
        {
            D3D11_MAPPED_SUBRESOURCE mappedResource = {};

            _immContext->Map((ID3D11Resource*)buffer.Resource.Instance, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
            memcpy(mappedResource.pData, data, dataByteSize);
            _immContext->Unmap((ID3D11Resource*)buffer.Resource.Instance, 0);
        }

        void D3D11Context::WriteBufferOffset(const Buffer& buffer, usize offset, const void* data, usize dataByteSize)
        {
        }

        void D3D11Context::WriteBufferAppend(Buffer& buffer, const void* data, usize dataByteSize)
        {
            if (buffer.CPUMemory == nullptr)
            {
                return;
            }

            memcpy((void*)((usize)buffer.CPUMemory + (usize)buffer.AppendOffset), data, dataByteSize);
            buffer.AppendOffset += dataByteSize;

            WriteBuffer(buffer, buffer.CPUMemory, buffer.AppendOffset);
        }

        void D3D11Context::FreeBuffer(const Buffer& buffer)
        {
            if (buffer.Resource.Instance != nullptr)
            {
                ((ID3D11Buffer*)buffer.Resource.Instance)->Release();
            }
        }

        void D3D11Context::CreateInputLayout(InputLayout& inputLayout)
        {
            usize elementCount = inputLayout.EntryCount;
            usize byteOffset = 0;
            D3D11_INPUT_ELEMENT_DESC elements[16] = {};
            for (usize i = 0; i < inputLayout.EntryCount; i++)
            {
                elements[i].SemanticName = inputLayout.Entries[i].Name;
                if (inputLayout.Entries[i].Format == InputLayout::Entry::eFormat::VEC2)
                {
                    elements[i].Format = DXGI_FORMAT_R32G32_FLOAT;
                }
                else if (inputLayout.Entries[i].Format == InputLayout::Entry::eFormat::VEC3)
                {
                    elements[i].Format = DXGI_FORMAT_R32G32B32_FLOAT;
                }
                else if (inputLayout.Entries[i].Format == InputLayout::Entry::eFormat::VEC4)
                {
                    elements[i].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
                }
                elements[i].AlignedByteOffset = byteOffset;
                elements[i].InstanceDataStepRate = D3D11_INPUT_PER_VERTEX_DATA;

                byteOffset += inputLayout.Entries[i].ByteSize;
            }

            _device->CreateInputLayout(&elements[0], elementCount, inputLayout.VertexBlob->ByteCode, inputLayout.VertexBlob->ByteCodeSize, (ID3D11InputLayout**)&inputLayout.InputLayout.Instance);
        }

        void D3D11Context::BindInputLayout(const InputLayout* inputLayout)
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
            _immContext->IASetInputLayout((ID3D11InputLayout*)_boundPipeline->InputLayout.InputLayout.Instance);
        }

        void D3D11Context::FreeInputLayout(const InputLayout& inputLayout)
        {
            if (inputLayout.InputLayout.Instance != nullptr)
            {
                ((ID3D11InputLayout*)inputLayout.InputLayout.Instance)->Release();
            }
        }

        void D3D11Context::BindViewport(const glm::vec4& coords, f32 minDepth, f32 maxDepth)
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

        void D3D11Context::CreateRenderPipeline(Pipeline& pipeline)
        {
            if (pipeline.Shader == nullptr) {
                LogError("Failed to create render pipeline. Shader does not exist.");
                assert(false);
                return;
            }

            if (pipeline.Shader->VertexBlob == nullptr) {
                LogError("Failed to create render pipeline. Shader has no Vertex stage.");
                assert(false);
                return;
            }

            pipeline.InputLayout.VertexBlob = pipeline.Shader->VertexBlob;
            CreateInputLayout(pipeline.InputLayout);

            CreateDepthStencilState(pipeline.DepthStencilState);
        }

        void D3D11Context::BindRenderPipeline(const Pipeline* pipeline)
        {
            _boundPipeline = (Pipeline*)pipeline;

            BindInputLayout(&_boundPipeline->InputLayout);
            BindBuffer(_boundPipeline->VertexBuffer);
            BindBuffer(_boundPipeline->IndexBuffer);
            BindBuffer(_boundPipeline->InstanceBuffer);
            for (const auto* buffer : _boundPipeline->ConstantBuffers) {
                BindBuffer(buffer);
            }
            BindShader(_boundPipeline->Shader);
            BindRenderTarget(_boundPipeline->RenderTarget);
            BindDepthStencilState(&_boundPipeline->DepthStencilState);
        }

        void D3D11Context::FreeRenderPipeline(Pipeline& pipeline)
        {
            if (pipeline.InputLayout.InputLayout.Instance != nullptr)
            {
                ((ID3D11InputLayout*)pipeline.InputLayout.InputLayout.Instance)->Release();
            }
        }

        void D3D11Context::CreateDepthStencilState(DepthStencilState& state)
        {
            D3D11_DEPTH_STENCIL_DESC dsDesc = {};
            dsDesc.DepthEnable = state.UseDepthTest == K_TRUE ? TRUE : FALSE;
            dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
            dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
            dsDesc.StencilEnable = FALSE;

            _device->CreateDepthStencilState(&dsDesc, (ID3D11DepthStencilState**)&state.Instance.Instance);
        }

        void D3D11Context::BindDepthStencilState(const DepthStencilState* state)
        {
            _immContext->OMSetDepthStencilState((ID3D11DepthStencilState*)state->Instance.Instance, 0);
        }

        void D3D11Context::FreeDepthStencilState(DepthStencilState& state)
        {
            if (state.Instance.Instance != nullptr)
            {
                ((ID3D11DepthStencilState*)state.Instance.Instance)->Release();
            }
        }

        void D3D11Context::DrawBatch(usize vertexOffset, usize indexOffset, usize indexCount, usize instanceCount)
        {
            _immContext->DrawIndexedInstanced(indexCount, instanceCount, 0, vertexOffset, indexOffset);
        }

        void D3D11Context::DrawQuad()
        {
            if (_boundShader->PrimitiveTopology == ePrimitiveTopology::TRIANGLE_STRIP)
            {
                _immContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
            }

            _immContext->PSSetSamplers(0, 1, (ID3D11SamplerState**)&_sampler.Instance);
            _immContext->Draw(4, 1);
        }

        void* D3D11Context::GetDevice()
        {
            return _device;
        }

    }

}
