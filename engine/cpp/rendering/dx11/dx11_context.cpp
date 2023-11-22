#ifdef DX11

#include <rendering/core/debugger.h>
#include <d3dcompiler.h>
#include <d3d11.h>
#include <rendering/core/context.hpp>


namespace xpe {

    namespace render {

        namespace context {

            static ID3D11Device* s_Device = nullptr;
            static ID3D11DeviceContext* s_ImmContext = nullptr;
            static ID3D11DeviceContext* s_DefContext = nullptr;
            static IDXGISwapChain* s_SwapChain = nullptr;
            static IDXGIDevice* s_GIDevice = nullptr;
            static IDXGIAdapter* s_GIAdapter = nullptr;
            static IDXGIFactory* s_GIFactory = nullptr;
            static DXGI_MODE_DESC s_GIModeDesc;

            static const std::unordered_map<eBufferUsage, D3D11_USAGE> k_BufferUsageTable =
            {
                    { eBufferUsage::DEFAULT, D3D11_USAGE_DEFAULT },
                    { eBufferUsage::STATIC, D3D11_USAGE_IMMUTABLE },
                    { eBufferUsage::DYNAMIC, D3D11_USAGE_DYNAMIC },
                    { eBufferUsage::STAGING, D3D11_USAGE_STAGING }
            };

            static const std::unordered_map<eBufferUsage, u32> k_BufferCpuFlagTable =
            {
                    { eBufferUsage::DEFAULT, D3D11_CPU_ACCESS_WRITE },
                    { eBufferUsage::STATIC, 0 },
                    { eBufferUsage::DYNAMIC, D3D11_CPU_ACCESS_WRITE },
                    { eBufferUsage::STAGING, D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE }
            };

            static const std::unordered_map<eTextureFormat, DXGI_FORMAT> k_TextureFormatTable =
            {
                    { eTextureFormat::R8, DXGI_FORMAT_R8_UNORM },
                    { eTextureFormat::R16, DXGI_FORMAT_R16_UNORM },
                    { eTextureFormat::R32, DXGI_FORMAT_R32_FLOAT },
                    { eTextureFormat::R32_TYPELESS, DXGI_FORMAT_R32_TYPELESS },

                    { eTextureFormat::RG8, DXGI_FORMAT_R8G8_UNORM },
                    { eTextureFormat::RG16, DXGI_FORMAT_R16G16_UNORM },
                    { eTextureFormat::RG32, DXGI_FORMAT_R32G32_FLOAT },

                    { eTextureFormat::RGB8, DXGI_FORMAT_R8G8B8A8_UNORM },
                    { eTextureFormat::RGB16, DXGI_FORMAT_R16G16B16A16_UNORM },
                    { eTextureFormat::RGB32, DXGI_FORMAT_R32G32B32_FLOAT },

                    { eTextureFormat::RGBA8, DXGI_FORMAT_R8G8B8A8_UNORM },
                    { eTextureFormat::RGBA16, DXGI_FORMAT_R16G16B16A16_UNORM },
                    { eTextureFormat::RGBA32, DXGI_FORMAT_R32G32B32A32_FLOAT },

                    { eTextureFormat::SRGBA8, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB }
            };

            static const std::unordered_map<sTexture::eUsage, D3D11_USAGE> k_TextureUsageTable =
            {
                    {sTexture::eUsage::DEFAULT, D3D11_USAGE_DEFAULT },
                    {sTexture::eUsage::STATIC,  D3D11_USAGE_IMMUTABLE },
                    {sTexture::eUsage::DYNAMIC, D3D11_USAGE_DYNAMIC },
                    {sTexture::eUsage::STAGING, D3D11_USAGE_STAGING }
            };

            static const std::unordered_map<sTexture::eUsage, u32> k_TextureCpuFlagTable =
            {
                    {sTexture::eUsage::DEFAULT, D3D11_CPU_ACCESS_WRITE },
                    {sTexture::eUsage::STATIC,  0 },
                    {sTexture::eUsage::DYNAMIC, D3D11_CPU_ACCESS_WRITE },
                    {sTexture::eUsage::STAGING, D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE }
            };

            static const std::unordered_map<sSampler::eComparison, D3D11_COMPARISON_FUNC> k_SamplerComparisonTable =
            {
                    {sSampler::eComparison::ALWAYS,        D3D11_COMPARISON_ALWAYS },
                    {sSampler::eComparison::EQUAL,         D3D11_COMPARISON_EQUAL },
                    {sSampler::eComparison::GREATER,       D3D11_COMPARISON_GREATER },
                    {sSampler::eComparison::GREATER_EQUAL, D3D11_COMPARISON_GREATER_EQUAL },
                    {sSampler::eComparison::LESS,          D3D11_COMPARISON_LESS },
                    {sSampler::eComparison::LESS_EQUAL,    D3D11_COMPARISON_LESS_EQUAL },
                    {sSampler::eComparison::NEVER,         D3D11_COMPARISON_NEVER },
                    {sSampler::eComparison::NOT_EQUAL,     D3D11_COMPARISON_NOT_EQUAL }
            };

            static const std::unordered_map<sSampler::eFilter, D3D11_FILTER> k_SamplerFilterTable =
            {
                    {sSampler::eFilter::MIN_MAG_MIP_POINT,  D3D11_FILTER_MIN_MAG_MIP_POINT },
                    {sSampler::eFilter::MIN_MAG_MIP_LINEAR, D3D11_FILTER_MIN_MAG_MIP_LINEAR },
                    {sSampler::eFilter::ANISOTROPIC,        D3D11_FILTER_ANISOTROPIC },
            };

            static const std::unordered_map<sSampler::eAddress, D3D11_TEXTURE_ADDRESS_MODE> k_SamplerAddressTable =
            {
                    {sSampler::eAddress::WRAP,        D3D11_TEXTURE_ADDRESS_WRAP },
                    {sSampler::eAddress::BORDER,      D3D11_TEXTURE_ADDRESS_BORDER },
                    {sSampler::eAddress::CLAMP,       D3D11_TEXTURE_ADDRESS_CLAMP },
                    {sSampler::eAddress::MIRROR,      D3D11_TEXTURE_ADDRESS_MIRROR },
                    {sSampler::eAddress::MIRROR_ONCE, D3D11_TEXTURE_ADDRESS_MIRROR_ONCE }
            };

