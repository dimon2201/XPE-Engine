#include <d3dcompiler.h>

#include <rendering/dx11/d3d11_context.hpp>
#include <rendering/dx11/d3d11_debugger.h>

namespace xpe {

    namespace render {

        static const std::unordered_map<Texture::eFormat, DXGI_FORMAT> s_TextureFormatTable =
        {
                { Texture::eFormat::R8, DXGI_FORMAT_R8_UNORM },
                { Texture::eFormat::R16, DXGI_FORMAT_R16_UNORM },
                { Texture::eFormat::R32, DXGI_FORMAT_R32_FLOAT },
                { Texture::eFormat::R32_TYPELESS, DXGI_FORMAT_R32_TYPELESS },

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

        static const std::unordered_map<Texture::eUsage, D3D11_USAGE> s_TextureUsageTable =
        {
                { Texture::eUsage::DEFAULT, D3D11_USAGE_DEFAULT },
                { Texture::eUsage::STATIC, D3D11_USAGE_IMMUTABLE },
                { Texture::eUsage::DYNAMIC, D3D11_USAGE_DYNAMIC },
                { Texture::eUsage::STAGING, D3D11_USAGE_STAGING }
        };

        static const std::unordered_map<TextureSampler::eComparison, D3D11_COMPARISON_FUNC> s_TextureSamplerComparisonTable =
        {
                { TextureSampler::eComparison::ALWAYS, D3D11_COMPARISON_ALWAYS },
                { TextureSampler::eComparison::EQUAL, D3D11_COMPARISON_EQUAL },
                { TextureSampler::eComparison::GREATER, D3D11_COMPARISON_GREATER },
                { TextureSampler::eComparison::GREATER_EQUAL, D3D11_COMPARISON_GREATER_EQUAL },
                { TextureSampler::eComparison::LESS, D3D11_COMPARISON_LESS },
                { TextureSampler::eComparison::LESS_EQUAL, D3D11_COMPARISON_LESS_EQUAL },
                { TextureSampler::eComparison::NEVER, D3D11_COMPARISON_NEVER },
                { TextureSampler::eComparison::NOT_EQUAL, D3D11_COMPARISON_NOT_EQUAL }
        };

        static const std::unordered_map<TextureSampler::eFilter, D3D11_FILTER> s_TextureSamplerFilterTable =
        {
                { TextureSampler::eFilter::MIN_MAG_MIP_POINT, D3D11_FILTER_MIN_MAG_MIP_POINT },
                { TextureSampler::eFilter::MIN_MAG_MIP_LINEAR, D3D11_FILTER_MIN_MAG_MIP_LINEAR },
        };

        static const std::unordered_map<TextureSampler::eAddressMode, D3D11_TEXTURE_ADDRESS_MODE> s_TextureSamplerAddressTable =
        {
                { TextureSampler::eAddressMode::WRAP, D3D11_TEXTURE_ADDRESS_WRAP },
                { TextureSampler::eAddressMode::BORDER, D3D11_TEXTURE_ADDRESS_BORDER },
                { TextureSampler::eAddressMode::CLAMP, D3D11_TEXTURE_ADDRESS_CLAMP },
                { TextureSampler::eAddressMode::MIRROR, D3D11_TEXTURE_ADDRESS_MIRROR },
                { TextureSampler::eAddressMode::MIRROR_ONCE, D3D11_TEXTURE_ADDRESS_MIRROR_ONCE }
        };

        void D3D11Context::Init()
        {
            LogInfo("D3D11Context::Init()");

            m_Device = nullptr;
            m_ImmContext = nullptr;
            m_SwapChain = nullptr;
            m_SwapChainTexture.Instance = nullptr;

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
                    (IDXGISwapChain**)&m_SwapChain,
                    (ID3D11Device**)&m_Device,
                    nullptr,
                    (ID3D11DeviceContext**)&m_ImmContext
            );

            if (result != S_OK) {
                FMT_ASSERT(false, "Failed to create rendering context and device for DX11");
                return;
            }

            InitDebugger(new D3D11Debugger(), this);

            LogDebugMessage();

            m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&m_SwapChainTexture.Instance);
            m_SwapChainTexture.Width = WindowManager::GetWidth();
            m_SwapChainTexture.Height = WindowManager::GetHeight();
            m_SwapChainTexture.Format = Texture::eFormat::RGBA8;

