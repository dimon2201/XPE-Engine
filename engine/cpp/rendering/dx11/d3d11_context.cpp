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
                { TextureSampler::eFilter::ANISOTROPIC, D3D11_FILTER_ANISOTROPIC },
        };

        static const std::unordered_map<TextureSampler::eAddress, D3D11_TEXTURE_ADDRESS_MODE> s_TextureSamplerAddressTable =
        {
                { TextureSampler::eAddress::WRAP,        D3D11_TEXTURE_ADDRESS_WRAP },
                { TextureSampler::eAddress::BORDER,      D3D11_TEXTURE_ADDRESS_BORDER },
                { TextureSampler::eAddress::CLAMP,       D3D11_TEXTURE_ADDRESS_CLAMP },
                { TextureSampler::eAddress::MIRROR,      D3D11_TEXTURE_ADDRESS_MIRROR },
                { TextureSampler::eAddress::MIRROR_ONCE, D3D11_TEXTURE_ADDRESS_MIRROR_ONCE }
        };

        void D3D11Context::Init()
        {
            LogInfo("D3D11Context::Init()");

            HRESULT result = D3D11CreateDevice(
                    nullptr,
                    D3D_DRIVER_TYPE_HARDWARE,
                    0,
                    D3D11_CREATE_DEVICE_DEBUG,
                    nullptr,
                    0,
                    D3D11_SDK_VERSION,
                    (ID3D11Device**)&m_Device,
                    nullptr,
                    (ID3D11DeviceContext**)&m_ImmContext
            );

            if (result != S_OK) {
                FMT_ASSERT(false, "Failed to create rendering context and device for DX11");
                return;
            }

            CreateHardwareConfig();

            InitDebugger(new D3D11Debugger(), this);

            m_Device->QueryInterface(__uuidof(IDXGIDevice), (void **)&m_GIDevice);
            LogDebugMessage();

            m_GIDevice->GetAdapter(&m_GIAdapter);
            LogDebugMessage();

            m_GIAdapter->GetParent(__uuidof(IDXGIFactory), (void **)&m_GIFactory);
            LogDebugMessage();

            CreateSwapchain(WindowManager::GetWidth(), WindowManager::GetHeight());

            LogInfo("D3D11Context initialized");
        }

        void D3D11Context::Free()
        {
            LogInfo("D3D11Context::Free()");

            m_ImmContext->Release();
            LogDebugMessage();

            m_Device->Release();
            LogDebugMessage();

            m_GIDevice->Release();
            LogDebugMessage();

            m_GIAdapter->Release();
            LogDebugMessage();

            m_GIFactory->Release();
            LogDebugMessage();

            FreeSwapchain();

            FreeDebugger();
        }

        void D3D11Context::CreateSwapchain(int width, int height)
        {
            DXGI_MODE_DESC bufferDesc = {};
            bufferDesc.Width = width;
            bufferDesc.Height = height;
            bufferDesc.RefreshRate.Numerator = WindowManager::GetRefreshRate();
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
            swapChainDesc.Windowed = WindowManager::IsWindowed();
            swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

            m_GIFactory->CreateSwapChain(m_Device, &swapChainDesc, &m_SwapChain);
            LogDebugMessage();

            CreateSwapchainTargetView();
        }

        void D3D11Context::FreeSwapchain()
        {
            m_SwapChain->Release();
            LogDebugMessage();
        }

        void D3D11Context::ResizeSwapchain(RenderTarget& renderTarget, int width, int height)
        {
            m_ImmContext->OMSetRenderTargets(0, 0, 0);

            ((ID3D11RenderTargetView*) renderTarget.ColorTargetView)->Release();

            m_SwapChain->ResizeBuffers(1, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, 0);
            LogDebugMessage();

            CreateSwapchainTargetView();
            renderTarget.ColorTargetView = m_SwapchainTargetView;

            m_ImmContext->OMSetRenderTargets(
                    1,
                    (ID3D11RenderTargetView**) &renderTarget.ColorTargetView,
                    NULL
            );
            LogDebugMessage();

            m_Viewport->Width = width;
            m_Viewport->Height = height;
            BindViewport(m_Viewport);
        }

        void D3D11Context::CreateSwapchainTargetView()
        {
            ID3D11Texture2D* swapchainTexture = nullptr;
            m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**) &swapchainTexture);
            LogDebugMessage();

            m_Device->CreateRenderTargetView(
                    (ID3D11Texture2D*) swapchainTexture,
                    NULL,
                    (ID3D11RenderTargetView**) &m_SwapchainTargetView
            );
            LogDebugMessage();

            ((ID3D11Texture2D*) swapchainTexture)->Release();
            LogDebugMessage();
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
                desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
                desc.Texture2D.MipSlice = 0;

                m_Device->CreateRenderTargetView(
                        (ID3D11Resource*)renderTarget.ColorTexture->Instance,
                        &desc,
                        (ID3D11RenderTargetView**)&renderTarget.ColorTargetView
                );
                LogDebugMessage();
            }

            if (renderTarget.DepthTexture != nullptr && renderTarget.DepthTargetView == nullptr)
            {
                D3D11_DEPTH_STENCIL_VIEW_DESC desc = {};
                desc.Format = DXGI_FORMAT_D32_FLOAT;
                desc.Texture2D.MipSlice = 0;
                desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

                m_Device->CreateDepthStencilView(
                        (ID3D11Resource*)renderTarget.DepthTexture->Instance,
                        &desc,
                        (ID3D11DepthStencilView**)&renderTarget.DepthTargetView
                );
                LogDebugMessage();
            }
        }

        void D3D11Context::BindRenderTarget(void *colorTargetView, void *depthTargetView)
        {
            m_BoundColorTargetView = colorTargetView;
            m_BoundDepthTargetView = depthTargetView;
            m_ImmContext->OMSetRenderTargets(
                    1,
                    (ID3D11RenderTargetView**)&colorTargetView,
                    (ID3D11DepthStencilView*)depthTargetView
            );
            LogDebugMessage();
        }

        void D3D11Context::UnbindRenderTarget()
        {
            m_BoundColorTargetView = nullptr;
            m_BoundDepthTargetView = nullptr;
            m_ImmContext->OMSetRenderTargets(0, 0, 0);
            LogDebugMessage();
        }

        void D3D11Context::ClearColorTarget(const glm::vec4 &color)
        {
            m_ImmContext->ClearRenderTargetView((ID3D11RenderTargetView*)m_BoundColorTargetView, &color.x);
            LogDebugMessage();
        }

        void D3D11Context::ClearDepthTarget(const f32 depth)
        {
            m_ImmContext->ClearDepthStencilView((ID3D11DepthStencilView*)m_BoundDepthTargetView, D3D11_CLEAR_DEPTH, depth, 0);
            LogDebugMessage();
        }

        void D3D11Context::FreeRenderTarget(RenderTarget& renderTarget)
        {
            if (renderTarget.ColorTargetView != nullptr)
            {
                ((ID3D11RenderTargetView*)renderTarget.ColorTargetView)->Release();
                LogDebugMessage();
                renderTarget.ColorTargetView = nullptr;
            }

            if (renderTarget.ColorTexture != nullptr)
            {
                FreeTexture(*renderTarget.ColorTexture);
            }

            if (renderTarget.DepthTargetView != nullptr)
            {
                ((ID3D11DepthStencilView*)renderTarget.DepthTargetView)->Release();
                LogDebugMessage();
                renderTarget.DepthTargetView = nullptr;
            }

            if (renderTarget.DepthTexture != nullptr)
            {
                FreeTexture(*renderTarget.DepthTexture);
            }
        }

        void D3D11Context::ResizeRenderTarget(RenderTarget& renderTarget, int width, int height)
        {
            UnbindRenderTarget();

            FreeRenderTarget(renderTarget);

            if (renderTarget.ColorTexture != nullptr) {
                renderTarget.ColorTexture->Width = width;
                renderTarget.ColorTexture->Height = height;
                renderTarget.ColorTexture->Instance = nullptr;
            }

            if (renderTarget.DepthTexture != nullptr) {
                renderTarget.DepthTexture->Width = width;
                renderTarget.DepthTexture->Height = height;
                renderTarget.DepthTexture->Instance = nullptr;
            }

            CreateRenderTarget(renderTarget);
        }

        void D3D11Context::Present()
        {
            m_SwapChain->Present(0, 0);
            LogDebugMessage();
        }

        void D3D11Context::CompileShaderStage(ShaderStage &stage) {
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
            }
        }

        void D3D11Context::CreateShaderStage(ShaderStage &stage)
        {
            Blob blob = stage.Blob;

            if (blob.Instance != nullptr) {

                const void* byteCode = blob.ByteCode;
                const usize byteCodeSize = blob.ByteCodeSize;

                switch (stage.Type) {

                    case eShaderType::VERTEX:
                        m_Device->CreateVertexShader(
                                byteCode,
                                byteCodeSize,
                                nullptr,
                                (ID3D11VertexShader**)&stage.Resource.Instance
                        );
                        LogDebugMessage();
                        break;

                    case eShaderType::PIXEL:
                        m_Device->CreatePixelShader(
                                byteCode,
                                byteCodeSize,
                                nullptr,
                                (ID3D11PixelShader**)&stage.Resource.Instance
                        );
                        LogDebugMessage();
                        break;

                    case eShaderType::GEOMETRY:
                        m_Device->CreateGeometryShader(
                                byteCode,
                                byteCodeSize,
                                nullptr,
                                (ID3D11GeometryShader**)&stage.Resource.Instance
                        );
                        LogDebugMessage();
                        break;

                    case eShaderType::COMPUTE:
                        m_Device->CreateComputeShader(
                                byteCode,
                                byteCodeSize,
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
            D3D11_TEXTURE1D_DESC texDesc = {};
            D3D11_SHADER_RESOURCE_VIEW_DESC srv = {};

            u32 arraySize = texture.Layers.empty() ? 1 : texture.Layers.size();
            u32 mipLevels = texture.GetMipLevels();
            D3D11_SUBRESOURCE_DATA* initialData = InitTextureData(texture, arraySize, mipLevels);

            UpdateTextureFlags(texture, texDesc.BindFlags, texDesc.MiscFlags);

            texDesc.Width = texture.Width;
            texDesc.MipLevels = mipLevels;
            texDesc.ArraySize = arraySize;
            texDesc.Format = s_TextureFormatTable.at(texture.Format);
            texDesc.Usage = s_TextureUsageTable.at(texture.Usage);
            texDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

            srv.Format = texDesc.Format;
            srv.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1D;
            srv.Texture1D.MostDetailedMip = texture.MostDetailedMip;
            srv.Texture1D.MipLevels = mipLevels;

            m_Device->CreateTexture1D(&texDesc, initialData, (ID3D11Texture1D**)&texture.Instance);
            LogDebugMessage();

            m_Device->CreateShaderResourceView((ID3D11Texture1D*)texture.Instance, &srv, (ID3D11ShaderResourceView**)&texture.ViewInstance);
            LogDebugMessage();

            GenerateMips(texture);

            FreeTextureData(initialData);
        }

        void D3D11Context::CreateTexture2D(Texture &texture)
        {
            D3D11_TEXTURE2D_DESC texDesc = {};
            D3D11_SHADER_RESOURCE_VIEW_DESC srv = {};

            u32 arraySize = texture.Layers.empty() ? 1 : texture.Layers.size();
            u32 mipLevels = texture.GetMipLevels();
            D3D11_SUBRESOURCE_DATA* initialData = InitTextureData(texture, arraySize, mipLevels);

            UpdateTextureFlags(texture, texDesc.BindFlags, texDesc.MiscFlags);

            texDesc.Width = texture.Width;
            texDesc.Height = texture.Height;
            texDesc.MipLevels = mipLevels;
            texDesc.ArraySize = arraySize;
            texDesc.Format = s_TextureFormatTable.at(texture.Format);
            texDesc.SampleDesc.Count = 1;
            texDesc.SampleDesc.Quality = 0;
            texDesc.Usage = s_TextureUsageTable.at(texture.Usage);
            texDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

            srv.Format = texDesc.Format;
            srv.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
            srv.Texture2D.MostDetailedMip = texture.MostDetailedMip;
            srv.Texture2D.MipLevels = mipLevels;

            m_Device->CreateTexture2D(&texDesc, initialData, (ID3D11Texture2D**)&texture.Instance);
            LogDebugMessage();

            m_Device->CreateShaderResourceView((ID3D11Texture2D*)texture.Instance, &srv, (ID3D11ShaderResourceView**)&texture.ViewInstance);
            LogDebugMessage();

            GenerateMips(texture);

            FreeTextureData(initialData);
        }

        void D3D11Context::CreateTexture2DArray(Texture &texture)
        {
            D3D11_TEXTURE2D_DESC texDesc = {};
            D3D11_SHADER_RESOURCE_VIEW_DESC srv = {};

            u32 arraySize = texture.Layers.empty() ? 1 : texture.Layers.size();
            u32 mipLevels = texture.GetMipLevels();
            D3D11_SUBRESOURCE_DATA* initialData = InitTextureData(texture, arraySize, mipLevels);

            UpdateTextureFlags(texture, texDesc.BindFlags, texDesc.MiscFlags);

            texDesc.Width = texture.Width;
            texDesc.Height = texture.Height;
            texDesc.MipLevels = mipLevels;
            texDesc.ArraySize = arraySize;
            texDesc.Format = s_TextureFormatTable.at(texture.Format);
            texDesc.SampleDesc.Count = 1;
            texDesc.SampleDesc.Quality = 0;
            texDesc.Usage = s_TextureUsageTable.at(texture.Usage);
            texDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

            srv.Format = texDesc.Format;
            srv.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
            srv.Texture2DArray.MostDetailedMip = texture.MostDetailedMip;
            srv.Texture2DArray.MipLevels = mipLevels;
            srv.Texture2DArray.FirstArraySlice = 0;
            srv.Texture2DArray.ArraySize = arraySize;

            m_Device->CreateTexture2D(&texDesc, initialData, (ID3D11Texture2D**)&texture.Instance);
            LogDebugMessage();

            m_Device->CreateShaderResourceView((ID3D11Texture2D*)texture.Instance, &srv, (ID3D11ShaderResourceView**)&texture.ViewInstance);
            LogDebugMessage();

            GenerateMips(texture);

            FreeTextureData(initialData);
        }

        void D3D11Context::CreateTexture3D(Texture &texture)
        {
            D3D11_TEXTURE3D_DESC texDesc = {};
            D3D11_SHADER_RESOURCE_VIEW_DESC srv = {};

            u32 arraySize = texture.Layers.empty() ? 1 : texture.Layers.size();
            u32 mipLevels = texture.GetMipLevels();
            D3D11_SUBRESOURCE_DATA* initialData = InitTextureData(texture, arraySize, mipLevels);

            UpdateTextureFlags(texture, texDesc.BindFlags, texDesc.MiscFlags);

            texDesc.Width = texture.Width;
            texDesc.Height = texture.Height;
            texDesc.Depth = texture.Depth;
            texDesc.MipLevels = mipLevels;
            texDesc.Format = s_TextureFormatTable.at(texture.Format);
            texDesc.Usage = s_TextureUsageTable.at(texture.Usage);
            texDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

            srv.Format = texDesc.Format;
            srv.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
            srv.Texture3D.MostDetailedMip = texture.MostDetailedMip;
            srv.Texture3D.MipLevels = mipLevels;

            m_Device->CreateTexture3D(&texDesc, initialData, (ID3D11Texture3D**)&texture.Instance);
            LogDebugMessage();

            m_Device->CreateShaderResourceView((ID3D11Texture3D*)texture.Instance, &srv, (ID3D11ShaderResourceView**)&texture.ViewInstance);
            LogDebugMessage();

            GenerateMips(texture);

            FreeTextureData(initialData);
        }

        void D3D11Context::CreateTextureCube(Texture &texture)
        {
            D3D11_TEXTURE2D_DESC texDesc = {};
            D3D11_SHADER_RESOURCE_VIEW_DESC srv = {};

            u32 arraySize = 6;
            u32 mipLevels = texture.GetMipLevels();
            D3D11_SUBRESOURCE_DATA* initialData = InitTextureData(texture, arraySize, mipLevels);

            UpdateTextureFlags(texture, texDesc.BindFlags, texDesc.MiscFlags);

            texDesc.Width = texture.Width;
            texDesc.Height = texture.Height;
            texDesc.MipLevels = mipLevels;
            texDesc.ArraySize = arraySize;
            texDesc.Format = s_TextureFormatTable.at(texture.Format);
            texDesc.SampleDesc.Count = 1;
            texDesc.SampleDesc.Quality = 0;
            texDesc.Usage = s_TextureUsageTable.at(texture.Usage);
            texDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
            texDesc.MiscFlags |= D3D11_RESOURCE_MISC_TEXTURECUBE;

            srv.Format = texDesc.Format;
            srv.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
            srv.TextureCube.MostDetailedMip = texture.MostDetailedMip;
            srv.TextureCube.MipLevels = mipLevels;

            m_Device->CreateTexture2D(&texDesc, initialData, (ID3D11Texture2D**)&texture.Instance);
            LogDebugMessage();

            m_Device->CreateShaderResourceView((ID3D11Texture2D*)texture.Instance, &srv, (ID3D11ShaderResourceView**)&texture.ViewInstance);
            LogDebugMessage();

            GenerateMips(texture);

            FreeTextureData(initialData);
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

        void D3D11Context::FreeTexture(Texture& texture)
        {
            if (texture.Instance != nullptr)
            {

                switch (texture.Type) {

                    case Texture::eType::TEXTURE_1D:
                        ((ID3D11Texture1D*)texture.Instance)->Release();
                        LogDebugMessage();
                        break;

                    case Texture::eType::TEXTURE_2D:
                        ((ID3D11Texture2D*)texture.Instance)->Release();
                        LogDebugMessage();
                        break;

                    case Texture::eType::TEXTURE_3D:
                        ((ID3D11Texture3D*)texture.Instance)->Release();
                        LogDebugMessage();
                        break;

                    case Texture::eType::TEXTURE_CUBE:
                        ((ID3D11Texture2D*)texture.Instance)->Release();
                        LogDebugMessage();
                        break;

                }

                texture.Instance = nullptr;

            }

            if (texture.ViewInstance != nullptr)
            {

                switch (texture.Type) {

                    case Texture::eType::TEXTURE_1D:
                        ((ID3D11ShaderResourceView*)texture.ViewInstance)->Release();
                        LogDebugMessage();
                        break;

                    case Texture::eType::TEXTURE_2D:
                        ((ID3D11ShaderResourceView*)texture.ViewInstance)->Release();
                        LogDebugMessage();
                        break;

                    case Texture::eType::TEXTURE_3D:
                        ((ID3D11ShaderResourceView*)texture.ViewInstance)->Release();
                        LogDebugMessage();
                        break;

                    case Texture::eType::TEXTURE_CUBE:
                        ((ID3D11ShaderResourceView*)texture.ViewInstance)->Release();
                        LogDebugMessage();
                        break;

                }

                texture.ViewInstance = nullptr;

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
                u32 rowPitch = pixelsSize / texture.Height;
                u32 depthPitch = pixelsSize;
                m_ImmContext->UpdateSubresource((ID3D11Resource*)texture.Instance, index, nullptr, pixels, rowPitch, depthPitch);
                LogDebugMessage();
            }

            else {
                memcpy(mappedResource.pData, pixels, pixelsSize);
                m_ImmContext->Unmap((ID3D11Resource*)texture.Instance, index);
                LogDebugMessage();
            }

        }

        void D3D11Context::GenerateMips(const Texture& texture) {
            u32 mipLevels = texture.GetMipLevels();
            if (mipLevels > 1) {
                m_ImmContext->GenerateMips((ID3D11ShaderResourceView*) texture.ViewInstance);
                LogDebugMessage();
            } else {
                LogWarning("Unable to generate mips on GPU for texture with MipLevels={}", mipLevels);
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

            desc.MaxAnisotropy = sampler.AnisotropyLevel;
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
                    (ID3D11InputLayout**)&inputLayout.Layout
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
            m_ImmContext->IASetInputLayout((ID3D11InputLayout*)m_BoundPipeline->InputLayout.Layout);
            LogDebugMessage();
        }

        void D3D11Context::BindPrimitiveTopology(const ePrimitiveTopology &primitiveTopology) {
            m_ImmContext->IASetPrimitiveTopology(s_PrimitiveTopologyTable.at(primitiveTopology));
            LogDebugMessage();
        }

        void D3D11Context::BindViewport(Viewport* viewport) {
            m_Viewport = viewport;

            D3D11_VIEWPORT v = {};
            v.TopLeftX = viewport->TopLeftX;
            v.TopLeftY = viewport->TopLeftY;
            v.Width = viewport->Width;
            v.Height = viewport->Height;
            v.MinDepth = viewport->MinDepth;
            v.MaxDepth = viewport->MaxDepth;

            m_ImmContext->RSSetViewports(1, &v);
            LogDebugMessage();
        }

        void D3D11Context::FreeInputLayout(InputLayout& inputLayout)
        {
            if (inputLayout.Layout != nullptr)
            {
                ((ID3D11InputLayout*)inputLayout.Layout)->Release();
                LogDebugMessage();
                inputLayout.Layout = nullptr;
            }
        }

        void D3D11Context::CreatePipeline(Pipeline& pipeline)
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

            CreateRasterizer(pipeline.Rasterizer);
        }

        void D3D11Context::BindPipeline(const Pipeline* pipeline)
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

            BindShader(boundPipeline.Shader);

            BindRenderTarget(boundPipeline.RenderTarget->ColorTargetView, boundPipeline.RenderTarget->DepthTargetView);

            BindDepthStencilState(&boundPipeline.DepthStencilState);

            BindRasterizer(&boundPipeline.Rasterizer);
        }

        void D3D11Context::FreePipeline(Pipeline& pipeline)
        {
            FreeInputLayout(pipeline.InputLayout);
            FreeDepthStencilState(pipeline.DepthStencilState);
            FreeRasterizer(pipeline.Rasterizer);
        }

        void D3D11Context::CreateDepthStencilState(DepthStencilState& state)
        {
            D3D11_DEPTH_STENCIL_DESC dsDesc = {};
            dsDesc.DepthEnable = state.UseDepthTest == K_TRUE ? TRUE : FALSE;
            dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
            dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
            dsDesc.StencilEnable = FALSE;

            m_Device->CreateDepthStencilState(&dsDesc, (ID3D11DepthStencilState**)&state.State);
            LogDebugMessage();
        }

        void D3D11Context::BindDepthStencilState(const DepthStencilState* state)
        {
            m_ImmContext->OMSetDepthStencilState((ID3D11DepthStencilState*)state->State, 0);
            LogDebugMessage();
        }

        void D3D11Context::FreeDepthStencilState(DepthStencilState& state)
        {
            if (state.State != nullptr)
            {
                ((ID3D11DepthStencilState*)state.State)->Release();
                LogDebugMessage();
                state.State = nullptr;
            }
        }

        static const std::unordered_map<eFillMode, D3D11_FILL_MODE> s_FillModeTable =
        {
                { eFillMode::SOLID,     D3D11_FILL_SOLID },
                { eFillMode::WIREFRAME, D3D11_FILL_WIREFRAME }
        };

        static const std::unordered_map<eCullMode, D3D11_CULL_MODE> s_CullModeTable =
        {
                { eCullMode::NONE,  D3D11_CULL_NONE },
                { eCullMode::FRONT, D3D11_CULL_FRONT },
                { eCullMode::BACK,  D3D11_CULL_BACK }
        };

        void D3D11Context::CreateRasterizer(Rasterizer &rasterizer)
        {
            D3D11_RASTERIZER_DESC desc = {};
            desc.FillMode = s_FillModeTable.at(rasterizer.FillMode);
            desc.CullMode = s_CullModeTable.at(rasterizer.CullMode);
            desc.FrontCounterClockwise = rasterizer.FrontCounterClockwise;
            desc.DepthBias = rasterizer.DepthBias;
            desc.DepthBiasClamp = rasterizer.DepthBiasClamp;
            desc.SlopeScaledDepthBias = rasterizer.SlopeScaledDepthBias;
            desc.DepthClipEnable = rasterizer.DepthClipEnable;
            desc.ScissorEnable = rasterizer.ScissorEnable;
            desc.MultisampleEnable = rasterizer.MultisampleEnable;
            desc.AntialiasedLineEnable = rasterizer.AntialiasedLineEnable;

            m_Device->CreateRasterizerState(&desc, (ID3D11RasterizerState**)&rasterizer.State);
        }

        void D3D11Context::BindRasterizer(const Rasterizer *rasterizer)
        {
            m_ImmContext->RSSetState((ID3D11RasterizerState*) rasterizer->State);
        }

        void D3D11Context::FreeRasterizer(Rasterizer& rasterizer)
        {
            if (rasterizer.State != nullptr) {
                ((ID3D11RasterizerState*) rasterizer.State)->Release();
                rasterizer.State = nullptr;
            }
        }

        void D3D11Context::DrawBatch(usize vertexOffset, usize indexOffset, usize indexCount, usize instanceCount)
        {
            m_ImmContext->DrawIndexedInstanced(indexCount, instanceCount, 0, vertexOffset, indexOffset);
            LogDebugMessage();
        }

        void D3D11Context::DrawBatch(usize vertexOffset, usize vertexCount, usize instanceCount)
        {
            m_ImmContext->DrawInstanced(vertexCount, instanceCount, 0, vertexOffset);
            LogDebugMessage();
        }

        void D3D11Context::DrawQuad()
        {
            m_ImmContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
            LogDebugMessage();

            m_ImmContext->Draw(4, 1);
            LogDebugMessage();
        }

        void* D3D11Context::GetDevice()
        {
            return m_Device;
        }

        void D3D11Context::CreateHardwareConfig()
        {
            core::HardwareConfig::K_TEXTURE_2D_ARRAY_SIZE_MAX = D3D11_REQ_TEXTURE2D_ARRAY_AXIS_DIMENSION;
            core::HardwareConfig::K_ANISOTROPY_LEVEL_MAX = D3D11_REQ_MAXANISOTROPY;
        }

        D3D11_SUBRESOURCE_DATA* D3D11Context::InitTextureData(const Texture& texture, const u32 arraySize, const u32 mipLevels)
        {
            D3D11_SUBRESOURCE_DATA* initialData = nullptr;

            u32 initialDataSize = arraySize * mipLevels;

            if (texture.InitializeData)
            {
                initialData = allocT(D3D11_SUBRESOURCE_DATA, initialDataSize);

                for (int i = 0; i < arraySize; i++)
                {
                    auto& textureLayer = texture.Layers[i];
                    int mipSize = textureLayer.Mips.size();
                    int k = i * (mipSize + 1);

                    auto& textureData = initialData[k];
                    textureData.pSysMem = textureLayer.Pixels;
                    textureData.SysMemPitch = textureLayer.RowByteSize;
                    textureData.SysMemSlicePitch = 0;

                    for (int j = 0 ; j < mipSize ; j++)
                    {
                        auto& mip = textureLayer.Mips[j];
                        auto& mipData = initialData[++k];
                        mipData.pSysMem = mip.Pixels;
                        mipData.SysMemPitch = mip.RowByteSize;
                        mipData.SysMemSlicePitch = 0;
                    }
                }
            }

            return initialData;
        }

        void D3D11Context::FreeTextureData(D3D11_SUBRESOURCE_DATA* initialData)
        {
            dealloc(initialData);
        }

        void D3D11Context::UpdateTextureFlags(Texture &texture, u32 &bindFlags, u32 &miscFlags)
        {
            bindFlags = D3D11_BIND_SHADER_RESOURCE;

            if (texture.EnableRenderTarget) {
                bindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
            }

            if (texture.GetMipLevels() > 1) {
                bindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
                miscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
            }
        }

    }

}