            static const std::unordered_map<sVertexFormat::sAttribute::eType, DXGI_FORMAT> k_VertexFormatTable =
            {
                    {sVertexFormat::sAttribute::eType::BOOL,          DXGI_FORMAT_R32_UINT },
                    {sVertexFormat::sAttribute::eType::INT,           DXGI_FORMAT_R32_SINT },
                    {sVertexFormat::sAttribute::eType::UINT,          DXGI_FORMAT_R32_UINT },
                    {sVertexFormat::sAttribute::eType::FLOAT,         DXGI_FORMAT_R32_FLOAT },

                    {sVertexFormat::sAttribute::eType::VEC2_FLOAT,    DXGI_FORMAT_R32G32_FLOAT },
                    {sVertexFormat::sAttribute::eType::VEC2_UINT,     DXGI_FORMAT_R32G32_UINT },
                    {sVertexFormat::sAttribute::eType::VEC2_INT,      DXGI_FORMAT_R32G32_SINT },
                    {sVertexFormat::sAttribute::eType::VEC2_TYPELESS, DXGI_FORMAT_R32G32_TYPELESS },

                    {sVertexFormat::sAttribute::eType::VEC3_FLOAT,    DXGI_FORMAT_R32G32B32_FLOAT },
                    {sVertexFormat::sAttribute::eType::VEC3_UINT,     DXGI_FORMAT_R32G32B32_UINT },
                    {sVertexFormat::sAttribute::eType::VEC3_INT,      DXGI_FORMAT_R32G32B32_SINT },
                    {sVertexFormat::sAttribute::eType::VEC3_TYPELESS, DXGI_FORMAT_R32G32B32_TYPELESS },

                    {sVertexFormat::sAttribute::eType::VEC4_FLOAT,    DXGI_FORMAT_R32G32B32A32_FLOAT },
                    {sVertexFormat::sAttribute::eType::VEC4_UINT,     DXGI_FORMAT_R32G32B32A32_UINT },
                    {sVertexFormat::sAttribute::eType::VEC4_INT,      DXGI_FORMAT_R32G32B32A32_SINT },
                    {sVertexFormat::sAttribute::eType::VEC4_TYPELESS, DXGI_FORMAT_R32G32B32A32_TYPELESS },
            };

            static const std::unordered_map<ePrimitiveTopology, D3D11_PRIMITIVE_TOPOLOGY> k_PrimitiveTopologyTable =
            {
                    { ePrimitiveTopology::TRIANGLE_STRIP, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP },
                    { ePrimitiveTopology::TRIANGLE_LIST, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST },
                    { ePrimitiveTopology::POINT_LIST, D3D11_PRIMITIVE_TOPOLOGY_POINTLIST },
                    { ePrimitiveTopology::LINE_LIST, D3D11_PRIMITIVE_TOPOLOGY_LINELIST },
                    { ePrimitiveTopology::LINE_STRIP, D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP }
            };

            static const std::unordered_map<eFillMode, D3D11_FILL_MODE> k_FillModeTable =
            {
                    { eFillMode::SOLID,     D3D11_FILL_SOLID },
                    { eFillMode::WIREFRAME, D3D11_FILL_WIREFRAME }
            };

            static const std::unordered_map<eCullMode, D3D11_CULL_MODE> k_CullModeTable =
            {
                    { eCullMode::NONE,  D3D11_CULL_NONE },
                    { eCullMode::FRONT, D3D11_CULL_FRONT },
                    { eCullMode::BACK,  D3D11_CULL_BACK }
            };

            static const std::unordered_map<eMapType, D3D11_MAP> k_MapTypeTable =
            {
                    { eMapType::READ, D3D11_MAP_READ },
                    { eMapType::WRITE, D3D11_MAP_WRITE },
                    { eMapType::READ_WRITE, D3D11_MAP_READ_WRITE },
                    { eMapType::WRITE_NO_OVERWRITE, D3D11_MAP_WRITE_NO_OVERWRITE },
                    { eMapType::WRITE_DISCARD, D3D11_MAP_WRITE_DISCARD },
            };

            static const std::unordered_map<eDepthWriteMask, D3D11_DEPTH_WRITE_MASK> k_DepthWriteMaskTable =
            {
                    { eDepthWriteMask::ZERO, D3D11_DEPTH_WRITE_MASK_ZERO },
                    { eDepthWriteMask::ALL, D3D11_DEPTH_WRITE_MASK_ALL },
            };

            static const std::unordered_map<eDepthStencilFunc, D3D11_COMPARISON_FUNC> k_DepthStencilComparisonTable =
            {
                    {eDepthStencilFunc::NEVER,         D3D11_COMPARISON_NEVER },
                    {eDepthStencilFunc::LESS,          D3D11_COMPARISON_LESS },
                    {eDepthStencilFunc::EQUAL,         D3D11_COMPARISON_EQUAL },
                    {eDepthStencilFunc::LESS_EQUAL,    D3D11_COMPARISON_LESS_EQUAL },
                    {eDepthStencilFunc::GREATER,       D3D11_COMPARISON_GREATER },
                    {eDepthStencilFunc::NOT_EQUAL,     D3D11_COMPARISON_NOT_EQUAL },
                    {eDepthStencilFunc::GREATER_EQUAL, D3D11_COMPARISON_GREATER_EQUAL },
                    {eDepthStencilFunc::ALWAYS,        D3D11_COMPARISON_ALWAYS }
            };

            static const std::unordered_map<eDepthStencilOp, D3D11_STENCIL_OP> k_DepthStencilOpTable =
            {
                    {eDepthStencilOp::KEEP,     D3D11_STENCIL_OP_KEEP },
                    {eDepthStencilOp::ZERO,     D3D11_STENCIL_OP_ZERO },
                    {eDepthStencilOp::REPLACE,  D3D11_STENCIL_OP_REPLACE },
                    {eDepthStencilOp::INCR_SAT, D3D11_STENCIL_OP_INCR_SAT },
                    {eDepthStencilOp::DECR_SAT, D3D11_STENCIL_OP_DECR_SAT },
                    {eDepthStencilOp::INVERT,   D3D11_STENCIL_OP_INVERT },
                    {eDepthStencilOp::INCR,     D3D11_STENCIL_OP_INCR },
                    {eDepthStencilOp::DECR,     D3D11_STENCIL_OP_DECR }
            };

            static const std::unordered_map<eBlend, D3D11_BLEND> k_BlendTable =
            {
                    { eBlend::ZERO, D3D11_BLEND_ZERO },
                    { eBlend::ONE, D3D11_BLEND_ONE },
                    { eBlend::SRC_COLOR, D3D11_BLEND_SRC_COLOR },
                    { eBlend::INV_SRC_COLOR, D3D11_BLEND_INV_SRC_COLOR },
                    { eBlend::SRC_ALPHA, D3D11_BLEND_SRC_ALPHA },
                    { eBlend::INV_SRC_ALPHA, D3D11_BLEND_INV_SRC_ALPHA },
                    { eBlend::DEST_ALPHA, D3D11_BLEND_DEST_ALPHA },
                    { eBlend::INV_DEST_ALPHA, D3D11_BLEND_INV_DEST_ALPHA },
                    { eBlend::DEST_COLOR, D3D11_BLEND_DEST_COLOR },
                    { eBlend::INV_DEST_COLOR, D3D11_BLEND_INV_DEST_COLOR },
                    { eBlend::SRC_ALPHA_SAT, D3D11_BLEND_SRC_ALPHA_SAT },
                    { eBlend::BLEND_FACTOR, D3D11_BLEND_BLEND_FACTOR },
                    { eBlend::INV_BLEND_FACTOR, D3D11_BLEND_INV_BLEND_FACTOR },
                    { eBlend::SRC1_COLOR, D3D11_BLEND_SRC1_COLOR },
                    { eBlend::INV_SRC1_COLOR, D3D11_BLEND_INV_SRC1_COLOR },
                    { eBlend::SRC1_ALPHA, D3D11_BLEND_SRC1_ALPHA },
                    { eBlend::INV_SRC1_ALPHA, D3D11_BLEND_INV_SRC1_ALPHA }
            };