            m_SwapChainTarget.Width = m_SwapChainTexture.Width;
            m_SwapChainTarget.Height = m_SwapChainTexture.Height;
            m_SwapChainTarget.ColorTexture = &m_SwapChainTexture;
            CreateRenderTarget(m_SwapChainTarget);
            LogDebugMessage();

            CreateSampler(m_SwapChainSampler);
            LogDebugMessage();

            LogInfo("D3D11Context initialized");
        }

        void D3D11Context::Free()
        {
            LogInfo("D3D11Context::Free()");

            // todo need to improve concept of RenderTarget
//            FreeRenderTarget(m_SwapChainTarget);
            LogDebugMessage();

            ((ID3D11Texture2D*)m_SwapChainTexture.Instance)->Release();
            LogDebugMessage();

            FreeSampler(&m_SwapChainSampler);
            LogDebugMessage();

            m_ImmContext->Release();
            LogDebugMessage();

            m_Device->Release();
            LogDebugMessage();

            m_SwapChain->Release();
            LogDebugMessage();

            FreeDebugger();
        }

        void D3D11Context::CreateRenderTarget(RenderTarget& renderTarget)
        {
            if (renderTarget.ColorTexture != nullptr && renderTarget.ColorTexture->Instance == nullptr)
            {
                CreateTexture(*renderTarget.ColorTexture);
            }

            if (renderTarget.DepthTexture != nullptr && renderTarget.DepthTexture->Instance == nullptr)
            {
                CreateTextureDepthStencil(*renderTarget.DepthTexture);
            }

            if (renderTarget.ColorTexture != nullptr && renderTarget.ColorTargetView == nullptr)
            {
                D3D11_RENDER_TARGET_VIEW_DESC desc = {};
                desc.Format = s_TextureFormatTable.at(renderTarget.ColorTexture->Format);
                desc.Texture2D.MipSlice = 0;
                desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

                m_Device->CreateRenderTargetView(
                        (ID3D11Resource*)renderTarget.ColorTexture->Instance,
                        &desc,
                        (ID3D11RenderTargetView**)&renderTarget.ColorTargetView
                );
                LogDebugMessage();
            }

            if (renderTarget.DepthTexture != nullptr && renderTarget.DepthTargetView == nullptr)
            {
                D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
                dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
                dsvDesc.Texture2D.MipSlice = 0;
                dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

                m_Device->CreateDepthStencilView(
                        (ID3D11Resource*)renderTarget.DepthTexture->Instance,
                        &dsvDesc,
                        (ID3D11DepthStencilView**)&renderTarget.DepthTargetView
                );
                LogDebugMessage();
            }
        }

        void D3D11Context::BindRenderTarget(void *colorTargetView, void *depthTargetView) {
            m_BoundColorTargetView = colorTargetView;
            m_BoundDepthTargetView = depthTargetView;
            m_ImmContext->OMSetRenderTargets(
                    1,
                    (ID3D11RenderTargetView**)&colorTargetView,
                    (ID3D11DepthStencilView*)depthTargetView
            );
            LogDebugMessage();
        }

        void D3D11Context::BindSwapChainTarget() {
            m_BoundColorTargetView = m_SwapChainTarget.ColorTargetView;
            m_BoundDepthTargetView = m_SwapChainTarget.DepthTargetView;
            m_ImmContext->OMSetRenderTargets(
                    1,
                    (ID3D11RenderTargetView**)&m_BoundColorTargetView,
                    (ID3D11DepthStencilView*)m_BoundDepthTargetView
            );
            LogDebugMessage();
        }

        void D3D11Context::ClearColorTarget(const glm::vec4 &color) {
            m_ImmContext->ClearRenderTargetView((ID3D11RenderTargetView*)m_BoundColorTargetView, &color.x);
            LogDebugMessage();
        }

        void D3D11Context::ClearDepthTarget(const f32 depth) {
            m_ImmContext->ClearDepthStencilView((ID3D11DepthStencilView*)m_BoundDepthTargetView, D3D11_CLEAR_DEPTH, depth, 0);
            LogDebugMessage();
        }

