#include <d3dcompiler.h>

#include <rendering/dx11/d3d11_context.hpp>
#include <rendering/dx11/d3d11_debugger.h>

namespace xpe {

    namespace render {

        void D3D11Context::Init()
        {
            _device = nullptr;
            _immContext = nullptr;
            _swapChain = nullptr;
            _swapChainTexture.Instance = nullptr;

            DXGI_MODE_DESC bufferDesc = {};
            bufferDesc.Width = WindowManager::GetWidth();
            bufferDesc.Height = WindowManager::GetHeight();
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
            swapChainDesc.OutputWindow = (HWND)WindowManager::GetWin32Instance();
            swapChainDesc.Windowed = TRUE;
            swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

            HRESULT result = D3D11CreateDeviceAndSwapChain(
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

            if (result != S_OK) {
                FMT_ASSERT(false, "Failed to create rendering context and device for DX11");
                return;
            }

            InitDebugger(new D3D11Debugger(), this);

            LogDebugMessage();

            _swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&_swapChainTexture.Instance);
            _swapChainTexture.Width = WindowManager::GetWidth();
            _swapChainTexture.Height = WindowManager::GetHeight();
            _swapChainTexture.Format = Texture::eFormat::RGBA8;

            _rt = CreateRenderTarget(glm::ivec2(_swapChainTexture.Width, _swapChainTexture.Height), &_swapChainTexture, nullptr, nullptr, nullptr);
            LogDebugMessage();

            CreateSampler(_sampler);
            LogDebugMessage();
        }

        void D3D11Context::Free()
        {
            // todo need to improve concept of RenderTarget
//            FreeRenderTarget(_rt);
            LogDebugMessage();

            ((ID3D11Texture2D*)_swapChainTexture.Instance)->Release();
            LogDebugMessage();

            FreeSampler(&_sampler);
            LogDebugMessage();

            _immContext->Release();
            LogDebugMessage();

            _device->Release();
            LogDebugMessage();

            _swapChain->Release();
            LogDebugMessage();

            FreeDebugger();
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
                LogDebugMessage();

                D3D11_SHADER_RESOURCE_VIEW_DESC colorSRV = {};
                colorSRV.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
                colorSRV.Texture2D.MipLevels = 1;
                colorSRV.Texture2D.MostDetailedMip = 0;
                colorSRV.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;

                _device->CreateShaderResourceView((ID3D11Resource*)rt.ColorTexture->Instance, &colorSRV, (ID3D11ShaderResourceView**)&rt.ColorTexture->ViewInstance);
                LogDebugMessage();
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
                LogDebugMessage();
            }

            if (colorView == nullptr)
            {
                D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
                rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
                rtvDesc.Texture2D.MipSlice = 0;
                rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

                _device->CreateRenderTargetView((ID3D11Resource*)rt.ColorTexture->Instance, &rtvDesc, (ID3D11RenderTargetView**)&rt.ColorTargetInstance);
                LogDebugMessage();
            }

            if (depthView == nullptr)
            {
                D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
                dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
                dsvDesc.Texture2D.MipSlice = 0;
                dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

                _device->CreateDepthStencilView((ID3D11Resource*)rt.DepthTexture->Instance, &dsvDesc, (ID3D11DepthStencilView**)&rt.DepthTargetInstance);
                LogDebugMessage();
            }

            return rt;
        }

        void D3D11Context::BindRenderTarget(const RenderTarget* renderTarget)
        {
            _boundRT = (RenderTarget*)renderTarget;

            if (_boundRT != nullptr)
            {
                _immContext->OMSetRenderTargets(1, (ID3D11RenderTargetView**)&_boundRT->ColorTargetInstance, (ID3D11DepthStencilView*)_boundRT->DepthTargetInstance);
                LogDebugMessage();
            }
            else
            {
                _boundRT = &_rt;
                _immContext->OMSetRenderTargets(1, (ID3D11RenderTargetView**)&_boundRT->ColorTargetInstance, nullptr);
                LogDebugMessage();
            }
        }

        void D3D11Context::ClearRenderTarget(const glm::vec4& color, const f32 depth)
        {
            if (_boundRT == nullptr) { return; }

            _immContext->ClearRenderTargetView((ID3D11RenderTargetView*)_boundRT->ColorTargetInstance, &color.x);
            LogDebugMessage();

            _immContext->ClearDepthStencilView((ID3D11DepthStencilView*)_boundRT->DepthTargetInstance, D3D11_CLEAR_DEPTH, depth, 0);
            LogDebugMessage();
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
                LogDebugMessage();
            }

            if (renderTarget.ColorTargetInstance != nullptr)
            {
                ((ID3D11RenderTargetView*)renderTarget.ColorTargetInstance)->Release();
                LogDebugMessage();
            }