            static const std::unordered_map<eBlendOp, D3D11_BLEND_OP> k_BlendOpTable =
            {
                    { eBlendOp::ADD, D3D11_BLEND_OP_ADD },
                    { eBlendOp::SUBTRACT, D3D11_BLEND_OP_SUBTRACT },
                    { eBlendOp::REV_SUBTRACT, D3D11_BLEND_OP_REV_SUBTRACT },
                    { eBlendOp::MIN, D3D11_BLEND_OP_MIN },
                    { eBlendOp::MAX, D3D11_BLEND_OP_MAX }
            };

            static void FreeInitialData(D3D11_SUBRESOURCE_DATA* initialData)
            {
                main_free(initialData);
            }

            static D3D11_SUBRESOURCE_DATA* InitBufferData(const sBuffer& buffer)
            {
                D3D11_SUBRESOURCE_DATA* initialData = nullptr;

                if (buffer.InitialData != nullptr)
                {
                    usize bufferSize = buffer.GetByteSize();
                    initialData = main_allocT(D3D11_SUBRESOURCE_DATA, bufferSize);
                    initialData->pSysMem = buffer.InitialData;
                    initialData->SysMemPitch = bufferSize;
                    initialData->SysMemSlicePitch = 0;
                }

                return initialData;
            }

            static D3D11_SUBRESOURCE_DATA* InitTextureData(const sTexture& texture, const u32 arraySize, const u32 mipLevels)
            {
                D3D11_SUBRESOURCE_DATA* initialData = nullptr;

                u32 initialDataSize = arraySize * mipLevels;

                if (texture.InitializeData)
                {
                    initialData = main_allocT(D3D11_SUBRESOURCE_DATA, initialDataSize);

                    for (int i = 0; i < arraySize; i++)
                    {
                        auto& textureLayer = texture.Layers[i];
                        int mipSize = textureLayer.Mips.size();
                        int k = i * (mipSize + 1);

                        auto& textureData = initialData[k];
                        textureData.pSysMem = textureLayer.Pixels;
                        textureData.SysMemPitch = textureLayer.Width * sTexture::k_BppTable.at(textureLayer.Format);
                        textureData.SysMemSlicePitch = 0;

                        for (int j = 0 ; j < mipSize ; j++)
                        {
                            auto& mip = textureLayer.Mips[j];
                            auto& mipData = initialData[++k];
                            mipData.pSysMem = mip.Pixels;
                            mipData.SysMemPitch = mip.Width * sTexture::k_BppTable.at(mip.Format);
                            mipData.SysMemSlicePitch = 0;
                        }
                    }
                }

                return initialData;
            }

            static void UpdateTextureFlags(sTexture& texture, u32& bindFlags, u32& miscFlags)
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

            void Init()
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
                        (ID3D11Device**)&s_Device,
                        nullptr,
                        (ID3D11DeviceContext**)&s_ImmContext
                );

                if (result != S_OK) {
                    FMT_ASSERT(false, "Failed to create rendering context and device for DX11");
                    return;
                }

                InitDebugger();

                s_Device->CreateDeferredContext(0, &s_DefContext);
                LogDebugMessage();

                cHardwareManager::UpdateGpuStats(s_Device);
                LogDebugMessage();

                s_Device->QueryInterface(__uuidof(IDXGIDevice), (void **)&s_GIDevice);
                LogDebugMessage();

                s_GIDevice->GetAdapter(&s_GIAdapter);
                LogDebugMessage();

                s_GIAdapter->GetParent(__uuidof(IDXGIFactory), (void **)&s_GIFactory);
                LogDebugMessage();

                CreateSwapchain(cWindowManager::GetWidth(), cWindowManager::GetHeight());

                LogInfo("D3D11Context initialized");
            }

            void Free()
            {
                LogInfo("D3D11Context::Free()");

                s_DefContext->Release();
                LogDebugMessage();

                s_ImmContext->Release();
                LogDebugMessage();

                s_Device->Release();
                LogDebugMessage();

                s_GIDevice->Release();
                LogDebugMessage();

                s_GIAdapter->Release();
                LogDebugMessage();

                s_GIFactory->Release();
                LogDebugMessage();

                FreeSwapchain();

                FreeDebugger();
            }

            void CreateSwapchain(int width, int height)
            {
                s_GIModeDesc = {};
                s_GIModeDesc.Width = width;
                s_GIModeDesc.Height = height;
                s_GIModeDesc.RefreshRate.Numerator = cWindowManager::GetRefreshRate();
                s_GIModeDesc.RefreshRate.Denominator = 1;
                s_GIModeDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
                s_GIModeDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
                s_GIModeDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

                DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
                swapChainDesc.BufferDesc = s_GIModeDesc;
                swapChainDesc.SampleDesc.Count = 1;
                swapChainDesc.SampleDesc.Quality = 0;
                swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
                swapChainDesc.BufferCount = 1;
                swapChainDesc.OutputWindow = (HWND)cWindowManager::GetWin32Instance();
                swapChainDesc.Windowed = cWindowManager::IsWindowed();
                swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

                s_GIFactory->CreateSwapChain(s_Device, &swapChainDesc, &s_SwapChain);
                LogDebugMessage();

                s_SwapChain->SetFullscreenState(cWindowManager::IsFullscreen(), nullptr);

                CreateSwapchainTargetView();
            }

            void FreeSwapchain()
            {
                s_SwapChain->Release();
                LogDebugMessage();
            }

            void ResizeSwapchain(sRenderTarget& renderTarget, int width, int height)
            {
                UnbindRenderTarget();
                if (SwapchainTextureInstance) {
                    ((ID3D11Texture2D*) SwapchainTextureInstance)->Release();
                    SwapchainTextureInstance = nullptr;
                }
                FreeRenderTargetColorViews(renderTarget.ColorViews);

                s_SwapChain->ResizeBuffers(1, 0, 0, DXGI_FORMAT_UNKNOWN, 0ul);
                LogDebugMessage();

                DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
                s_SwapChain->GetDesc(&swapChainDesc);
                LogDebugMessage();

                LogWarning("ResizeSwapchain: expected resolution=({}, {}); actual resolution=({}, {})", width, height, swapChainDesc.BufferDesc.Width, swapChainDesc.BufferDesc.Height);

                CreateSwapchainTargetView();

                renderTarget.ColorViews[0] = SwapchainTargetView;
                BindRenderTarget(renderTarget.ColorViews, renderTarget.DepthStencilView);

                if (renderTarget.Viewport) {
                    renderTarget.Viewport->Width = width;
                    renderTarget.Viewport->Height = height;
                }

                BindViewport(renderTarget.Viewport);
            }