        glm::ivec2 D3D11Context::GetSwapChainDimensions()
        {
            return {m_SwapChainTarget.Width, m_SwapChainTarget.Height };
        }

        void D3D11Context::FreeRenderTarget(const RenderTarget& renderTarget)
        {
            if (renderTarget.ColorTexture != nullptr)
            {
                ((ID3D11Texture2D*)renderTarget.ColorTexture->Instance)->Release();
                LogDebugMessage();
            }

            if (renderTarget.ColorTargetView != nullptr)
            {
                ((ID3D11RenderTargetView*)renderTarget.ColorTargetView)->Release();
                LogDebugMessage();
            }

            if (renderTarget.DepthTexture != nullptr)
            {
                ((ID3D11Texture2D*)renderTarget.DepthTexture->Instance)->Release();
                LogDebugMessage();
            }

            if (renderTarget.DepthTargetView != nullptr)
            {
                ((ID3D11DepthStencilView*)renderTarget.DepthTargetView)->Release();
                LogDebugMessage();
            }
        }

        void D3D11Context::Present()
        {
            m_SwapChain->Present(0, 0);
            LogDebugMessage();
        }

        void D3D11Context::CreateShader(Shader& shader)
        {
            for (auto* stage : shader.Stages) {
                CreateShaderStage(*stage);
            }
        }

        void D3D11Context::BindShader(const Shader* shader)
        {
            m_BoundShader = (Shader*)shader;
            for (const auto* stage : shader->Stages) {
                BindShaderStage(*stage);
            }
        }

        void D3D11Context::FreeShader(Shader& shader)
        {
            for (auto* stage : shader.Stages) {
                FreeShaderStage(*stage);
            }
            shader.Stages.clear();
        }