            if (renderTarget.DepthTexture != nullptr)
            {
                ((ID3D11Texture2D*)renderTarget.DepthTexture->Instance)->Release();
                LogDebugMessage();
            }

            if (renderTarget.DepthTargetInstance != nullptr)
            {
                ((ID3D11DepthStencilView*)renderTarget.DepthTargetInstance)->Release();
                LogDebugMessage();
            }
        }

        void D3D11Context::Present()
        {
            _swapChain->Present(0, 0);
            LogDebugMessage();
        }

        void D3D11Context::CreateShader(Shader& shader)
        {

            for (auto& stage : shader.Stages) {

                // skip stage that is already compiled
                // save vertex BLOB into shader
                if (stage.Compiled) {
                    if (stage.Type == eShaderType::VERTEX) {
                        shader.VertexBlob = &stage.Blob;
                    }
                    continue;
                }

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
                LogDebugMessage();

                if (errorBlob != nullptr) {
                    LogError((char*)errorBlob->GetBufferPointer());
                    errorBlob->Release();
                    LogDebugMessage();
                }

                if (shaderBlob != nullptr) {

                    stage.Compiled = true;
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
                            LogDebugMessage();
                            shader.VertexBlob = &stage.Blob;
                            break;

                        case eShaderType::PIXEL:
                            _device->CreatePixelShader(
                                    shaderBlob->GetBufferPointer(),
                                    shaderBlob->GetBufferSize(),
                                    nullptr,
                                    (ID3D11PixelShader**)&stage.Resource.Instance
                            );
                            LogDebugMessage();
                            break;

                        case eShaderType::GEOMETRY:
                            _device->CreateGeometryShader(
                                    shaderBlob->GetBufferPointer(),
                                    shaderBlob->GetBufferSize(),
                                    nullptr,
                                    (ID3D11GeometryShader**)&stage.Resource.Instance
                            );
                            LogDebugMessage();
                            break;

                        case eShaderType::COMPUTE:
                            _device->CreateComputeShader(
                                    shaderBlob->GetBufferPointer(),
                                    shaderBlob->GetBufferSize(),
                                    nullptr,
                                    (ID3D11ComputeShader**)&stage.Resource.Instance
                            );
                            LogDebugMessage();
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
                        LogDebugMessage();
                        break;

                    case eShaderType::PIXEL:
                        _immContext->PSSetShader((ID3D11PixelShader*)stage.Resource.Instance, nullptr, 0);
                        LogDebugMessage();
                        break;

                    case eShaderType::GEOMETRY:
                        _immContext->GSSetShader((ID3D11GeometryShader*)stage.Resource.Instance, nullptr, 0);
                        LogDebugMessage();
                        break;

                    case eShaderType::COMPUTE:
                        _immContext->CSSetShader((ID3D11ComputeShader*)stage.Resource.Instance, nullptr, 0);
                        LogDebugMessage();
                        break;

                }

            }

        }

        void D3D11Context::FreeShader(Shader& shader)
        {
            for (const auto& stage : shader.Stages) {

                if (stage.Blob.Instance != nullptr) {
                    ((ID3DBlob*)stage.Blob.Instance)->Release();
                    LogDebugMessage();
                }

                if (stage.Resource.Instance != nullptr) {

                    switch (stage.Type) {

                        case eShaderType::VERTEX:
                            ((ID3D11VertexShader*)stage.Resource.Instance)->Release();
                            LogDebugMessage();
                            break;

                        case eShaderType::PIXEL:
                            ((ID3D11PixelShader*)stage.Resource.Instance)->Release();
                            LogDebugMessage();
                            break;

                        case eShaderType::GEOMETRY:
                            ((ID3D11GeometryShader*)stage.Resource.Instance)->Release();
                            LogDebugMessage();
                            break;

                        case eShaderType::COMPUTE:
                            ((ID3D11ComputeShader*)stage.Resource.Instance)->Release();
                            LogDebugMessage();
                            break;

                    }

                }

            }

            shader.Stages.clear();
        }

        static const unordered_map<Texture::eFormat, DXGI_FORMAT> s_TextureFormatTable = {

                { Texture::eFormat::R8, DXGI_FORMAT_R8_UNORM },
                { Texture::eFormat::R16, DXGI_FORMAT_R16_UNORM },
                { Texture::eFormat::R32, DXGI_FORMAT_R32_FLOAT },

                { Texture::eFormat::RG8, DXGI_FORMAT_R8G8_UNORM },
                { Texture::eFormat::RG16, DXGI_FORMAT_R16G16_UNORM },
                { Texture::eFormat::RG32, DXGI_FORMAT_R32G32_FLOAT },

                { Texture::eFormat::RGB8, DXGI_FORMAT_R8G8B8A8_UNORM },
                { Texture::eFormat::RGB16, DXGI_FORMAT_R16G16B16A16_UNORM },
                { Texture::eFormat::RGB32, DXGI_FORMAT_R32G32B32_FLOAT },

                { Texture::eFormat::RGBA8, DXGI_FORMAT_R8G8B8A8_UNORM },
                { Texture::eFormat::RGBA16, DXGI_FORMAT_R16G16B16A16_UNORM },
                { Texture::eFormat::RGBA32, DXGI_FORMAT_R32G32B32A32_FLOAT },

        };

        static const unordered_map<Texture::eUsage, D3D11_USAGE> s_TextureUsageTable = {

            { Texture::eUsage::DEFAULT, D3D11_USAGE_DEFAULT },
            { Texture::eUsage::STATIC, D3D11_USAGE_IMMUTABLE },
            { Texture::eUsage::DYNAMIC, D3D11_USAGE_DYNAMIC },
            { Texture::eUsage::STAGING, D3D11_USAGE_STAGING }

        };

        static const unordered_map<TextureSampler::eComparison, D3D11_COMPARISON_FUNC> s_TextureSamplerComparisonTable = {
            { TextureSampler::eComparison::ALWAYS, D3D11_COMPARISON_ALWAYS },
            { TextureSampler::eComparison::EQUAL, D3D11_COMPARISON_EQUAL },
            { TextureSampler::eComparison::GREATER, D3D11_COMPARISON_GREATER },
            { TextureSampler::eComparison::GREATER_EQUAL, D3D11_COMPARISON_GREATER_EQUAL },
            { TextureSampler::eComparison::LESS, D3D11_COMPARISON_LESS },
            { TextureSampler::eComparison::LESS_EQUAL, D3D11_COMPARISON_LESS_EQUAL },
            { TextureSampler::eComparison::NEVER, D3D11_COMPARISON_NEVER },
            { TextureSampler::eComparison::NOT_EQUAL, D3D11_COMPARISON_NOT_EQUAL }
        };

        static const unordered_map<TextureSampler::eFilter, D3D11_FILTER> s_TextureSamplerFilterTable = {
            { TextureSampler::eFilter::MIN_MAG_MIP_POINT, D3D11_FILTER_MIN_MAG_MIP_POINT },
            { TextureSampler::eFilter::MIN_MAG_MIP_LINEAR, D3D11_FILTER_MIN_MAG_MIP_LINEAR },
        };

        static const unordered_map<TextureSampler::eAddressMode, D3D11_TEXTURE_ADDRESS_MODE> s_TextureSamplerAddressTable = {
            { TextureSampler::eAddressMode::WRAP, D3D11_TEXTURE_ADDRESS_WRAP },
            { TextureSampler::eAddressMode::BORDER, D3D11_TEXTURE_ADDRESS_BORDER },
            { TextureSampler::eAddressMode::CLAMP, D3D11_TEXTURE_ADDRESS_CLAMP },
            { TextureSampler::eAddressMode::MIRROR, D3D11_TEXTURE_ADDRESS_MIRROR },
            { TextureSampler::eAddressMode::MIRROR_ONCE, D3D11_TEXTURE_ADDRESS_MIRROR_ONCE }
        };

        void D3D11Context::CreateTexture(Texture& texture, const void* instance)
        {

            if (instance == nullptr) {

                switch (texture.Type) {

                    case Texture::eType::TEXTURE_1D:
                        CreateTexture1D(texture);
                        break;

                    case Texture::eType::TEXTURE_2D:
                        CreateTexture2D(texture);
                        break;

                    case Texture::eType::TEXTURE_2D_ARRAY:
                        CreateTexture2DArray(texture);
                        break;

                    case Texture::eType::TEXTURE_3D:
                        CreateTexture3D(texture);
                        break;

                    case Texture::eType::TEXTURE_CUBE:
                        CreateTextureCube(texture);
                        break;

                }

            }

            else {
                texture.Instance = (void*) instance;
            }

        }

        void D3D11Context::CreateTexture1D(Texture &texture) {
            HRESULT status = 0;
            D3D11_TEXTURE1D_DESC texDesc = {};
            D3D11_SHADER_RESOURCE_VIEW_DESC srv = {};
            D3D11_SUBRESOURCE_DATA* initialData = nullptr;
            u32 arraySize = texture.Layers.empty() ? 1 : texture.Layers.size();

            if (texture.InitializeData) {
                initialData = (D3D11_SUBRESOURCE_DATA*) MemoryPoolManager::Allocate(sizeof(D3D11_SUBRESOURCE_DATA) * arraySize);

                u32 sysMemPitch = texture.Width * TextureManager::BPPTable.at(texture.Format);

                for (int i = 0 ; i < arraySize ; i++) {
                    auto& data = initialData[i];
                    data.pSysMem = texture.Layers[i].Pixels;
                    data.SysMemPitch = sysMemPitch;
                    data.SysMemSlicePitch = 0;
                }
            }

            texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
            texDesc.Usage = s_TextureUsageTable.at(texture.Usage);
            texDesc.Format = s_TextureFormatTable.at(texture.Format);
            texDesc.Width = texture.Width;
            texDesc.ArraySize = arraySize;
            texDesc.MipLevels = texture.MipLevels;
            texDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

            srv.Format = s_TextureFormatTable.at(texture.Format);
            srv.Texture1D.MipLevels = texture.MipLevels;
            srv.Texture1D.MostDetailedMip = texture.MostDetailedMip;
            srv.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1D;

            _device->CreateTexture1D(&texDesc, initialData, (ID3D11Texture1D**)&texture.Instance);
            LogDebugMessage();

            _device->CreateShaderResourceView((ID3D11Resource*)texture.Instance, &srv, (ID3D11ShaderResourceView**)&texture.ViewInstance);
            LogDebugMessage();

            MemoryPoolManager::Free(initialData);
        }

        void D3D11Context::CreateTexture2D(Texture &texture) {
            HRESULT status = 0;
            D3D11_TEXTURE2D_DESC texDesc = {};
            D3D11_SHADER_RESOURCE_VIEW_DESC srv = {};
            D3D11_SUBRESOURCE_DATA* initialData = nullptr;
            u32 arraySize = texture.Layers.empty() ? 1 : texture.Layers.size();

            if (texture.InitializeData) {
                initialData = (D3D11_SUBRESOURCE_DATA*) MemoryPoolManager::Allocate(sizeof(D3D11_SUBRESOURCE_DATA) * arraySize);

                u32 sysMemPitch = texture.Width * TextureManager::BPPTable.at(texture.Format);

                for (int i = 0 ; i < arraySize ; i++) {
                    auto& data = initialData[i];
                    data.pSysMem = texture.Layers[i].Pixels;
                    data.SysMemPitch = sysMemPitch;
                    data.SysMemSlicePitch = 0;
                }
            }

            texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
            texDesc.Usage = s_TextureUsageTable.at(texture.Usage);
            texDesc.Format = s_TextureFormatTable.at(texture.Format);
            texDesc.Width = texture.Width;
            texDesc.Height = texture.Height;
            texDesc.ArraySize = arraySize;
            texDesc.SampleDesc.Count = 1;
            texDesc.SampleDesc.Quality = 0;
            texDesc.MipLevels = texture.MipLevels;
            texDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

            srv.Format = s_TextureFormatTable.at(texture.Format);
            srv.Texture2D.MipLevels = texture.MipLevels;
            srv.Texture2D.MostDetailedMip = texture.MostDetailedMip;
            srv.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;

            _device->CreateTexture2D(&texDesc, initialData, (ID3D11Texture2D**)&texture.Instance);
            LogDebugMessage();

            _device->CreateShaderResourceView((ID3D11Resource*)texture.Instance, &srv, (ID3D11ShaderResourceView**)&texture.ViewInstance);
            LogDebugMessage();

            MemoryPoolManager::Free(initialData);
        }

        void D3D11Context::CreateTexture2DArray(Texture &texture) {
            HRESULT status = 0;
            D3D11_TEXTURE2D_DESC texDesc = {};
            D3D11_SHADER_RESOURCE_VIEW_DESC srv = {};
            D3D11_SUBRESOURCE_DATA* initialData = nullptr;
            u32 arraySize = texture.Layers.empty() ? 1 : texture.Layers.size();

            if (texture.InitializeData) {
                initialData = (D3D11_SUBRESOURCE_DATA*) MemoryPoolManager::Allocate(sizeof(D3D11_SUBRESOURCE_DATA) * arraySize);

                u32 sysMemPitch = texture.Width * TextureManager::BPPTable.at(texture.Format);

                for (int i = 0 ; i < arraySize ; i++) {
                    auto& data = initialData[i];
                    data.pSysMem = texture.Layers[i].Pixels;
                    data.SysMemPitch = sysMemPitch;
                    data.SysMemSlicePitch = 0;
                }
            }

            texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
            texDesc.Usage = s_TextureUsageTable.at(texture.Usage);
            texDesc.Format = s_TextureFormatTable.at(texture.Format);
            texDesc.Width = texture.Width;
            texDesc.Height = texture.Height;
            texDesc.ArraySize = arraySize;
            texDesc.SampleDesc.Count = 1;
            texDesc.SampleDesc.Quality = 0;
            texDesc.MipLevels = texture.MipLevels;
            texDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
            texDesc.MiscFlags = 0;

            srv.Format = s_TextureFormatTable.at(texture.Format);
            srv.Texture2DArray.MipLevels = texture.MipLevels;
            srv.Texture2DArray.MostDetailedMip = texture.MostDetailedMip;
            srv.Texture2DArray.FirstArraySlice = 0;
            srv.Texture2DArray.ArraySize = arraySize;
            srv.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;

            status = _device->CreateTexture2D(&texDesc, initialData, (ID3D11Texture2D**)&texture.Instance);
            LogDebugMessage();

            _device->CreateShaderResourceView((ID3D11Resource*)texture.Instance, &srv, (ID3D11ShaderResourceView**)&texture.ViewInstance);
            LogDebugMessage();

            MemoryPoolManager::Free(initialData);
        }

        void D3D11Context::CreateTexture3D(Texture &texture) {
            HRESULT status = 0;
            D3D11_TEXTURE3D_DESC texDesc = {};
            D3D11_SHADER_RESOURCE_VIEW_DESC srv = {};
            D3D11_SUBRESOURCE_DATA* initialData = nullptr;
            u32 arraySize = texture.Layers.empty() ? 1 : texture.Layers.size();

            if (texture.InitializeData) {
                initialData = (D3D11_SUBRESOURCE_DATA*) MemoryPoolManager::Allocate(sizeof(D3D11_SUBRESOURCE_DATA) * arraySize);

                u32 sysMemPitch = texture.Width * TextureManager::BPPTable.at(texture.Format);

                for (int i = 0 ; i < arraySize ; i++) {
                    auto& data = initialData[i];
                    data.pSysMem = texture.Layers[i].Pixels;
                    data.SysMemPitch = sysMemPitch;
                    data.SysMemSlicePitch = 0; // todo for 3D texture we should set non-zero value here
                }
            }

            texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
            texDesc.Usage = s_TextureUsageTable.at(texture.Usage);
            texDesc.Format = s_TextureFormatTable.at(texture.Format);
            texDesc.Width = texture.Width;
            texDesc.Height = texture.Height;
            texDesc.MipLevels = texture.MipLevels;
            texDesc.Depth = texture.Depth;
            texDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

            srv.Format = s_TextureFormatTable.at(texture.Format);
            srv.Texture3D.MipLevels = texture.MipLevels;
            srv.Texture3D.MostDetailedMip = texture.MostDetailedMip;
            srv.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;

            _device->CreateTexture3D(&texDesc, initialData, (ID3D11Texture3D**)&texture.Instance);
            LogDebugMessage();

            _device->CreateShaderResourceView((ID3D11Resource*)texture.Instance, &srv, (ID3D11ShaderResourceView**)&texture.ViewInstance);
            LogDebugMessage();

            MemoryPoolManager::Free(initialData);
        }

        void D3D11Context::CreateTextureCube(Texture &texture) {
            D3D11_TEXTURE2D_DESC desc = {};
            D3D11_SUBRESOURCE_DATA initialData[6];
            D3D11_SHADER_RESOURCE_VIEW_DESC srv = {};

            desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
            desc.Usage = s_TextureUsageTable.at(texture.Usage);
            desc.Format = s_TextureFormatTable.at(texture.Format);
            desc.Width = texture.Width;
            desc.Height = texture.Height;
            desc.MipLevels = texture.MipLevels;
            desc.ArraySize = 6;
            desc.SampleDesc.Count = 1;
            desc.SampleDesc.Quality = 0;
            desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
            desc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

            srv.Format = s_TextureFormatTable.at(texture.Format);
            srv.TextureCube.MipLevels = texture.MipLevels;
            srv.TextureCube.MostDetailedMip = texture.MostDetailedMip;
            srv.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;

            u32 sysMemPitch = texture.Width * TextureManager::BPPTable.at(texture.Format);

            for (int i = 0; i < 6; i++)
            {
                auto& data = initialData[i];
                data.pSysMem = texture.Layers[i].Pixels;
                data.SysMemPitch = sysMemPitch;
                data.SysMemSlicePitch = 0;
            }

            _device->CreateTexture2D(&desc, &initialData[0], (ID3D11Texture2D**)&texture.Instance);
            LogDebugMessage();

            _device->CreateShaderResourceView((ID3D11Texture2D*)texture.Instance, &srv, (ID3D11ShaderResourceView**)&texture.ViewInstance);
            LogDebugMessage();
        }

        void D3D11Context::BindTexture(const Texture* texture)
        {
            if (texture != nullptr) {
                _immContext->PSSetShaderResources(texture->Slot, 1, (ID3D11ShaderResourceView**)&texture->ViewInstance);
                LogDebugMessage();
            }
        }

        void D3D11Context::BindTextureSlot(u32 slot) {
            ID3D11ShaderResourceView* views = nullptr;
            _immContext->PSSetShaderResources(slot, 1, &views);
            LogDebugMessage();
        }

        void D3D11Context::FreeTexture(const Texture* texture)
        {
            if (texture->Instance != nullptr)
            {

                switch (texture->Type) {

                    case Texture::eType::TEXTURE_1D:
                        ((ID3D11ShaderResourceView*)texture->ViewInstance)->Release();
                        LogDebugMessage();
                        ((ID3D11Texture1D*)texture->Instance)->Release();
                        LogDebugMessage();
                        break;

                    case Texture::eType::TEXTURE_2D:
                        ((ID3D11ShaderResourceView*)texture->ViewInstance)->Release();
                        LogDebugMessage();
                        ((ID3D11Texture2D*)texture->Instance)->Release();
                        LogDebugMessage();
                        break;

                    case Texture::eType::TEXTURE_3D:
                        ((ID3D11ShaderResourceView*)texture->ViewInstance)->Release();
                        LogDebugMessage();
                        ((ID3D11Texture3D*)texture->Instance)->Release();
                        LogDebugMessage();
                        break;

                    case Texture::eType::TEXTURE_CUBE:
                        ((ID3D11ShaderResourceView*)texture->ViewInstance)->Release();
                        LogDebugMessage();
                        ((ID3D11Texture2D*)texture->Instance)->Release();
                        LogDebugMessage();
                        break;

                }

            }
        }

        void D3D11Context::WriteTexture(const Texture &texture, const void* pixels, usize pixelsSize, u32 index)
        {
            if (texture.Usage == Texture::eUsage::STATIC) {
                LogError("Unable to write into DX11 texture, because Usage = STATIC");
                return;
            }

            D3D11_MAPPED_SUBRESOURCE mappedResource = {};

            _immContext->Map((ID3D11Resource*)texture.Instance, index, D3D11_MAP_WRITE, 0, &mappedResource);
            LogDebugMessage();

            if (mappedResource.pData == nullptr) {
                u32 rowPitch = texture.Width * TextureManager::BPPTable.at(texture.Format);
                u32 depthPitch = rowPitch * texture.Height;
                _immContext->UpdateSubresource((ID3D11Resource*)texture.Instance, index, nullptr, pixels, rowPitch, depthPitch);
                LogDebugMessage();
            }

            else {
                memcpy(mappedResource.pData, pixels, pixelsSize);
                _immContext->Unmap((ID3D11Resource*)texture.Instance, index);
                LogDebugMessage();
            }

        }

        void D3D11Context::CreateSampler(TextureSampler& sampler)
        {
            D3D11_SAMPLER_DESC desc = {};
            desc.Filter = s_TextureSamplerFilterTable.at(sampler.Filter);
            desc.AddressU = s_TextureSamplerAddressTable.at(sampler.AddressU);
            desc.AddressV = s_TextureSamplerAddressTable.at(sampler.AddressV);
            desc.AddressW = s_TextureSamplerAddressTable.at(sampler.AddressW);
            desc.ComparisonFunc = s_TextureSamplerComparisonTable.at(sampler.Comparison);
            desc.MaxAnisotropy = sampler.MaxAnisotropy;
            desc.MinLOD = sampler.MinLOD;
            desc.MaxLOD = sampler.MaxLOD;
            desc.MipLODBias = sampler.MipLODBias;
            desc.BorderColor[0] = sampler.BorderColor[0];
            desc.BorderColor[1] = sampler.BorderColor[1];
            desc.BorderColor[2] = sampler.BorderColor[2];
            desc.BorderColor[3] = sampler.BorderColor[3];

            _device->CreateSamplerState(&desc, (ID3D11SamplerState**)&sampler.Instance);
            LogDebugMessage();
        }

        void D3D11Context::BindSampler(const TextureSampler* sampler)
        {
            _immContext->PSSetSamplers(sampler->Slot, 1, (ID3D11SamplerState**)&sampler->Instance);
            LogDebugMessage();
        }

        void D3D11Context::FreeSampler(const TextureSampler* sampler)
        {
            if (sampler->Instance != nullptr)
            {
                ((ID3D11SamplerState*)sampler->Instance)->Release();
                LogDebugMessage();
            }
        }

        void D3D11Context::CreateBuffer(Buffer& buffer)
        {
            eBufferType bufferType = buffer.Type;
            usize byteSize = buffer.ByteSize;
            buffer.AppendOffset = 0;

            if (buffer.Duplicate == K_TRUE)
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

            else if (bufferType == eBufferType::STRUCTURED)
            {
                bufferDesc.ByteWidth = (UINT)byteSize;
                bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
                bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
                bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
                bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
                bufferDesc.StructureByteStride = buffer.StructureSize;
            }

            else if (bufferType == eBufferType::CONSTANT)
            {
                bufferDesc.ByteWidth = (UINT)byteSize;
                bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
                bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
                bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
            }

            _device->CreateBuffer(&bufferDesc, nullptr, (ID3D11Buffer**)&buffer.Instance);
            LogDebugMessage();

            if (bufferType == eBufferType::STRUCTURED)
            {
                D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
                srvDesc.Format = DXGI_FORMAT_UNKNOWN;
                srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
                srvDesc.BufferEx.FirstElement = buffer.FirstElement;
                srvDesc.BufferEx.NumElements = buffer.NumElements;

                _device->CreateShaderResourceView((ID3D11Resource*)buffer.Instance, &srvDesc, (ID3D11ShaderResourceView**)&buffer.ViewInstance);
                LogDebugMessage();
            }

        }

        void D3D11Context::BindVertexBuffer(const Buffer *buffer) {
            UINT stride = buffer->StructureSize;
            UINT offset = 0;
            _immContext->IASetVertexBuffers(buffer->Slot, 1, (ID3D11Buffer**)&buffer->Instance, &stride, &offset);
            LogDebugMessage();
        }

        void D3D11Context::BindIndexBuffer(const Buffer *buffer) {
            // we can skip 16-bit index type
            // it's very rare that we will bind index buffer with index range [0, ~65555]
            DXGI_FORMAT format = DXGI_FORMAT_R32_UINT;
            _immContext->IASetIndexBuffer((ID3D11Buffer*)buffer->Instance, format, 0);
            LogDebugMessage();
        }

        void D3D11Context::BindVSBuffer(const Buffer *buffer) {
            if (buffer->Type == eBufferType::STRUCTURED)
            {
                _immContext->VSSetShaderResources(buffer->Slot, 1, (ID3D11ShaderResourceView**)&buffer->ViewInstance);
                LogDebugMessage();
            }
            else if (buffer->Type == eBufferType::CONSTANT)
            {
                _immContext->VSSetConstantBuffers(buffer->Slot, 1, (ID3D11Buffer**)&buffer->Instance);
                LogDebugMessage();
            }
        }

        void D3D11Context::BindPSBuffer(const Buffer *buffer) {
            if (buffer->Type == eBufferType::STRUCTURED)
            {
                _immContext->PSSetShaderResources(buffer->Slot, 1, (ID3D11ShaderResourceView**)&buffer->ViewInstance);
                LogDebugMessage();
            }
            else if (buffer->Type == eBufferType::CONSTANT)
            {
                _immContext->PSSetConstantBuffers(buffer->Slot, 1, (ID3D11Buffer**)&buffer->Instance);
                LogDebugMessage();
            }
        }

        void D3D11Context::WriteBuffer(const Buffer& buffer, const void* data, usize dataByteSize)
        {
            D3D11_MAPPED_SUBRESOURCE mappedResource = {};

            _immContext->Map((ID3D11Resource*)buffer.Instance, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
            LogDebugMessage();

            memcpy(mappedResource.pData, data, dataByteSize);

            _immContext->Unmap((ID3D11Resource*)buffer.Instance, 0);
            LogDebugMessage();
        }

        void D3D11Context::WriteBufferOffset(const Buffer& buffer, usize offset, const void* data, usize dataByteSize)
        {
            D3D11_MAPPED_SUBRESOURCE mappedResource = {};

            _immContext->Map((ID3D11Resource*)buffer.Instance, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
            LogDebugMessage();

            void* dst = (void*)((u8*)mappedResource.pData + offset);
            memcpy(dst, data, dataByteSize);

            _immContext->Unmap((ID3D11Resource*)buffer.Instance, 0);
            LogDebugMessage();
        }

        void D3D11Context::WriteBufferAppend(Buffer& buffer, const void* data, usize dataByteSize)
        {
            // todo do we really need CPU memory for buffer? check structure, vertex, index buffers implementations
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
            if (buffer.Instance != nullptr)
            {
                ((ID3D11Buffer*)buffer.Instance)->Release();
                LogDebugMessage();
            }
        }

        static const unordered_map<VertexFormat::Attribute::eFormat, DXGI_FORMAT> p_VertexFormatTable = {
            { VertexFormat::Attribute::eFormat::BOOL, DXGI_FORMAT_R32_UINT },
            { VertexFormat::Attribute::eFormat::INT, DXGI_FORMAT_R32_SINT },
            { VertexFormat::Attribute::eFormat::FLOAT, DXGI_FORMAT_R32_FLOAT },
            { VertexFormat::Attribute::eFormat::VEC2, DXGI_FORMAT_R32G32_FLOAT },
            { VertexFormat::Attribute::eFormat::VEC3, DXGI_FORMAT_R32G32B32_FLOAT },
            { VertexFormat::Attribute::eFormat::VEC4, DXGI_FORMAT_R32G32B32A32_FLOAT },
        };

        void D3D11Context::CreateInputLayout(InputLayout& inputLayout)
        {
            VertexFormat& vertexFormat = inputLayout.Format;
            usize attributeCount = vertexFormat.Attributes.size();
            usize attributeOffset = 0;
            auto* attributes = (D3D11_INPUT_ELEMENT_DESC*) MemoryPoolManager::Allocate(attributeCount * sizeof(D3D11_INPUT_ELEMENT_DESC));

            for (u32 i = 0 ; i < attributeCount ; i++)
            {
                auto& attribute = vertexFormat.Attributes[i];
                auto& dxAttribute = attributes[i];
                dxAttribute = {};

                dxAttribute.SemanticName = attribute.Name;
                dxAttribute.Format = p_VertexFormatTable.at(attribute.Format);
                dxAttribute.AlignedByteOffset = attributeOffset;
                dxAttribute.InstanceDataStepRate = D3D11_INPUT_PER_VERTEX_DATA;

                attributeOffset += (u32) attribute.Format;
            }

            _device->CreateInputLayout(
                    attributes, attributeCount,
                    inputLayout.VertexBlob->ByteCode, inputLayout.VertexBlob->ByteCodeSize,
                    (ID3D11InputLayout**)&inputLayout.Layout.Instance
            );
            LogDebugMessage();

            MemoryPoolManager::Free(attributes);
        }

        void D3D11Context::BindInputLayout(const InputLayout* inputLayout)
        {
            if (inputLayout->PrimitiveTopology == ePrimitiveTopology::TRIANGLE_STRIP)
            {
                _immContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
                LogDebugMessage();
            }

            else if (inputLayout->PrimitiveTopology == ePrimitiveTopology::TRIANGLE_LIST)
            {
                _immContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
                LogDebugMessage();
            }

            _immContext->IASetInputLayout((ID3D11InputLayout*)_boundPipeline->InputLayout.Layout.Instance);
            LogDebugMessage();
        }

        void D3D11Context::FreeInputLayout(const InputLayout& inputLayout)
        {
            if (inputLayout.Layout.Instance != nullptr)
            {
                ((ID3D11InputLayout*)inputLayout.Layout.Instance)->Release();
                LogDebugMessage();
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
            LogDebugMessage();
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

            for (const auto* buffer : _boundPipeline->VSBuffers) {
                BindVSBuffer(buffer);
            }

            for (const auto* buffer : _boundPipeline->PSBuffers) {
                BindPSBuffer(buffer);
            }

            BindShader(_boundPipeline->Shader);

            BindRenderTarget(_boundPipeline->RenderTarget);

            BindDepthStencilState(&_boundPipeline->DepthStencilState);
        }

        void D3D11Context::FreeRenderPipeline(Pipeline& pipeline)
        {
            if (pipeline.InputLayout.Layout.Instance != nullptr)
            {
                ((ID3D11InputLayout*)pipeline.InputLayout.Layout.Instance)->Release();
                LogDebugMessage();
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
            LogDebugMessage();
        }

        void D3D11Context::BindDepthStencilState(const DepthStencilState* state)
        {
            _immContext->OMSetDepthStencilState((ID3D11DepthStencilState*)state->Instance.Instance, 0);
            LogDebugMessage();
        }

        void D3D11Context::FreeDepthStencilState(DepthStencilState& state)
        {
            if (state.Instance.Instance != nullptr)
            {
                ((ID3D11DepthStencilState*)state.Instance.Instance)->Release();
                LogDebugMessage();
            }
        }

        void D3D11Context::DrawBatch(usize vertexOffset, usize indexOffset, usize indexCount, usize instanceCount)
        {
            _immContext->DrawIndexedInstanced(indexCount, instanceCount, 0, vertexOffset, indexOffset);
            LogDebugMessage();
        }

        void D3D11Context::DrawBatch(usize vertexOffset, usize vertexCount, usize instanceCount) {
            _immContext->DrawInstanced(vertexCount, instanceCount, 0, vertexOffset);
            LogDebugMessage();
        }

        void D3D11Context::DrawQuad()
        {
            if (_boundShader->PrimitiveTopology == ePrimitiveTopology::TRIANGLE_STRIP)
            {
                _immContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
                LogDebugMessage();
            }

            _immContext->PSSetSamplers(0, 1, (ID3D11SamplerState**)&_sampler.Instance);
            LogDebugMessage();

            _immContext->Draw(4, 1);
            LogDebugMessage();
        }

        void* D3D11Context::GetDevice()
        {
            return _device;
        }

    }

}