            void CreateSwapchainTargetView()
            {
                s_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**) &SwapchainTextureInstance);
                LogDebugMessage();

                D3D11_RENDER_TARGET_VIEW_DESC rtDesc = {};
                rtDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
                rtDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

                s_Device->CreateRenderTargetView(
                        (ID3D11Texture2D*) SwapchainTextureInstance,
                        &rtDesc,
                        (ID3D11RenderTargetView**) &SwapchainTargetView
                );
                LogDebugMessage();
            }

            void CreateRenderTarget(sRenderTarget& renderTarget)
            {
                u32 colorCount = renderTarget.Colors.size();
                for (u32 i = 0 ; i < colorCount ; i++)
                {
                    auto& color = renderTarget.Colors[i];
                    auto& colorView = renderTarget.ColorViews[i];

                    if (color && color->Instance && color->Width != 0 && color->Height != 0 && colorView == nullptr)
                    {
                        D3D11_RENDER_TARGET_VIEW_DESC desc = {};
                        desc.Format = k_TextureFormatTable.at(color->Format);
                        desc.ViewDimension = color->SampleCount > 1 ? D3D11_RTV_DIMENSION_TEXTURE2DMS : D3D11_RTV_DIMENSION_TEXTURE2D;
                        desc.Texture2D.MipSlice = 0;

                        s_Device->CreateRenderTargetView(
                                (ID3D11Resource*) color->Instance,
                                &desc,
                                (ID3D11RenderTargetView**) &colorView
                        );
                        LogDebugMessage();
                    }
                }

                auto& depth = renderTarget.DepthStencil;
                auto& depthView = renderTarget.DepthStencilView;

                if (depth && depth->Instance && depth->Width != 0 && depth->Height != 0 && depthView == nullptr)
                {
                    D3D11_DEPTH_STENCIL_VIEW_DESC desc = {};
                    desc.Format = DXGI_FORMAT_D32_FLOAT;
                    desc.ViewDimension = depth->SampleCount > 1 ? D3D11_DSV_DIMENSION_TEXTURE2DMS : D3D11_DSV_DIMENSION_TEXTURE2D;
                    desc.Texture2D.MipSlice = 0;

                    s_Device->CreateDepthStencilView(
                            (ID3D11Resource*) depth->Instance,
                            &desc,
                            (ID3D11DepthStencilView**) &depthView
                    );
                    LogDebugMessage();
                }
            }

            void BindRenderTarget(const vector<void*>& colorViews, void* depthView)
            {
                s_ImmContext->OMSetRenderTargets(
                        colorViews.size(),
                        colorViews.empty() ? nullptr : (ID3D11RenderTargetView**) &colorViews[0],
                        (ID3D11DepthStencilView*) depthView
                );
                LogDebugMessage();
            }

            void UnbindRenderTarget()
            {
                s_ImmContext->OMSetRenderTargets(0, nullptr, nullptr);
                LogDebugMessage();
            }

            void ClearColorTarget(void* colorView, const glm::vec4 &color)
            {
                s_ImmContext->ClearRenderTargetView((ID3D11RenderTargetView*) colorView, &color.x);
                LogDebugMessage();
            }

            void ClearDepthTarget(void *depthView, const f32 depth)
            {
                s_ImmContext->ClearDepthStencilView((ID3D11DepthStencilView*) depthView, D3D11_CLEAR_DEPTH, depth, 0);
                LogDebugMessage();
            }

            void ClearStencilTarget(void *depthView, const u8 stencil)
            {
                s_ImmContext->ClearDepthStencilView((ID3D11DepthStencilView*) depthView, D3D11_CLEAR_STENCIL, 0, stencil);
                LogDebugMessage();
            }

            void ClearDepthStencilTarget(void *depthView, const f32 depth, const u8 stencil)
            {
                s_ImmContext->ClearDepthStencilView((ID3D11DepthStencilView*) depthView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, depth, stencil);
                LogDebugMessage();
            }

            void FreeRenderTargetColorViews(vector<void*> &colorViews)
            {
                for (auto& colorView : colorViews)
                {
                    if (colorView != nullptr)
                    {
                        ((ID3D11RenderTargetView*) colorView)->Release();
                        LogDebugMessage();
                        colorView = nullptr;
                    }
                }
            }

            void FreeRenderTargetDepthView(void **depthView)
            {
                auto& out = *depthView;
                if (out != nullptr)
                {
                    ((ID3D11DepthStencilView*) out)->Release();
                    LogDebugMessage();
                    out = nullptr;
                }
            }

            void ResizeRenderTarget(sRenderTarget& renderTarget, int width, int height)
            {
                UnbindRenderTarget();
                FreeRenderTarget(renderTarget);
                if (renderTarget.Viewport) {
                    renderTarget.Viewport->Width = width;
                    renderTarget.Viewport->Height = height;
                }
                CreateRenderTarget(renderTarget);
            }

            void Present()
            {
                s_SwapChain->Present(0, 0);
                LogDebugMessage();
            }

            void CompileShaderStage(sShaderStage &stage) {
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

            void CreateShaderStage(sShaderStage &stage)
            {
                sBlob blob = stage.Blob;

                if (blob.Instance != nullptr) {

                    const void* byteCode = blob.ByteCode;
                    const usize byteCodeSize = blob.ByteCodeSize;

                    switch (stage.Type) {

                        case eShaderType::VERTEX:
                            s_Device->CreateVertexShader(
                                    byteCode,
                                    byteCodeSize,
                                    nullptr,
                                    (ID3D11VertexShader**)&stage.Instance
                            );
                            LogDebugMessage();
                            break;

                        case eShaderType::PIXEL:
                            s_Device->CreatePixelShader(
                                    byteCode,
                                    byteCodeSize,
                                    nullptr,
                                    (ID3D11PixelShader**)&stage.Instance
                            );
                            LogDebugMessage();
                            break;

                        case eShaderType::GEOMETRY:
                            s_Device->CreateGeometryShader(
                                    byteCode,
                                    byteCodeSize,
                                    nullptr,
                                    (ID3D11GeometryShader**)&stage.Instance
                            );
                            LogDebugMessage();
                            break;

                        case eShaderType::COMPUTE:
                            s_Device->CreateComputeShader(
                                    byteCode,
                                    byteCodeSize,
                                    nullptr,
                                    (ID3D11ComputeShader**)&stage.Instance
                            );
                            LogDebugMessage();
                            break;

                    }
                }
            }

            void BindShaderStage(const sShaderStage &stage)
            {
                switch (stage.Type) {

                    case eShaderType::VERTEX:
                        s_ImmContext->VSSetShader((ID3D11VertexShader*) stage.Instance, nullptr, 0);
                        LogDebugMessage();
                        break;

                    case eShaderType::PIXEL:
                        s_ImmContext->PSSetShader((ID3D11PixelShader*) stage.Instance, nullptr, 0);
                        LogDebugMessage();
                        break;

                    case eShaderType::GEOMETRY:
                        s_ImmContext->GSSetShader((ID3D11GeometryShader*) stage.Instance, nullptr, 0);
                        LogDebugMessage();
                        break;

                    case eShaderType::COMPUTE:
                        s_ImmContext->CSSetShader((ID3D11ComputeShader*) stage.Instance, nullptr, 0);
                        LogDebugMessage();
                        break;

                }
            }

            void FreeShaderStage(sShaderStage &stage)
            {
                if (stage.Blob.Instance != nullptr) {
                    ((ID3DBlob*) stage.Blob.Instance)->Release();
                    LogDebugMessage();
                    stage.Blob.Instance = nullptr;
                }

                if (stage.Instance != nullptr) {
                    switch (stage.Type) {
                        case eShaderType::VERTEX:
                            ((ID3D11VertexShader*) stage.Instance)->Release();
                            LogDebugMessage();
                            break;

                        case eShaderType::PIXEL:
                            ((ID3D11PixelShader*) stage.Instance)->Release();
                            LogDebugMessage();
                            break;

                        case eShaderType::GEOMETRY:
                            ((ID3D11GeometryShader*) stage.Instance)->Release();
                            LogDebugMessage();
                            break;

                        case eShaderType::COMPUTE:
                            ((ID3D11ComputeShader*) stage.Instance)->Release();
                            LogDebugMessage();
                            break;
                    }
                    stage.Instance = nullptr;
                }
            }

            void FreeShaderResourceView(void* viewInstance)
            {
                ((ID3D11ShaderResourceView*) viewInstance)->Release();
                LogDebugMessage();
            }

            void CreateTexture1D(sTexture &texture)
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
                texDesc.Format = k_TextureFormatTable.at(texture.Format);
                texDesc.Usage = k_TextureUsageTable.at(texture.Usage);
                texDesc.CPUAccessFlags = k_TextureCpuFlagTable.at(texture.Usage);

                srv.Format = texDesc.Format;
                srv.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1D;
                srv.Texture1D.MostDetailedMip = texture.MostDetailedMip;
                srv.Texture1D.MipLevels = mipLevels;

                s_Device->CreateTexture1D(&texDesc, initialData, (ID3D11Texture1D**) &texture.Instance);
                LogDebugMessage();

                s_Device->CreateShaderResourceView((ID3D11Texture1D*) texture.Instance, &srv, (ID3D11ShaderResourceView**) &texture.ViewInstance);
                LogDebugMessage();

                GenerateMips(texture);

                FreeInitialData(initialData);
            }

            void CreateTexture2D(sTexture &texture)
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
                texDesc.Format = k_TextureFormatTable.at(texture.Format);
                texDesc.SampleDesc.Count = texture.SampleCount;
                texDesc.SampleDesc.Quality = 0;
                texDesc.Usage = k_TextureUsageTable.at(texture.Usage);
                texDesc.CPUAccessFlags = texture.SampleCount == 1 ? k_TextureCpuFlagTable.at(texture.Usage) : 0;

                srv.Format = texDesc.Format;
                srv.ViewDimension = texture.SampleCount > 1 ? D3D11_SRV_DIMENSION_TEXTURE2DMS : D3D11_SRV_DIMENSION_TEXTURE2D;
                srv.Texture2D.MostDetailedMip = texture.MostDetailedMip;
                srv.Texture2D.MipLevels = mipLevels;

                s_Device->CreateTexture2D(&texDesc, initialData, (ID3D11Texture2D**)&texture.Instance);
                LogDebugMessage();

                s_Device->CreateShaderResourceView((ID3D11Texture2D*)texture.Instance, &srv, (ID3D11ShaderResourceView**)&texture.ViewInstance);
                LogDebugMessage();

                GenerateMips(texture);

                FreeInitialData(initialData);
            }

            void CreateTexture2DArray(sTexture &texture)
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
                texDesc.Format = k_TextureFormatTable.at(texture.Format);
                texDesc.SampleDesc.Count = 1;
                texDesc.SampleDesc.Quality = 0;
                texDesc.Usage = k_TextureUsageTable.at(texture.Usage);
                texDesc.CPUAccessFlags = k_TextureCpuFlagTable.at(texture.Usage);

                srv.Format = texDesc.Format;
                srv.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
                srv.Texture2DArray.MostDetailedMip = texture.MostDetailedMip;
                srv.Texture2DArray.MipLevels = mipLevels;
                srv.Texture2DArray.FirstArraySlice = 0;
                srv.Texture2DArray.ArraySize = arraySize;

                s_Device->CreateTexture2D(&texDesc, initialData, (ID3D11Texture2D**)&texture.Instance);
                LogDebugMessage();

                s_Device->CreateShaderResourceView((ID3D11Texture2D*)texture.Instance, &srv, (ID3D11ShaderResourceView**)&texture.ViewInstance);
                LogDebugMessage();

                GenerateMips(texture);

                FreeInitialData(initialData);
            }

            void CreateTexture3D(sTexture &texture)
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
                texDesc.Format = k_TextureFormatTable.at(texture.Format);
                texDesc.Usage = k_TextureUsageTable.at(texture.Usage);
                texDesc.CPUAccessFlags = k_TextureCpuFlagTable.at(texture.Usage);

                srv.Format = texDesc.Format;
                srv.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
                srv.Texture3D.MostDetailedMip = texture.MostDetailedMip;
                srv.Texture3D.MipLevels = mipLevels;

                s_Device->CreateTexture3D(&texDesc, initialData, (ID3D11Texture3D**)&texture.Instance);
                LogDebugMessage();

                s_Device->CreateShaderResourceView((ID3D11Texture3D*)texture.Instance, &srv, (ID3D11ShaderResourceView**)&texture.ViewInstance);
                LogDebugMessage();

                GenerateMips(texture);

                FreeInitialData(initialData);
            }

            void CreateTextureCube(sTexture &texture)
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
                texDesc.Format = k_TextureFormatTable.at(texture.Format);
                texDesc.SampleDesc.Count = 1;
                texDesc.SampleDesc.Quality = 0;
                texDesc.Usage = k_TextureUsageTable.at(texture.Usage);
                texDesc.CPUAccessFlags = k_TextureCpuFlagTable.at(texture.Usage);
                texDesc.MiscFlags |= D3D11_RESOURCE_MISC_TEXTURECUBE;

                srv.Format = texDesc.Format;
                srv.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
                srv.TextureCube.MostDetailedMip = texture.MostDetailedMip;
                srv.TextureCube.MipLevels = mipLevels;

                s_Device->CreateTexture2D(&texDesc, initialData, (ID3D11Texture2D**)&texture.Instance);
                LogDebugMessage();

                s_Device->CreateShaderResourceView((ID3D11Texture2D*)texture.Instance, &srv, (ID3D11ShaderResourceView**)&texture.ViewInstance);
                LogDebugMessage();

                GenerateMips(texture);

                FreeInitialData(initialData);
            }

            void CreateTextureDepthStencil(sTexture &texture)
            {
                D3D11_TEXTURE2D_DESC texDesc = {};
                D3D11_SHADER_RESOURCE_VIEW_DESC srv = {};

                texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
                texDesc.Format = k_TextureFormatTable.at(texture.Format);
                texDesc.Width = texture.Width;
                texDesc.Height = texture.Height;
                texDesc.ArraySize = 1;
                texDesc.SampleDesc.Count = texture.SampleCount;
                texDesc.SampleDesc.Quality = 0;
                texDesc.MipLevels = 1;
                texDesc.Usage = k_TextureUsageTable.at(texture.Usage);
                texDesc.CPUAccessFlags = 0;

                s_Device->CreateTexture2D(&texDesc, nullptr, (ID3D11Texture2D**)&texture.Instance);
                LogDebugMessage();

                srv.Format = DXGI_FORMAT_R32_FLOAT;
                srv.ViewDimension = texture.SampleCount == 1 ? D3D11_SRV_DIMENSION_TEXTURE2D : D3D11_SRV_DIMENSION_TEXTURE2DMS;
                srv.Texture2D.MostDetailedMip = texture.MostDetailedMip;
                srv.Texture2D.MipLevels = 1;

                s_Device->CreateShaderResourceView((ID3D11Texture2D*)texture.Instance, &srv, (ID3D11ShaderResourceView**)&texture.ViewInstance);
                LogDebugMessage();
            }

            void BindTexture(const sTexture& texture)
            {
                s_ImmContext->PSSetShaderResources(texture.Slot, 1, (ID3D11ShaderResourceView**)&texture.ViewInstance);
                LogDebugMessage();
            }

            void BindTexture(const sTexture& texture, u32 slot)
            {
                s_ImmContext->PSSetShaderResources(slot, 1, (ID3D11ShaderResourceView**)&texture.ViewInstance);
                LogDebugMessage();
            }

            void BindTextureSlot(u32 slot)
            {
                ID3D11ShaderResourceView* views = nullptr;
                s_ImmContext->PSSetShaderResources(slot, 1, &views);
                LogDebugMessage();
            }

            void UnbindTexture(const sTexture& texture)
            {
                static ID3D11ShaderResourceView* nullInstance = nullptr;
                s_ImmContext->PSSetShaderResources(texture.Slot, 1, (ID3D11ShaderResourceView**)&nullInstance);
                LogDebugMessage();
            }

            void FreeTexture1D(void* instance)
            {
                ((ID3D11Texture1D*) instance)->Release();
                LogDebugMessage();
            }

            void FreeTexture2D(void* instance)
            {
                ((ID3D11Texture2D*) instance)->Release();
                LogDebugMessage();
            }

            void FreeTexture2DArray(void* instance)
            {
                ((ID3D11Texture2D*) instance)->Release();
                LogDebugMessage();
            }

            void FreeTexture3D(void* instance)
            {
                ((ID3D11Texture3D*) instance)->Release();
                LogDebugMessage();
            }

            void FreeTextureCube(void* instance)
            {
                ((ID3D11Texture2D*) instance)->Release();
                LogDebugMessage();
            }

            void FreeTextureDepthStencil(void* instance)
            {
                ((ID3D11Texture2D*) instance)->Release();
                LogDebugMessage();
            }

            void GenerateMips(const sTexture& texture) {
                u32 mipLevels = texture.GetMipLevels();
                if (mipLevels > 1) {
                    s_ImmContext->GenerateMips((ID3D11ShaderResourceView*) texture.ViewInstance);
                    LogDebugMessage();
                }
            }

            void CreateSampler(sSampler& sampler)
            {
                D3D11_SAMPLER_DESC desc = {};

                desc.Filter = k_SamplerFilterTable.at(sampler.Filter);

                desc.AddressU = k_SamplerAddressTable.at(sampler.AddressU);
                desc.AddressV = k_SamplerAddressTable.at(sampler.AddressV);
                desc.AddressW = k_SamplerAddressTable.at(sampler.AddressW);

                desc.ComparisonFunc = k_SamplerComparisonTable.at(sampler.Comparison);

                desc.MaxAnisotropy = sampler.AnisotropyLevel;
                desc.MinLOD = sampler.MinLOD;
                desc.MaxLOD = sampler.MaxLOD;
                desc.MipLODBias = sampler.MipLODBias;

                desc.BorderColor[0] = sampler.BorderColor[0];
                desc.BorderColor[1] = sampler.BorderColor[1];
                desc.BorderColor[2] = sampler.BorderColor[2];
                desc.BorderColor[3] = sampler.BorderColor[3];

                s_Device->CreateSamplerState(&desc, (ID3D11SamplerState**)&sampler.Instance);
                LogDebugMessage();
            }

            void BindSampler(const sSampler& sampler)
            {
                s_ImmContext->PSSetSamplers(sampler.Slot, 1, (ID3D11SamplerState**)&sampler.Instance);
                LogDebugMessage();
            }

            void UnbindSampler(const sSampler& sampler)
            {
                static ID3D11SamplerState* nullInstance = nullptr;
                s_ImmContext->PSSetSamplers(sampler.Slot, 1, (ID3D11SamplerState**)&nullInstance);
                LogDebugMessage();
            }

            void FreeSampler(sSampler& sampler)
            {
                if (sampler.Instance != nullptr)
                {
                    ((ID3D11SamplerState*)sampler.Instance)->Release();
                    LogDebugMessage();
                    sampler.Instance = nullptr;
                }
            }

            void CreateBuffer(sBuffer& buffer)
            {
                eBufferType bufferType = buffer.Type;
                usize byteSize = buffer.GetByteSize();

                D3D11_BUFFER_DESC bufferDesc = {};
                memset(&bufferDesc, 0, sizeof(D3D11_BUFFER_DESC));

                bufferDesc.ByteWidth = (UINT)byteSize;
                bufferDesc.Usage = k_BufferUsageTable.at(buffer.Usage);
                bufferDesc.CPUAccessFlags = k_BufferCpuFlagTable.at(buffer.Usage);

                D3D11_SUBRESOURCE_DATA* initialData = InitBufferData(buffer);

                if (bufferType == eBufferType::VERTEX)
                {
                    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
                }

                else if (bufferType == eBufferType::INDEX)
                {
                    bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
                }

                else if (bufferType == eBufferType::CONSTANT)
                {
                    bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
                }

                else if (bufferType == eBufferType::STRUCTURED)
                {
                    bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
                    bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
                    bufferDesc.StructureByteStride = buffer.StructureSize;
                }

                s_Device->CreateBuffer(&bufferDesc, initialData, (ID3D11Buffer**)&buffer.Instance);
                LogDebugMessage();

                if (bufferType == eBufferType::STRUCTURED)
                {
                    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
                    srvDesc.Format = DXGI_FORMAT_UNKNOWN;
                    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
                    srvDesc.BufferEx.FirstElement = 0;
                    srvDesc.BufferEx.NumElements = buffer.NumElements;

                    s_Device->CreateShaderResourceView((ID3D11Resource*)buffer.Instance, &srvDesc, (ID3D11ShaderResourceView**)&buffer.ViewInstance);
                    LogDebugMessage();
                }

                FreeInitialData(initialData);
            }

            void BindVertexBuffer(const sBuffer& buffer)
            {
                UINT stride = buffer.StructureSize;
                UINT offset = 0;
                s_ImmContext->IASetVertexBuffers(buffer.Slot, 1, (ID3D11Buffer**)&buffer.Instance, &stride, &offset);
                LogDebugMessage();
            }

            void BindIndexBuffer(const sBuffer& buffer)
            {
                // we can skip 16-bit index type
                // it's very rare that we will bind index buffer with index range [0, ~65555]
                DXGI_FORMAT format = DXGI_FORMAT_R32_UINT;
                s_ImmContext->IASetIndexBuffer((ID3D11Buffer*)buffer.Instance, format, 0);
                LogDebugMessage();
            }

            void BindVSBuffer(const sBuffer& buffer)
            {
                if (buffer.Type == eBufferType::STRUCTURED)
                {
                    s_ImmContext->VSSetShaderResources(buffer.Slot, 1, (ID3D11ShaderResourceView**)&buffer.ViewInstance);
                    LogDebugMessage();
                }
                else if (buffer.Type == eBufferType::CONSTANT)
                {
                    s_ImmContext->VSSetConstantBuffers(buffer.Slot, 1, (ID3D11Buffer**)&buffer.Instance);
                    LogDebugMessage();
                }
            }

            void UnbindVSBuffer(const sBuffer& buffer)
            {
                static ID3D11ShaderResourceView* nullInstance = nullptr;

                if (buffer.Type == eBufferType::STRUCTURED)
                {
                    s_ImmContext->VSSetShaderResources(buffer.Slot, 1, (ID3D11ShaderResourceView**)&nullInstance);
                    LogDebugMessage();
                }

                else if (buffer.Type == eBufferType::CONSTANT)
                {
                    s_ImmContext->VSSetConstantBuffers(buffer.Slot, 1, (ID3D11Buffer**)&nullInstance);
                    LogDebugMessage();
                }
            }

            void BindPSBuffer(const sBuffer& buffer)
            {
                if (buffer.Type == eBufferType::STRUCTURED)
                {
                    s_ImmContext->PSSetShaderResources(buffer.Slot, 1, (ID3D11ShaderResourceView**)&buffer.ViewInstance);
                    LogDebugMessage();
                }
                else if (buffer.Type == eBufferType::CONSTANT)
                {
                    s_ImmContext->PSSetConstantBuffers(buffer.Slot, 1, (ID3D11Buffer**)&buffer.Instance);
                    LogDebugMessage();
                }
            }

            void UnbindPSBuffer(const sBuffer& buffer)
            {
                static ID3D11ShaderResourceView* nullInstance = nullptr;

                if (buffer.Type == eBufferType::STRUCTURED)
                {
                    s_ImmContext->PSSetShaderResources(buffer.Slot, 1, (ID3D11ShaderResourceView**)&nullInstance);
                    LogDebugMessage();
                }

                else if (buffer.Type == eBufferType::CONSTANT)
                {
                    s_ImmContext->PSSetConstantBuffers(buffer.Slot, 1, (ID3D11Buffer**)&nullInstance);
                    LogDebugMessage();
                }
            }

            void* Map(
                    const sResource &resource,
                    u32 subresourceIndex,
                    eMapType mapType
            ) {
                D3D11_MAPPED_SUBRESOURCE mappedResource = {};

                s_ImmContext->Map((ID3D11Resource*) resource.Instance, subresourceIndex, k_MapTypeTable.at(mapType), 0, &mappedResource);
                LogDebugMessage();

                return mappedResource.pData;
            }

            void Unmap(const sResource& resource)
            {
                s_ImmContext->Unmap((ID3D11Resource*)resource.Instance, 0);
                LogDebugMessage();
            }

            void UpdateSubData(const sResource& resource, u32 subresourceIndex, const void* data, u32 rowPitch, u32 depthPitch)
            {
                s_ImmContext->UpdateSubresource((ID3D11Resource*) resource.Instance, subresourceIndex, nullptr, data, rowPitch, depthPitch);
                LogDebugMessage();
            }

            void FreeBuffer(sBuffer& buffer)
            {
                if (buffer.Instance != nullptr)
                {
                    ((ID3D11Buffer*)buffer.Instance)->Release();
                    LogDebugMessage();
                    buffer.Instance = nullptr;
                }
            }

            void CreateInputLayout(sInputLayout& inputLayout)
            {
                sVertexFormat& vertexFormat = inputLayout.Format;
                usize attributeCount = vertexFormat.Attributes.size();
                usize attributeOffset = 0;
                auto* attributes = main_allocT(D3D11_INPUT_ELEMENT_DESC, attributeCount);

                for (u32 i = 0 ; i < attributeCount ; i++)
                {
                    auto& attribute = vertexFormat.Attributes[i];
                    auto& dxAttribute = attributes[i];
                    dxAttribute = {};

                    dxAttribute.SemanticName = attribute.Name;
                    dxAttribute.Format = k_VertexFormatTable.at(attribute.Format);
                    dxAttribute.AlignedByteOffset = attributeOffset;
                    dxAttribute.InstanceDataStepRate = D3D11_INPUT_PER_VERTEX_DATA;

                    attributeOffset += attribute.ByteSize;
                }

                s_Device->CreateInputLayout(
                        attributes, attributeCount,
                        inputLayout.VertexBlob->ByteCode, inputLayout.VertexBlob->ByteCodeSize,
                        (ID3D11InputLayout**)&inputLayout.Layout
                );
                LogDebugMessage();

                main_free(attributes);
            }

            void BindInputLayout(const sInputLayout& inputLayout)
            {
                s_ImmContext->IASetInputLayout((ID3D11InputLayout*) inputLayout.Layout);
                LogDebugMessage();
            }

            void BindPrimitiveTopology(const ePrimitiveTopology &primitiveTopology) {
                s_ImmContext->IASetPrimitiveTopology(k_PrimitiveTopologyTable.at(primitiveTopology));
                LogDebugMessage();
            }

            void BindViewport(sViewport* viewport) {
                if (viewport == nullptr) {
                    UnbindViewport();
                    return;
                }

                D3D11_VIEWPORT* vs = sallocT(D3D11_VIEWPORT, 1);

                auto& v = vs[0];
                v = {};
                v.TopLeftX = viewport->Left;
                v.TopLeftY = viewport->Top;
                v.Width = viewport->Width;
                v.Height = viewport->Height;
                v.MinDepth = viewport->MinDepth;
                v.MaxDepth = viewport->MaxDepth;

                s_ImmContext->RSSetViewports(1, vs);
                LogDebugMessage();
            }

            void UnbindViewport()
            {
                s_ImmContext->RSSetViewports(0, nullptr);
                LogDebugMessage();
            }

            void FreeInputLayout(sInputLayout& inputLayout)
            {
                if (inputLayout.Layout != nullptr)
                {
                    ((ID3D11InputLayout*) inputLayout.Layout)->Release();
                    LogDebugMessage();
                    inputLayout.Layout = nullptr;
                }
            }

            void CreateDepthStencilMode(sDepthStencilMode &depthStencilMode)
            {
                D3D11_DEPTH_STENCIL_DESC desc = {};
                desc.DepthEnable = depthStencilMode.EnableDepth;
                desc.DepthWriteMask = k_DepthWriteMaskTable.at(depthStencilMode.DepthWriteMask);
                desc.DepthFunc = k_DepthStencilComparisonTable.at(depthStencilMode.DepthFunc);
                desc.StencilEnable = depthStencilMode.EnableStencil;
                desc.StencilReadMask = depthStencilMode.StencilReadMask;
                desc.StencilWriteMask = depthStencilMode.StencilWriteMask;

                D3D11_DEPTH_STENCILOP_DESC frontFaceDesc = {};
                frontFaceDesc.StencilFailOp = k_DepthStencilOpTable.at(depthStencilMode.FrontFace.StencilFailOp);
                frontFaceDesc.StencilDepthFailOp = k_DepthStencilOpTable.at(depthStencilMode.FrontFace.StencilDepthFailOp);
                frontFaceDesc.StencilPassOp = k_DepthStencilOpTable.at(depthStencilMode.FrontFace.StencilPassOp);
                frontFaceDesc.StencilFunc = k_DepthStencilComparisonTable.at(depthStencilMode.FrontFace.StencilFunc);

                D3D11_DEPTH_STENCILOP_DESC backFaceDesc = {};
                backFaceDesc.StencilFailOp = k_DepthStencilOpTable.at(depthStencilMode.BackFace.StencilFailOp);
                backFaceDesc.StencilDepthFailOp = k_DepthStencilOpTable.at(depthStencilMode.BackFace.StencilDepthFailOp);
                backFaceDesc.StencilPassOp = k_DepthStencilOpTable.at(depthStencilMode.BackFace.StencilPassOp);
                backFaceDesc.StencilFunc = k_DepthStencilComparisonTable.at(depthStencilMode.BackFace.StencilFunc);

                desc.FrontFace = frontFaceDesc;
                desc.BackFace = backFaceDesc;
                s_Device->CreateDepthStencilState(&desc, (ID3D11DepthStencilState**) &depthStencilMode.State);
                LogDebugMessage();
            }

            void BindDepthStencilMode(void* state)
            {
                s_ImmContext->OMSetDepthStencilState((ID3D11DepthStencilState*) state, 0);
                LogDebugMessage();
            }

            void FreeDepthStencilMode(sDepthStencilMode &depthStencilMode)
            {
                if (depthStencilMode.State != nullptr)
                {
                    ((ID3D11DepthStencilState*) depthStencilMode.State)->Release();
                    LogDebugMessage();
                    depthStencilMode.State = nullptr;
                }
            }

            void CreateBlendMode(sBlendMode &blendMode)
            {
                D3D11_BLEND_DESC desc = {};
                desc.AlphaToCoverageEnable = blendMode.AlphaToCoverageEnable;
                desc.IndependentBlendEnable = blendMode.IndependentBlendEnable;

                u32 blendTargetSize = blendMode.Targets.size();
                for (u32 i = 0 ; i < blendTargetSize ; i++)
                {
                    auto& blendTarget = blendMode.Targets[i];
                    desc.RenderTarget[i].BlendEnable = blendTarget.Enable;
                    desc.RenderTarget[i].SrcBlend = k_BlendTable.at(blendTarget.Src);
                    desc.RenderTarget[i].DestBlend = k_BlendTable.at(blendTarget.Dest);
                    desc.RenderTarget[i].BlendOp = k_BlendOpTable.at(blendTarget.BlendOp);
                    desc.RenderTarget[i].SrcBlendAlpha = k_BlendTable.at(blendTarget.SrcAlpha);
                    desc.RenderTarget[i].DestBlendAlpha = k_BlendTable.at(blendTarget.DestAlpha);
                    desc.RenderTarget[i].BlendOpAlpha = k_BlendOpTable.at(blendTarget.BlendOpAlpha);
                    desc.RenderTarget[i].RenderTargetWriteMask = blendTarget.RenderTargetWriteMask;
                }

                s_Device->CreateBlendState(&desc, (ID3D11BlendState**) &blendMode.State);
                LogDebugMessage();
            }

            void BindBlendMode(void* state)
            {
                s_ImmContext->OMSetBlendState((ID3D11BlendState*) state, nullptr, 0xffffffff);
                LogDebugMessage();
            }

            void FreeBlendMode(sBlendMode &blendMode)
            {
                if (blendMode.State != nullptr)
                {
                    ((ID3D11BlendState*)blendMode.State)->Release();
                    LogDebugMessage();
                    blendMode.State = nullptr;
                }
            }

            void CreateRasterizer(sRasterizer &rasterizer)
            {
                D3D11_RASTERIZER_DESC desc = {};

                desc.FillMode = k_FillModeTable.at(rasterizer.FillMode);
                desc.CullMode = k_CullModeTable.at(rasterizer.CullMode);
                desc.FrontCounterClockwise = rasterizer.FrontCounterClockwise;
                desc.DepthBias = rasterizer.DepthBias;
                desc.DepthBiasClamp = rasterizer.DepthBiasClamp;
                desc.SlopeScaledDepthBias = rasterizer.SlopeScaledDepthBias;
                desc.DepthClipEnable = rasterizer.DepthClipEnable;
                desc.ScissorEnable = rasterizer.ScissorEnable;
                desc.MultisampleEnable = rasterizer.MultisampleEnable;
                desc.AntialiasedLineEnable = rasterizer.AntialiasedLineEnable;

                s_Device->CreateRasterizerState(&desc, (ID3D11RasterizerState**) &rasterizer.Instance);
                LogDebugMessage();
            }

            void BindRasterizer(void* state)
            {
                s_ImmContext->RSSetState((ID3D11RasterizerState*) state);
                LogDebugMessage();
            }

            void FreeRasterizer(sRasterizer& rasterizer)
            {
                if (rasterizer.Instance != nullptr) {
                    ((ID3D11RasterizerState*) rasterizer.Instance)->Release();
                    LogDebugMessage();
                    rasterizer.Instance = nullptr;
                }
            }

            void DrawIndexed(usize indexCount, usize instanceCount, usize vertexOffset, usize indexOffset, usize instanceOffset)
            {
                s_ImmContext->DrawIndexedInstanced(indexCount, instanceCount, indexOffset, vertexOffset, instanceOffset);
                LogDebugMessage();
            }

            void DrawVertexed(usize vertexCount, usize instanceCount, usize vertexOffset, usize instanceOffset)
            {
                s_ImmContext->DrawInstanced(vertexCount, instanceCount, vertexOffset, instanceOffset);
                LogDebugMessage();
            }

            void DrawQuad()
            {
                s_ImmContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
                LogDebugMessage();

                s_ImmContext->Draw(4, 1);
                LogDebugMessage();
            }

            void* GetDevice()
            {
                return s_Device;
            }

        }

    }

}

#endif // DX11