        void D3D11Context::CreateShaderStage(ShaderStage &stage)
        {
            // skip stage that is already compiled
            // save vertex BLOB into shader
            if (stage.Compiled) {
                return;
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
                        m_Device->CreateVertexShader(
                                shaderBlob->GetBufferPointer(),
                                shaderBlob->GetBufferSize(),
                                nullptr,
                                (ID3D11VertexShader**)&stage.Resource.Instance
                        );
                        LogDebugMessage();
                        break;

                    case eShaderType::PIXEL:
                        m_Device->CreatePixelShader(
                                shaderBlob->GetBufferPointer(),
                                shaderBlob->GetBufferSize(),
                                nullptr,
                                (ID3D11PixelShader**)&stage.Resource.Instance
                        );
                        LogDebugMessage();
                        break;

                    case eShaderType::GEOMETRY:
                        m_Device->CreateGeometryShader(
                                shaderBlob->GetBufferPointer(),
                                shaderBlob->GetBufferSize(),
                                nullptr,
                                (ID3D11GeometryShader**)&stage.Resource.Instance
                        );
                        LogDebugMessage();
                        break;

                    case eShaderType::COMPUTE:
                        m_Device->CreateComputeShader(
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

        void D3D11Context::BindShaderStage(const ShaderStage &stage)
        {
            switch (stage.Type) {

                case eShaderType::VERTEX:
                    m_ImmContext->VSSetShader((ID3D11VertexShader*)stage.Resource.Instance, nullptr, 0);
                    LogDebugMessage();
                    break;

                case eShaderType::PIXEL:
                    m_ImmContext->PSSetShader((ID3D11PixelShader*)stage.Resource.Instance, nullptr, 0);
                    LogDebugMessage();
                    break;

                case eShaderType::GEOMETRY:
                    m_ImmContext->GSSetShader((ID3D11GeometryShader*)stage.Resource.Instance, nullptr, 0);
                    LogDebugMessage();
                    break;

                case eShaderType::COMPUTE:
                    m_ImmContext->CSSetShader((ID3D11ComputeShader*)stage.Resource.Instance, nullptr, 0);
                    LogDebugMessage();
                    break;

            }
        }

        void D3D11Context::FreeShaderStage(ShaderStage &stage)
        {
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

        void D3D11Context::CreateTexture1D(Texture &texture)
        {
            HRESULT status = 0;
            D3D11_TEXTURE1D_DESC texDesc = {};
            D3D11_SHADER_RESOURCE_VIEW_DESC srv = {};
            D3D11_SUBRESOURCE_DATA* initialData = nullptr;
            u32 arraySize = texture.Layers.empty() ? 1 : texture.Layers.size();

            if (texture.InitializeData) {
                initialData = allocT(D3D11_SUBRESOURCE_DATA, arraySize);

                u32 sysMemPitch = texture.Width * TextureManager::BPPTable.at(texture.Format);

                for (int i = 0 ; i < arraySize ; i++) {
                    auto& data = initialData[i];
                    data.pSysMem = texture.Layers[i].Pixels;
                    data.SysMemPitch = sysMemPitch;
                    data.SysMemSlicePitch = 0;
                }
            }

            texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
            if (texture.BindRenderTarget) {
                texDesc.BindFlags |= D3D11_BIND_RENDER_TARGET;
            }

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

            m_Device->CreateTexture1D(&texDesc, initialData, (ID3D11Texture1D**)&texture.Instance);
            LogDebugMessage();

            m_Device->CreateShaderResourceView((ID3D11Resource*)texture.Instance, &srv, (ID3D11ShaderResourceView**)&texture.ViewInstance);
            LogDebugMessage();

            dealloc(initialData);
        }

        void D3D11Context::CreateTexture2D(Texture &texture)
        {
            HRESULT status = 0;
            D3D11_TEXTURE2D_DESC texDesc = {};
            D3D11_SHADER_RESOURCE_VIEW_DESC srv = {};
            D3D11_SUBRESOURCE_DATA* initialData = nullptr;
            u32 arraySize = texture.Layers.empty() ? 1 : texture.Layers.size();

            if (texture.InitializeData) {
                initialData = allocT(D3D11_SUBRESOURCE_DATA, arraySize);

                u32 sysMemPitch = texture.Width * TextureManager::BPPTable.at(texture.Format);

                for (int i = 0 ; i < arraySize ; i++) {
                    auto& data = initialData[i];
                    data.pSysMem = texture.Layers[i].Pixels;
                    data.SysMemPitch = sysMemPitch;
                    data.SysMemSlicePitch = 0;
                }
            }

            texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
            if (texture.BindRenderTarget) {
                texDesc.BindFlags |= D3D11_BIND_RENDER_TARGET;
            }

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

            m_Device->CreateTexture2D(&texDesc, initialData, (ID3D11Texture2D**)&texture.Instance);
            LogDebugMessage();

            m_Device->CreateShaderResourceView((ID3D11Resource*)texture.Instance, &srv, (ID3D11ShaderResourceView**)&texture.ViewInstance);
            LogDebugMessage();

            dealloc(initialData);
        }

        void D3D11Context::CreateTexture2DArray(Texture &texture)
        {
            HRESULT status = 0;
            D3D11_TEXTURE2D_DESC texDesc = {};
            D3D11_SHADER_RESOURCE_VIEW_DESC srv = {};
            D3D11_SUBRESOURCE_DATA* initialData = nullptr;
            u32 arraySize = texture.Layers.empty() ? 1 : texture.Layers.size();

            if (texture.InitializeData) {
                initialData = allocT(D3D11_SUBRESOURCE_DATA, arraySize);

                u32 sysMemPitch = texture.Width * TextureManager::BPPTable.at(texture.Format);

                for (int i = 0 ; i < arraySize ; i++) {
                    auto& data = initialData[i];
                    data.pSysMem = texture.Layers[i].Pixels;
                    data.SysMemPitch = sysMemPitch;
                    data.SysMemSlicePitch = 0;
                }
            }

            texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
            if (texture.BindRenderTarget) {
                texDesc.BindFlags |= D3D11_BIND_RENDER_TARGET;
            }

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

            status = m_Device->CreateTexture2D(&texDesc, initialData, (ID3D11Texture2D**)&texture.Instance);
            LogDebugMessage();

            m_Device->CreateShaderResourceView((ID3D11Resource*)texture.Instance, &srv, (ID3D11ShaderResourceView**)&texture.ViewInstance);
            LogDebugMessage();

            dealloc(initialData);
        }

        void D3D11Context::CreateTexture3D(Texture &texture)
        {
            HRESULT status = 0;
            D3D11_TEXTURE3D_DESC texDesc = {};
            D3D11_SHADER_RESOURCE_VIEW_DESC srv = {};
            D3D11_SUBRESOURCE_DATA* initialData = nullptr;
            u32 arraySize = texture.Layers.empty() ? 1 : texture.Layers.size();

            if (texture.InitializeData) {
                initialData = allocT(D3D11_SUBRESOURCE_DATA, arraySize);

                u32 sysMemPitch = texture.Width * TextureManager::BPPTable.at(texture.Format);

                for (int i = 0 ; i < arraySize ; i++) {
                    auto& data = initialData[i];
                    data.pSysMem = texture.Layers[i].Pixels;
                    data.SysMemPitch = sysMemPitch;
                    data.SysMemSlicePitch = 0; // todo for 3D texture we should set non-zero value here
                }
            }

            texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
            if (texture.BindRenderTarget) {
                texDesc.BindFlags |= D3D11_BIND_RENDER_TARGET;
            }

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

            m_Device->CreateTexture3D(&texDesc, initialData, (ID3D11Texture3D**)&texture.Instance);
            LogDebugMessage();

            m_Device->CreateShaderResourceView((ID3D11Resource*)texture.Instance, &srv, (ID3D11ShaderResourceView**)&texture.ViewInstance);
            LogDebugMessage();

            dealloc(initialData);
        }

        void D3D11Context::CreateTextureCube(Texture &texture)
        {
            D3D11_TEXTURE2D_DESC desc = {};
            D3D11_SUBRESOURCE_DATA initialData[6];
            D3D11_SHADER_RESOURCE_VIEW_DESC srv = {};

            desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
            if (texture.BindRenderTarget) {
                desc.BindFlags |= D3D11_BIND_RENDER_TARGET;
            }

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

            m_Device->CreateTexture2D(&desc, &initialData[0], (ID3D11Texture2D**)&texture.Instance);
            LogDebugMessage();

            m_Device->CreateShaderResourceView((ID3D11Texture2D*)texture.Instance, &srv, (ID3D11ShaderResourceView**)&texture.ViewInstance);
            LogDebugMessage();
        }

        void D3D11Context::CreateTextureDepthStencil(Texture &texture) {
            D3D11_TEXTURE2D_DESC texDesc = {};

            texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
            texDesc.Format = DXGI_FORMAT_R32_TYPELESS;
            texDesc.Width = texture.Width;
            texDesc.Height = texture.Height;
            texDesc.ArraySize = 1;
            texDesc.SampleDesc.Count = 1;
            texDesc.MipLevels = 1;
            texDesc.Usage = s_TextureUsageTable.at(texture.Usage);

            m_Device->CreateTexture2D(&texDesc, nullptr, (ID3D11Texture2D**)&texture.Instance);
        }

        void D3D11Context::BindTexture(const Texture* texture)
        {
            if (texture != nullptr) {
                m_ImmContext->PSSetShaderResources(texture->Slot, 1, (ID3D11ShaderResourceView**)&texture->ViewInstance);
                LogDebugMessage();
            }
        }

        void D3D11Context::BindTextureSlot(u32 slot)
        {
            ID3D11ShaderResourceView* views = nullptr;
            m_ImmContext->PSSetShaderResources(slot, 1, &views);
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

            m_ImmContext->Map((ID3D11Resource*)texture.Instance, index, D3D11_MAP_WRITE, 0, &mappedResource);
            LogDebugMessage();

            if (mappedResource.pData == nullptr) {
                u32 rowPitch = texture.Width * TextureManager::BPPTable.at(texture.Format);
                u32 depthPitch = rowPitch * texture.Height;
                m_ImmContext->UpdateSubresource((ID3D11Resource*)texture.Instance, index, nullptr, pixels, rowPitch, depthPitch);
                LogDebugMessage();
            }

            else {
                memcpy(mappedResource.pData, pixels, pixelsSize);
                m_ImmContext->Unmap((ID3D11Resource*)texture.Instance, index);
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

            m_Device->CreateSamplerState(&desc, (ID3D11SamplerState**)&sampler.Instance);
            LogDebugMessage();
        }

        void D3D11Context::BindSampler(const TextureSampler* sampler)
        {
            m_ImmContext->PSSetSamplers(sampler->Slot, 1, (ID3D11SamplerState**)&sampler->Instance);
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
                buffer.CPUMemory = alloc(byteSize);
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

            m_Device->CreateBuffer(&bufferDesc, nullptr, (ID3D11Buffer**)&buffer.Instance);
            LogDebugMessage();

            if (bufferType == eBufferType::STRUCTURED)
            {
                D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
                srvDesc.Format = DXGI_FORMAT_UNKNOWN;
                srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
                srvDesc.BufferEx.FirstElement = buffer.FirstElement;
                srvDesc.BufferEx.NumElements = buffer.NumElements;

                m_Device->CreateShaderResourceView((ID3D11Resource*)buffer.Instance, &srvDesc, (ID3D11ShaderResourceView**)&buffer.ViewInstance);
                LogDebugMessage();
            }

        }

        void D3D11Context::BindVertexBuffer(const Buffer *buffer)
        {
            UINT stride = buffer->StructureSize;
            UINT offset = 0;
            m_ImmContext->IASetVertexBuffers(buffer->Slot, 1, (ID3D11Buffer**)&buffer->Instance, &stride, &offset);
            LogDebugMessage();
        }

        void D3D11Context::BindIndexBuffer(const Buffer *buffer)
        {
            // we can skip 16-bit index type
            // it's very rare that we will bind index buffer with index range [0, ~65555]
            DXGI_FORMAT format = DXGI_FORMAT_R32_UINT;
            m_ImmContext->IASetIndexBuffer((ID3D11Buffer*)buffer->Instance, format, 0);
            LogDebugMessage();
        }

        void D3D11Context::BindVSBuffer(const Buffer *buffer)
        {
            if (buffer->Type == eBufferType::STRUCTURED)
            {
                m_ImmContext->VSSetShaderResources(buffer->Slot, 1, (ID3D11ShaderResourceView**)&buffer->ViewInstance);
                LogDebugMessage();
            }
            else if (buffer->Type == eBufferType::CONSTANT)
            {
                m_ImmContext->VSSetConstantBuffers(buffer->Slot, 1, (ID3D11Buffer**)&buffer->Instance);
                LogDebugMessage();
            }
        }

        void D3D11Context::BindPSBuffer(const Buffer *buffer)
        {
            if (buffer->Type == eBufferType::STRUCTURED)
            {
                m_ImmContext->PSSetShaderResources(buffer->Slot, 1, (ID3D11ShaderResourceView**)&buffer->ViewInstance);
                LogDebugMessage();
            }
            else if (buffer->Type == eBufferType::CONSTANT)
            {
                m_ImmContext->PSSetConstantBuffers(buffer->Slot, 1, (ID3D11Buffer**)&buffer->Instance);
                LogDebugMessage();
            }
        }

        void D3D11Context::WriteBuffer(const Buffer& buffer, const void* data, usize dataByteSize)
        {
            D3D11_MAPPED_SUBRESOURCE mappedResource = {};

            m_ImmContext->Map((ID3D11Resource*)buffer.Instance, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
            LogDebugMessage();

            memcpy(mappedResource.pData, data, dataByteSize);

            m_ImmContext->Unmap((ID3D11Resource*)buffer.Instance, 0);
            LogDebugMessage();
        }

        void D3D11Context::WriteBufferOffset(const Buffer& buffer, usize offset, const void* data, usize dataByteSize)
        {
            D3D11_MAPPED_SUBRESOURCE mappedResource = {};

            m_ImmContext->Map((ID3D11Resource*)buffer.Instance, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
            LogDebugMessage();

            void* dst = (void*)((u8*)mappedResource.pData + offset);
            memcpy(dst, data, dataByteSize);

            m_ImmContext->Unmap((ID3D11Resource*)buffer.Instance, 0);
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

        static const std::unordered_map<VertexFormat::Attribute::eFormat, DXGI_FORMAT> p_VertexFormatTable =
        {
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
            auto* attributes = allocT(D3D11_INPUT_ELEMENT_DESC, attributeCount);

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

            m_Device->CreateInputLayout(
                    attributes, attributeCount,
                    inputLayout.VertexBlob->ByteCode, inputLayout.VertexBlob->ByteCodeSize,
                    (ID3D11InputLayout**)&inputLayout.Layout.Instance
            );
            LogDebugMessage();

            dealloc(attributes);
        }

        static const std::unordered_map<ePrimitiveTopology, D3D11_PRIMITIVE_TOPOLOGY> s_PrimitiveTopologyTable =
        {
                { ePrimitiveTopology::TRIANGLE_STRIP, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP },
                { ePrimitiveTopology::TRIANGLE_LIST, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST },
                { ePrimitiveTopology::POINT_LIST, D3D11_PRIMITIVE_TOPOLOGY_POINTLIST },
                { ePrimitiveTopology::LINE_LIST, D3D11_PRIMITIVE_TOPOLOGY_LINELIST },
                { ePrimitiveTopology::LINE_STRIP, D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP }
        };

        void D3D11Context::BindInputLayout(const InputLayout* inputLayout)
        {
            m_ImmContext->IASetPrimitiveTopology(s_PrimitiveTopologyTable.at(inputLayout->PrimitiveTopology));
            LogDebugMessage();

            m_ImmContext->IASetInputLayout((ID3D11InputLayout*)m_BoundPipeline->InputLayout.Layout.Instance);
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

            m_ImmContext->RSSetViewports(1, &viewport);
            LogDebugMessage();
        }

        void D3D11Context::CreateRenderPipeline(Pipeline& pipeline)
        {
            if (pipeline.Shader == nullptr) {
                LogError("Failed to create render pipeline. Shader does not exist.");
                assert(false);
                return;
            }

            Blob* vertexBlob = nullptr;
            for (auto* stage : pipeline.Shader->Stages) {
                if (stage->Type == eShaderType::VERTEX) {
                    vertexBlob = &stage->Blob;
                }
            }

            if (vertexBlob == nullptr) {
                LogError("Failed to create render pipeline. Shader has no Vertex stage.");
                assert(false);
                return;
            }

            pipeline.InputLayout.VertexBlob = vertexBlob;
            CreateInputLayout(pipeline.InputLayout);

            CreateDepthStencilState(pipeline.DepthStencilState);
        }

        void D3D11Context::BindRenderPipeline(const Pipeline* pipeline)
        {
            m_BoundPipeline = (Pipeline*) pipeline;
            Pipeline& boundPipeline = *m_BoundPipeline;

            BindInputLayout(&boundPipeline.InputLayout);

            for (const auto* buffer : boundPipeline.VSBuffers) {
                BindVSBuffer(buffer);
            }

            for (const auto* buffer : boundPipeline.PSBuffers) {
                BindPSBuffer(buffer);
            }

            for (const auto* texture : boundPipeline.Textures) {
                BindTexture(texture);
            }

            for (const auto* sampler : boundPipeline.Samplers) {
                BindSampler(sampler);
            }

            BindShader(boundPipeline.Shader);

            BindRenderTarget(boundPipeline.RenderTarget->ColorTargetView, boundPipeline.RenderTarget->DepthTargetView);

            BindDepthStencilState(&m_BoundPipeline->DepthStencilState);
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

            m_Device->CreateDepthStencilState(&dsDesc, (ID3D11DepthStencilState**)&state.Instance.Instance);
            LogDebugMessage();
        }

        void D3D11Context::BindDepthStencilState(const DepthStencilState* state)
        {
            m_ImmContext->OMSetDepthStencilState((ID3D11DepthStencilState*)state->Instance.Instance, 0);
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
            m_ImmContext->DrawIndexedInstanced(indexCount, instanceCount, 0, vertexOffset, indexOffset);
            LogDebugMessage();
        }

        void D3D11Context::DrawBatch(usize vertexOffset, usize vertexCount, usize instanceCount) {
            m_ImmContext->DrawInstanced(vertexCount, instanceCount, 0, vertexOffset);
            LogDebugMessage();
        }

        void D3D11Context::DrawQuad()
        {
            m_ImmContext->IASetPrimitiveTopology(s_PrimitiveTopologyTable.at(m_BoundShader->PrimitiveTopology));
            LogDebugMessage();

            m_ImmContext->PSSetSamplers(0, 1, (ID3D11SamplerState**)&m_SwapChainSampler.Instance);
            LogDebugMessage();

            m_ImmContext->Draw(4, 1);
            LogDebugMessage();
        }

        void* D3D11Context::GetDevice()
        {
            return m_Device;
        }

    }

}
