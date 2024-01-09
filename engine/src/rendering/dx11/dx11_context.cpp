#include <d3dcompiler.h>
#include <d3d11.h>
#include <rendering/core/core.hpp>
#include <rendering/core/debugger.hpp>
#include <rendering/core/context.hpp>
#include <rendering/core/shader.hpp>

namespace xpe {

    namespace render {

        namespace context {

            using namespace core;

            static ID3D11Device* s_Device = nullptr;
            static ID3D11DeviceContext* s_ImmContext = nullptr;
            static ID3D11DeviceContext* s_DefContext = nullptr;
            static IDXGISwapChain* s_SwapChain = nullptr;
            static IDXGIDevice* s_GIDevice = nullptr;
            static IDXGIAdapter* s_GIAdapter = nullptr;
            static IDXGIFactory* s_GIFactory = nullptr;
            static DXGI_MODE_DESC s_GIModeDesc;

            static const std::unordered_map<cBuffer::eUsage, D3D11_USAGE> k_BufferUsageTable =
            {
                    { cBuffer::eUsage::DEFAULT, D3D11_USAGE_DEFAULT },
                    { cBuffer::eUsage::STATIC,  D3D11_USAGE_IMMUTABLE },
                    { cBuffer::eUsage::DYNAMIC, D3D11_USAGE_DYNAMIC },
                    { cBuffer::eUsage::STAGING, D3D11_USAGE_STAGING }
            };

            static const std::unordered_map<cBuffer::eUsage, u32> k_BufferUsageCpuFlagTable =
            {
                    { cBuffer::eUsage::DEFAULT, 0 },
                    { cBuffer::eUsage::STATIC, 0 },
                    { cBuffer::eUsage::DYNAMIC, D3D11_CPU_ACCESS_WRITE },
                    { cBuffer::eUsage::STAGING, D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE }
            };

            static const std::unordered_map<cBuffer::eType, u32> k_BufferTypeBindFlagTable =
            {
                    { cBuffer::eType::VERTEX, D3D11_BIND_VERTEX_BUFFER },
                    { cBuffer::eType::INDEX,  D3D11_BIND_INDEX_BUFFER },
                    { cBuffer::eType::CONSTANT,   D3D11_BIND_CONSTANT_BUFFER },
                    { cBuffer::eType::STRUCTURED,   D3D11_BIND_SHADER_RESOURCE },
                    { cBuffer::eType::RW,     D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS }
            };

            static const std::unordered_map<cBuffer::eType, u32> k_BufferTypeMiscFlagTable =
            {
                    { cBuffer::eType::VERTEX, 0 },
                    { cBuffer::eType::INDEX,  0 },
                    { cBuffer::eType::CONSTANT,   0 },
                    { cBuffer::eType::STRUCTURED,   D3D11_RESOURCE_MISC_BUFFER_STRUCTURED },
                    { cBuffer::eType::RW,     D3D11_RESOURCE_MISC_BUFFER_STRUCTURED }
            };

            static const std::unordered_map<eTextureFormat, DXGI_FORMAT> k_TextureFormatTable =
            {
                    { eTextureFormat::R8, DXGI_FORMAT_R8_UNORM },
                    { eTextureFormat::R16, DXGI_FORMAT_R16_UNORM },
                    { eTextureFormat::R32, DXGI_FORMAT_R32_FLOAT },
                    { eTextureFormat::R32_TYPELESS, DXGI_FORMAT_R32_TYPELESS },
                    { eTextureFormat::R16_TYPELESS, DXGI_FORMAT_R16_TYPELESS },

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

            static const std::unordered_map<cTexture::eUsage, D3D11_USAGE> k_TextureUsageTable =
            {
                    {cTexture::eUsage::DEFAULT, D3D11_USAGE_DEFAULT },
                    {cTexture::eUsage::STATIC,  D3D11_USAGE_IMMUTABLE },
                    {cTexture::eUsage::DYNAMIC, D3D11_USAGE_DYNAMIC },
                    {cTexture::eUsage::STAGING, D3D11_USAGE_STAGING }
            };

            static const std::unordered_map<cTexture::eUsage, u32> k_TextureCpuFlagTable =
            {
                    {cTexture::eUsage::DEFAULT, D3D11_CPU_ACCESS_WRITE },
                    {cTexture::eUsage::STATIC,  0 },
                    {cTexture::eUsage::DYNAMIC, D3D11_CPU_ACCESS_WRITE },
                    {cTexture::eUsage::STAGING, D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE }
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

            static const std::unordered_map<cResource::eMapType, D3D11_MAP> k_MapTypeTable =
            {
                    { cResource::eMapType::READ, D3D11_MAP_READ },
                    { cResource::eMapType::WRITE, D3D11_MAP_WRITE },
                    { cResource::eMapType::READ_WRITE, D3D11_MAP_READ_WRITE },
                    { cResource::eMapType::WRITE_NO_OVERWRITE, D3D11_MAP_WRITE_NO_OVERWRITE },
                    { cResource::eMapType::WRITE_DISCARD, D3D11_MAP_WRITE_DISCARD },
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

            static D3D11_SUBRESOURCE_DATA* InitBufferData(const cBuffer& buffer)
            {
                D3D11_SUBRESOURCE_DATA* initialData = nullptr;

                if (buffer.GetInitialData() != nullptr)
                {
                    usize bufferSize = buffer.GetByteSize();
                    initialData = main_allocT(D3D11_SUBRESOURCE_DATA, bufferSize);
                    initialData->pSysMem = buffer.GetInitialData();
                    initialData->SysMemPitch = bufferSize;
                    initialData->SysMemSlicePitch = 0;
                }

                return initialData;
            }

            static D3D11_SUBRESOURCE_DATA* InitTextureData(cTexture& texture, const u32 arraySize, const u32 mipLevels)
            {
                D3D11_SUBRESOURCE_DATA* initialData = nullptr;

                u32 initialDataSize = arraySize * mipLevels;

                if (texture.IsDataInitialized())
                {
                    initialData = main_allocT(D3D11_SUBRESOURCE_DATA, initialDataSize);

                    for (int i = 0; i < arraySize; i++)
                    {
                        auto& textureLayer = texture.GetLayers()[i];
                        int mipSize = textureLayer.Mips.size();
                        int k = i * (mipSize + 1);

                        auto& textureData = initialData[k];
                        textureData.pSysMem = textureLayer.Pixels;
                        textureData.SysMemPitch = textureLayer.Width * cTexture::k_BppTable.at(textureLayer.Format);
                        textureData.SysMemSlicePitch = 0;

                        for (int j = 0 ; j < mipSize ; j++)
                        {
                            auto& mip = textureLayer.Mips[j];
                            auto& mipData = initialData[++k];
                            mipData.pSysMem = mip.Pixels;
                            mipData.SysMemPitch = mip.Width * cTexture::k_BppTable.at(mip.Format);
                            mipData.SysMemSlicePitch = 0;
                        }
                    }
                }

                return initialData;
            }

            static void UpdateTextureFlags(cTexture& texture, u32& bindFlags, u32& miscFlags)
            {
                bindFlags = D3D11_BIND_SHADER_RESOURCE;

                if (texture.IsRenderTargetEnabled()) {
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

            void ResizeSwapchain(cRenderTarget& renderTarget, int width, int height)
            {
                UnbindRenderTarget();
                if (SwapchainTextureInstance) {
                    ((ID3D11Texture2D*) SwapchainTextureInstance)->Release();
                    SwapchainTextureInstance = nullptr;
                }
                FreeRenderTargetColorViews(renderTarget.GetColorViews());

                s_SwapChain->ResizeBuffers(1, 0, 0, DXGI_FORMAT_UNKNOWN, 0ul);
                LogDebugMessage();

                DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
                s_SwapChain->GetDesc(&swapChainDesc);
                LogDebugMessage();

                LogWarning("ResizeSwapchain: expected resolution=({}, {}); actual resolution=({}, {})", width, height, swapChainDesc.BufferDesc.Width, swapChainDesc.BufferDesc.Height);

                CreateSwapchainTargetView();

                renderTarget.GetColorViews()[0] = SwapchainTargetView;
                BindRenderTarget(renderTarget.GetColorViews(), renderTarget.GetDepthStencilView());
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

            void CreateRenderTarget(cRenderTarget& renderTarget)
            {
                u32 colorCount = renderTarget.GetColors().size();
                for (u32 i = 0; i < colorCount; i++)
                {
                    auto& color = renderTarget.GetColors()[i];
                    auto& colorView = renderTarget.GetColorViews()[i];

                    if (color && color->GetInstance() && color->GetWidth() != 0 && color->GetHeight() != 0 && colorView == nullptr)
                    {
                        D3D11_RENDER_TARGET_VIEW_DESC desc = {};
                        desc.Format = k_TextureFormatTable.at(color->GetFormat());
                        desc.ViewDimension = color->GetSampleCount() > 1 ? D3D11_RTV_DIMENSION_TEXTURE2DMS : D3D11_RTV_DIMENSION_TEXTURE2D;
                        desc.Texture2D.MipSlice = 0;

                        s_Device->CreateRenderTargetView(
                                (ID3D11Resource*) color->GetInstance(),
                                &desc,
                                (ID3D11RenderTargetView**) &colorView
                        );
                        LogDebugMessage();
                    }
                }

                auto depth = renderTarget.GetDepthStencilPtr();
                auto depthView = renderTarget.GetDepthStencilViewPtr();

                if ((*depth) && (*depth)->GetInstance() && (*depth)->GetWidth() != 0 && (*depth)->GetHeight() != 0 && (*depthView) == nullptr)
                {
                    D3D11_DEPTH_STENCIL_VIEW_DESC desc = {};
                    desc.Format = (*depth)->GetFormat() == eTextureFormat::R16_TYPELESS ? DXGI_FORMAT_D16_UNORM : DXGI_FORMAT_D32_FLOAT;
                    desc.ViewDimension = (*depth)->GetSampleCount() > 1 ? D3D11_DSV_DIMENSION_TEXTURE2DMS : D3D11_DSV_DIMENSION_TEXTURE2D;
                    desc.Texture2D.MipSlice = 0;

                    s_Device->CreateDepthStencilView(
                            (ID3D11Resource*)(*depth)->GetInstance(),
                            &desc,
                            (ID3D11DepthStencilView**)depthView
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

            void ResizeRenderTarget(cRenderTarget& renderTarget, int width, int height)
            {
                UnbindRenderTarget();
                FreeRenderTarget(renderTarget);
                CreateRenderTarget(renderTarget);
            }

            void Present()
            {
                s_SwapChain->Present(0, 0);
                LogDebugMessage();
            }

            void CompileShaderStage(sShaderStage* stage) {
                // skip stage that is already compiled
                if (stage->Compiled) {
                    return;
                }

                ID3DBlob* shaderBlob = nullptr;
                ID3DBlob* errorBlob = nullptr;

                D3DCompile(
                        stage->Source.c_str(),
                        stage->Source.length(),
                        nullptr,
                        nullptr,
                        nullptr,
                        stage->EntryPoint.c_str(),
                        stage->Profile.c_str(),
                        (UINT)stage->Flag,
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
                    stage->Compiled = true;
                    stage->Blob.Instance = shaderBlob;
                    stage->Blob.ByteCode = shaderBlob->GetBufferPointer();
                    stage->Blob.ByteCodeSize = shaderBlob->GetBufferSize();
                }
            }

            void CreateShaderStage(sShaderStage* stage)
            {
                sBlob blob = stage->Blob;

                if (blob.Instance != nullptr) {

                    const void* byteCode = blob.ByteCode;
                    const usize byteCodeSize = blob.ByteCodeSize;

                    switch (stage->Type) {

                        case sShaderStage::eType::VERTEX:
                            s_Device->CreateVertexShader(
                                    byteCode,
                                    byteCodeSize,
                                    nullptr,
                                    (ID3D11VertexShader**)stage->GetInstancePtr()
                            );
                            LogDebugMessage();
                            break;

                        case sShaderStage::eType::PIXEL:
                            s_Device->CreatePixelShader(
                                    byteCode,
                                    byteCodeSize,
                                    nullptr,
                                    (ID3D11PixelShader**)stage->GetInstancePtr()
                            );
                            LogDebugMessage();
                            break;

                        case sShaderStage::eType::GEOMETRY:
                            s_Device->CreateGeometryShader(
                                    byteCode,
                                    byteCodeSize,
                                    nullptr,
                                    (ID3D11GeometryShader**)stage->GetInstancePtr()
                            );
                            LogDebugMessage();
                            break;

                        case sShaderStage::eType::COMPUTE:
                            s_Device->CreateComputeShader(
                                    byteCode,
                                    byteCodeSize,
                                    nullptr,
                                    (ID3D11ComputeShader**)stage->GetInstancePtr()
                            );
                            LogDebugMessage();
                            break;

                    }
                }
            }

            void BindVSStage(const sShaderStage* stage)
            {
                s_ImmContext->VSSetShader((ID3D11VertexShader*) stage->GetInstance(), nullptr, 0);
                LogDebugMessage();

                for (auto& binding : stage->BufferBindings)
                {
                    cBuffer* buffer = (cBuffer*)binding.Resource;
                    VSBindBuffer(binding.ViewType, buffer->GetType(), binding.Slot, (void*)buffer->GetInstance(), (void*)binding.View);
                }

                for (auto& binding : stage->TextureBindings)
                {
                    cTexture* texture = (cTexture*)binding.Resource;
                    VSBindTexture(*texture, texture->GetSlot());
                }

                for (auto* sampler : stage->Samplers)
                {
                    VSBindSampler(*sampler);
                }
            }

            void BindPSStage(const sShaderStage* stage)
            {
                if (stage != nullptr)
                {
                    s_ImmContext->PSSetShader((ID3D11PixelShader*)stage->GetInstance(), nullptr, 0);
                    LogDebugMessage();

                    for (auto& binding : stage->BufferBindings)
                    {
                        cBuffer* buffer = (cBuffer*)binding.Resource;
                        PSBindBuffer(binding.ViewType, buffer->GetType(), binding.Slot, (void*)buffer->GetInstance(), (void*)binding.View);
                    }

                    for (auto& binding : stage->TextureBindings)
                    {
                        cTexture* texture = (cTexture*)binding.Resource;
                        PSBindTexture(binding.ViewType, binding.Slot, (void*)texture->GetInstance(), binding.View);
                    }

                    for (auto* sampler : stage->Samplers)
                    {
                        PSBindSampler(*sampler);
                    }
                }
                else
                {
                    s_ImmContext->PSSetShader(nullptr, nullptr, 0);
                    LogDebugMessage();
                }
            }

            void BindGSStage(const sShaderStage* stage)
            {
                s_ImmContext->GSSetShader((ID3D11GeometryShader*) stage->GetInstance(), nullptr, 0);
                LogDebugMessage();

                for (auto& binding : stage->BufferBindings)
                {
                    cBuffer* buffer = (cBuffer*)binding.Resource;
                    GSBindBuffer(binding.ViewType, buffer->GetType(), binding.Slot, (void*)buffer->GetInstance(), (void*)binding.View);
                }

                for (auto& binding : stage->TextureBindings)
                {
                    cTexture* texture = (cTexture*)binding.Resource;
                    GSBindTexture(*texture);
                }

                for (auto* sampler : stage->Samplers)
                {
                    GSBindSampler(*sampler);
                }
            }

            void BindCSStage(const sShaderStage* stage)
            {
                s_ImmContext->CSSetShader((ID3D11ComputeShader*) stage->GetInstance(), nullptr, 0);
                LogDebugMessage();

                for (auto& binding : stage->BufferBindings)
                {
                    cBuffer* buffer = (cBuffer*)binding.Resource;
                    CSBindBuffer(binding.ViewType, buffer->GetType(), binding.Slot, (void*)buffer->GetInstance(), (void*)binding.View);
                }

                for (auto& binding : stage->TextureBindings)
                {
                    cTexture* texture = (cTexture*)binding.Resource;
                    CSBindTexture(*texture);
                }

                for (auto* sampler : stage->Samplers)
                {
                    CSBindSampler(*sampler);
                }
            }

            void UnbindVSStage(const sShaderStage* stage)
            {
                for (auto& binding : stage->BufferBindings)
                {
                    cBuffer* buffer = (cBuffer*)binding.Resource;
                    VSBindBuffer(binding.ViewType, buffer->GetType(), binding.Slot, nullptr, nullptr);
                }

                for (auto& binding : stage->TextureBindings)
                {
                    cTexture* texture = (cTexture*)binding.Resource;
                    VSUnbindTexture(*texture);
                }

                for (auto* sampler : stage->Samplers)
                {
                    VSUnbindSampler(*sampler);
                }
            }

            void UnbindPSStage(const sShaderStage* stage)
            {
                for (auto& binding : stage->BufferBindings)
                {
                    cBuffer* buffer = (cBuffer*)binding.Resource;
                    PSBindBuffer(binding.ViewType, buffer->GetType(), binding.Slot, nullptr, nullptr);
                }

                for (auto& binding : stage->TextureBindings)
                {
                    cTexture* texture = (cTexture*)binding.Resource;
                    PSBindTexture(binding.ViewType, binding.Slot, nullptr, nullptr);
                }

                for (auto* sampler : stage->Samplers)
                {
                    PSUnbindSampler(*sampler);
                }
            }

            void UnbindGSStage(const sShaderStage* stage)
            {
                for (auto& binding : stage->BufferBindings)
                {
                    cBuffer* buffer = (cBuffer*)binding.Resource;
                    GSBindBuffer(binding.ViewType, buffer->GetType(), binding.Slot, nullptr, nullptr);
                }

                for (auto& binding : stage->TextureBindings)
                {
                    cTexture* texture = (cTexture*)binding.Resource;
                    GSUnbindTexture(*texture);
                }

                for (auto* sampler : stage->Samplers)
                {
                    GSUnbindSampler(*sampler);
                }
            }

            void UnbindCSStage(const sShaderStage* stage)
            {
                for (auto& binding : stage->BufferBindings)
                {
                    cBuffer* buffer = (cBuffer*)binding.Resource;
                    CSBindBuffer(binding.ViewType, buffer->GetType(), binding.Slot, nullptr, nullptr);
                }

                for (auto& binding : stage->TextureBindings)
                {
                    cTexture* texture = (cTexture*)binding.Resource;
                    CSUnbindTexture(*texture);
                }

                for (auto* sampler : stage->Samplers)
                {
                    CSUnbindSampler(*sampler);
                }
            }

            void FreeShaderStage(sShaderStage* stage)
            {
                if (stage->Blob.Instance != nullptr) {
                    ((ID3DBlob*) stage->Blob.Instance)->Release();
                    LogDebugMessage();
                    stage->Blob.Instance = nullptr;
                }

                if (stage->GetInstance() != nullptr) {
                    switch (stage->Type) {
                        case sShaderStage::eType::VERTEX:
                            ((ID3D11VertexShader*) stage->GetInstance())->Release();
                            LogDebugMessage();
                            break;

                        case sShaderStage::eType::PIXEL:
                            ((ID3D11PixelShader*) stage->GetInstance())->Release();
                            LogDebugMessage();
                            break;

                        case sShaderStage::eType::GEOMETRY:
                            ((ID3D11GeometryShader*) stage->GetInstance())->Release();
                            LogDebugMessage();
                            break;

                        case sShaderStage::eType::COMPUTE:
                            ((ID3D11ComputeShader*) stage->GetInstance())->Release();
                            LogDebugMessage();
                            break;
                    }
                    //stage.GetInstance() = nullptr;
                }
            }

            void FreeShaderResourceView(void* viewInstance)
            {
                ((ID3D11ShaderResourceView*)viewInstance)->Release();
                LogDebugMessage();
            }

            void FreeUnorderedAccessView(void* viewInstance)
            {
                ((ID3D11UnorderedAccessView*)viewInstance)->Release();
                LogDebugMessage();
            }

            void CreateTexture1D(cTexture &texture)
            {
                u32 arraySize = texture.GetLayers().empty() ? 1 : texture.GetLayers().size();
                u32 mipLevels = texture.GetMipLevels();
                D3D11_SUBRESOURCE_DATA* initialData = InitTextureData(texture, arraySize, mipLevels);
                D3D11_TEXTURE1D_DESC texDesc = {};
                UpdateTextureFlags(texture, texDesc.BindFlags, texDesc.MiscFlags);
                texDesc.Width = texture.GetWidth();
                texDesc.MipLevels = mipLevels;
                texDesc.ArraySize = arraySize;
                texDesc.Format = k_TextureFormatTable.at(texture.GetFormat());
                texDesc.Usage = k_TextureUsageTable.at(texture.GetUsage());
                texDesc.CPUAccessFlags = k_TextureCpuFlagTable.at(texture.GetUsage());

                s_Device->CreateTexture1D(&texDesc, initialData, (ID3D11Texture1D**) texture.GetInstancePtr());
                LogDebugMessage();

                if (texture.GetViewType() == cTexture::eViewType::SRV)
                {
                    D3D11_SHADER_RESOURCE_VIEW_DESC srv = {};
                    srv.Format = texDesc.Format;
                    srv.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1D;
                    srv.Texture1D.MostDetailedMip = texture.GetMostDetailedMip();
                    srv.Texture1D.MipLevels = mipLevels;

                    s_Device->CreateShaderResourceView(
                            (ID3D11Texture1D*) texture.GetInstance(),
                            &srv,
                            (ID3D11ShaderResourceView**) texture.GetSRVInstancePtr()
                    );
                    LogDebugMessage();
                }
                else if (texture.GetViewType() == cTexture::eViewType::UAV)
                {
                    D3D11_UNORDERED_ACCESS_VIEW_DESC uav = {};
                    uav.Format = texDesc.Format;
                    uav.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE1D;

                    s_Device->CreateUnorderedAccessView(
                            (ID3D11Texture1D*) texture.GetInstance(),
                            &uav,
                            (ID3D11UnorderedAccessView**) texture.GetUAVInstancePtr()
                    );
                    LogDebugMessage();
                }

                GenerateMips(texture);

                FreeInitialData(initialData);
            }

            void CreateTexture2D(cTexture &texture)
            {
                u32 arraySize = texture.GetLayers().empty() ? 1 : texture.GetLayers().size();
                u32 mipLevels = texture.GetMipLevels();
                D3D11_SUBRESOURCE_DATA* initialData = InitTextureData(texture, arraySize, mipLevels);
                D3D11_TEXTURE2D_DESC desc = {};
                UpdateTextureFlags(texture, desc.BindFlags, desc.MiscFlags);
                desc.Width = texture.GetWidth();
                desc.Height = texture.GetHeight();
                desc.MipLevels = mipLevels;
                desc.ArraySize = arraySize;
                desc.Format = k_TextureFormatTable.at(texture.GetFormat());
                desc.SampleDesc.Count = texture.GetSampleCount();
                desc.SampleDesc.Quality = 0;
                desc.Usage = k_TextureUsageTable.at(texture.GetUsage());
                desc.CPUAccessFlags = texture.GetSampleCount() == 1 ? k_TextureCpuFlagTable.at(texture.GetUsage()) : 0;

                s_Device->CreateTexture2D(&desc, initialData, (ID3D11Texture2D**)texture.GetInstancePtr());
                LogDebugMessage();

                if (texture.GetViewType() == cTexture::eViewType::SRV)
                {
                    D3D11_SHADER_RESOURCE_VIEW_DESC srv = {};
                    srv.Format = desc.Format;
                    srv.ViewDimension = texture.GetSampleCount() > 1 ? D3D11_SRV_DIMENSION_TEXTURE2DMS : D3D11_SRV_DIMENSION_TEXTURE2D;
                    srv.Texture2D.MostDetailedMip = texture.GetMostDetailedMip();
                    srv.Texture2D.MipLevels = mipLevels;

                    s_Device->CreateShaderResourceView(
                            (ID3D11Texture2D*)texture.GetInstance(),
                            &srv,
                            (ID3D11ShaderResourceView**)texture.GetSRVInstancePtr()
                    );
                    LogDebugMessage();
                }
                else if (texture.GetViewType() == cTexture::eViewType::UAV)
                {
                    D3D11_UNORDERED_ACCESS_VIEW_DESC uav = {};
                    uav.Format = desc.Format;
                    uav.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;

                    s_Device->CreateUnorderedAccessView(
                            (ID3D11Texture2D*)texture.GetInstance(),
                            &uav,
                            (ID3D11UnorderedAccessView**)texture.GetUAVInstancePtr()
                    );
                    LogDebugMessage();
                }

                GenerateMips(texture);

                FreeInitialData(initialData);
            }

            void CreateTexture2DArray(cTexture &texture)
            {
                u32 arraySize = texture.GetLayers().empty() ? 1 : texture.GetLayers().size();
                u32 mipLevels = texture.GetMipLevels();
                D3D11_SUBRESOURCE_DATA* initialData = InitTextureData(texture, arraySize, mipLevels);
                D3D11_TEXTURE2D_DESC texDesc = {};
                UpdateTextureFlags(texture, texDesc.BindFlags, texDesc.MiscFlags);
                texDesc.Width = texture.GetWidth();
                texDesc.Height = texture.GetHeight();
                texDesc.MipLevels = mipLevels;
                texDesc.ArraySize = arraySize;
                texDesc.Format = k_TextureFormatTable.at(texture.GetFormat());
                texDesc.SampleDesc.Count = 1;
                texDesc.SampleDesc.Quality = 0;
                texDesc.Usage = k_TextureUsageTable.at(texture.GetUsage());
                texDesc.CPUAccessFlags = k_TextureCpuFlagTable.at(texture.GetUsage());

                s_Device->CreateTexture2D(&texDesc, initialData, (ID3D11Texture2D**)texture.GetInstancePtr());
                LogDebugMessage();

                if (texture.GetViewType() == cTexture::eViewType::SRV)
                {
                    D3D11_SHADER_RESOURCE_VIEW_DESC srv = {};
                    srv.Format = texDesc.Format;
                    srv.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
                    srv.Texture2DArray.MostDetailedMip = texture.GetMostDetailedMip();
                    srv.Texture2DArray.MipLevels = mipLevels;
                    srv.Texture2DArray.FirstArraySlice = 0;
                    srv.Texture2DArray.ArraySize = arraySize;
                    s_Device->CreateShaderResourceView(
                            (ID3D11Texture2D*)texture.GetInstance(),
                            &srv,
                            (ID3D11ShaderResourceView**)texture.GetSRVInstancePtr()
                    );
                    LogDebugMessage();
                }
                else if (texture.GetViewType() == cTexture::eViewType::UAV)
                {
                    D3D11_UNORDERED_ACCESS_VIEW_DESC uav = {};
                    uav.Format = texDesc.Format;
                    uav.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2DARRAY;
                    uav.Texture2DArray.FirstArraySlice = 0;
                    uav.Texture2DArray.ArraySize = arraySize;
                    s_Device->CreateUnorderedAccessView(
                            (ID3D11Texture2D*)texture.GetInstance(),
                            &uav,
                            (ID3D11UnorderedAccessView**)texture.GetUAVInstancePtr()
                    );
                    LogDebugMessage();
                }

                GenerateMips(texture);

                FreeInitialData(initialData);
            }

            void CreateTexture3D(cTexture &texture)
            {
                u32 arraySize = texture.GetLayers().empty() ? 1 : texture.GetLayers().size();
                u32 mipLevels = texture.GetMipLevels();
                D3D11_SUBRESOURCE_DATA* initialData = InitTextureData(texture, arraySize, mipLevels);
                D3D11_TEXTURE3D_DESC texDesc = {};
                UpdateTextureFlags(texture, texDesc.BindFlags, texDesc.MiscFlags);
                texDesc.Width = texture.GetWidth();
                texDesc.Height = texture.GetHeight();
                texDesc.Depth = texture.GetDepth();
                texDesc.MipLevels = mipLevels;
                texDesc.Format = k_TextureFormatTable.at(texture.GetFormat());
                texDesc.Usage = k_TextureUsageTable.at(texture.GetUsage());
                texDesc.CPUAccessFlags = k_TextureCpuFlagTable.at(texture.GetUsage());

                s_Device->CreateTexture3D(&texDesc, initialData, (ID3D11Texture3D**)texture.GetInstancePtr());
                LogDebugMessage();

                if (texture.GetViewType() == cTexture::eViewType::SRV)
                {
                    D3D11_SHADER_RESOURCE_VIEW_DESC srv = {};
                    srv.Format = texDesc.Format;
                    srv.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
                    srv.Texture3D.MostDetailedMip = texture.GetMostDetailedMip();
                    srv.Texture3D.MipLevels = mipLevels;

                    s_Device->CreateShaderResourceView(
                            (ID3D11Texture3D*)texture.GetInstance(),
                            &srv,
                            (ID3D11ShaderResourceView**)texture.GetSRVInstancePtr()
                    );
                    LogDebugMessage();
                }
                else if (texture.GetViewType() == cTexture::eViewType::UAV)
                {
                    D3D11_UNORDERED_ACCESS_VIEW_DESC uav = {};
                    uav.Format = texDesc.Format;
                    uav.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE3D;

                    s_Device->CreateUnorderedAccessView(
                            (ID3D11Texture3D*)texture.GetInstance(),
                            &uav,
                            (ID3D11UnorderedAccessView**)texture.GetUAVInstancePtr()
                    );
                    LogDebugMessage();
                }

                GenerateMips(texture);

                FreeInitialData(initialData);
            }

            void CreateTextureCube(cTexture &texture)
            {
                u32 arraySize = 6;
                u32 mipLevels = texture.GetMipLevels();
                D3D11_SUBRESOURCE_DATA* initialData = InitTextureData(texture, arraySize, mipLevels);
                D3D11_TEXTURE2D_DESC texDesc = {};
                UpdateTextureFlags(texture, texDesc.BindFlags, texDesc.MiscFlags);
                texDesc.Width = texture.GetWidth();
                texDesc.Height = texture.GetHeight();
                texDesc.MipLevels = mipLevels;
                texDesc.ArraySize = arraySize;
                texDesc.Format = k_TextureFormatTable.at(texture.GetFormat());
                texDesc.SampleDesc.Count = 1;
                texDesc.SampleDesc.Quality = 0;
                texDesc.Usage = k_TextureUsageTable.at(texture.GetUsage());
                texDesc.CPUAccessFlags = k_TextureCpuFlagTable.at(texture.GetUsage());
                texDesc.MiscFlags |= D3D11_RESOURCE_MISC_TEXTURECUBE;

                s_Device->CreateTexture2D(&texDesc, initialData, (ID3D11Texture2D**)texture.GetInstancePtr());
                LogDebugMessage();

                if (texture.GetViewType() == cTexture::eViewType::SRV)
                {
                    D3D11_SHADER_RESOURCE_VIEW_DESC srv = {};
                    srv.Format = texDesc.Format;
                    srv.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
                    srv.TextureCube.MostDetailedMip = texture.GetMostDetailedMip();
                    srv.TextureCube.MipLevels = mipLevels;

                    s_Device->CreateShaderResourceView(
                            (ID3D11Texture2D*)texture.GetInstance(),
                            &srv,
                            (ID3D11ShaderResourceView**)texture.GetSRVInstancePtr()
                    );
                    LogDebugMessage();
                }

                GenerateMips(texture);

                FreeInitialData(initialData);
            }

            void CreateTextureDepthStencil(cTexture &texture)
            {
                D3D11_TEXTURE2D_DESC texDesc = {};
                texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
                texDesc.Format = k_TextureFormatTable.at(texture.GetFormat());
                texDesc.Width = texture.GetWidth();
                texDesc.Height = texture.GetHeight();
                texDesc.ArraySize = 1;
                texDesc.SampleDesc.Count = texture.GetSampleCount();
                texDesc.SampleDesc.Quality = 0;
                texDesc.MipLevels = 1;
                texDesc.Usage = k_TextureUsageTable.at(texture.GetUsage());
                texDesc.CPUAccessFlags = 0;

                s_Device->CreateTexture2D(&texDesc, nullptr, (ID3D11Texture2D**)texture.GetInstancePtr());
                LogDebugMessage();

                if (texture.GetViewType() == cTexture::eViewType::SRV)
                {
                    D3D11_SHADER_RESOURCE_VIEW_DESC srv = {};
                    srv.Format = texture.GetFormat() == eTextureFormat::R16_TYPELESS ? DXGI_FORMAT_R16_FLOAT : DXGI_FORMAT_R32_FLOAT;
                    srv.ViewDimension = texture.GetSampleCount() == 1 ? D3D11_SRV_DIMENSION_TEXTURE2D : D3D11_SRV_DIMENSION_TEXTURE2DMS;
                    srv.Texture2D.MostDetailedMip = texture.GetMostDetailedMip();
                    srv.Texture2D.MipLevels = 1;

                    s_Device->CreateShaderResourceView(
                            (ID3D11Texture2D*)texture.GetInstance(),
                            &srv,
                            (ID3D11ShaderResourceView**)texture.GetSRVInstancePtr()
                    );
                    LogDebugMessage();
                }
                else if (texture.GetViewType() == cTexture::eViewType::UAV)
                {
                    D3D11_UNORDERED_ACCESS_VIEW_DESC uav = {};
                    uav.Format = texture.GetFormat() == eTextureFormat::R16_TYPELESS ? DXGI_FORMAT_R16_FLOAT : DXGI_FORMAT_R32_FLOAT;
                    uav.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;

                    s_Device->CreateUnorderedAccessView(
                            (ID3D11Texture2D*)texture.GetInstance(),
                            &uav,
                            (ID3D11UnorderedAccessView**)texture.GetUAVInstancePtr()
                    );
                    LogDebugMessage();
                }
            }

            void VSBindTexture(cTexture::eViewType viewType, u32 slot, void* viewInstance)
            {
                if (viewType == cTexture::eViewType::SRV)
                {
                    s_ImmContext->VSSetShaderResources(slot, 1, (ID3D11ShaderResourceView**)&viewInstance);
                    LogDebugMessage();
                }
            }

            void GSBindTexture(cTexture::eViewType viewType, u32 slot, void* viewInstance)
            {
                if (viewType == cTexture::eViewType::SRV)
                {
                    s_ImmContext->GSSetShaderResources(slot, 1, (ID3D11ShaderResourceView**)&viewInstance);
                    LogDebugMessage();
                }
            }

            void CSBindTexture(cTexture::eViewType viewType, u32 slot, void* viewInstance)
            {
                if (viewType == cTexture::eViewType::SRV)
                {
                    s_ImmContext->CSSetShaderResources(slot, 1, (ID3D11ShaderResourceView**)&viewInstance);
                    LogDebugMessage();
                }
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

            void GenerateMips(const cTexture& texture) {
                u32 mipLevels = texture.GetMipLevels();
                if (mipLevels > 1) {
                    s_ImmContext->GenerateMips((ID3D11ShaderResourceView*)texture.GetSRVInstance());
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

                s_Device->CreateSamplerState(&desc, (ID3D11SamplerState**)sampler.GetInstancePtr());
                LogDebugMessage();
            }

            void VSBindSampler(u32 slot, void* viewInstance)
            {
                s_ImmContext->VSSetSamplers(slot, 1, (ID3D11SamplerState**)&viewInstance);
                LogDebugMessage();
            }

            void PSBindSampler(u32 slot, void* viewInstance)
            {
                s_ImmContext->PSSetSamplers(slot, 1, (ID3D11SamplerState**)&viewInstance);
                LogDebugMessage();
            }

            void GSBindSampler(u32 slot, void* viewInstance)
            {
                s_ImmContext->GSSetSamplers(slot, 1, (ID3D11SamplerState**)&viewInstance);
                LogDebugMessage();
            }

            void CSBindSampler(u32 slot, void* viewInstance)
            {
                s_ImmContext->CSSetSamplers(slot, 1, (ID3D11SamplerState**)&viewInstance);
                LogDebugMessage();
            }

            void FreeSampler(sSampler& sampler)
            {
                if (sampler.GetInstance() != nullptr)
                {
                    ((ID3D11SamplerState*)sampler.GetInstance())->Release();
                    LogDebugMessage();
                    //sampler.GetInstance() = nullptr;
                }
            }

            void CreateBuffer(cBuffer& buffer)
            {
                cBuffer::eType bufferType = buffer.GetType();
                cBuffer::eUsage bufferUsage = buffer.GetUsage();
                cResource::eViewType bufferViewType = buffer.GetViewType();
                usize byteSize = buffer.GetByteSize();
                usize structureSize = buffer.GetStructureSize();

                D3D11_SUBRESOURCE_DATA* initialData = InitBufferData(buffer);
                D3D11_BUFFER_DESC bufferDesc = {};
                memset(&bufferDesc, 0, sizeof(D3D11_BUFFER_DESC));
                bufferDesc.ByteWidth = (UINT)byteSize;
                bufferDesc.StructureByteStride = (UINT)structureSize;
                bufferDesc.Usage = k_BufferUsageTable.at(bufferUsage);
                bufferDesc.CPUAccessFlags = k_BufferUsageCpuFlagTable.at(bufferUsage);
                bufferDesc.BindFlags = k_BufferTypeBindFlagTable.at(bufferType);
                bufferDesc.MiscFlags = k_BufferTypeMiscFlagTable.at(bufferType);

                s_Device->CreateBuffer(&bufferDesc, initialData, (ID3D11Buffer**)buffer.GetInstancePtr());
                LogDebugMessage();

                if (bufferViewType == cBuffer::eViewType::SRV)
                {
                    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
                    srvDesc.Format = DXGI_FORMAT_UNKNOWN;
                    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
                    srvDesc.BufferEx.FirstElement = 0;
                    srvDesc.BufferEx.NumElements = buffer.GetNumElements();

                    s_Device->CreateShaderResourceView(
                            (ID3D11Resource*)buffer.GetInstance(),
                            &srvDesc,
                            (ID3D11ShaderResourceView**)buffer.GetSRVInstancePtr()
                    );
                    LogDebugMessage();
                }
                else if (bufferViewType == cBuffer::eViewType::UAV)
                {
                    D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
                    uavDesc.Format = DXGI_FORMAT_UNKNOWN;
                    uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
                    uavDesc.Buffer.Flags = 0;
                    uavDesc.Buffer.FirstElement = 0;
                    uavDesc.Buffer.NumElements = buffer.GetNumElements();

                    s_Device->CreateUnorderedAccessView(
                            (ID3D11Resource*)buffer.GetInstance(),
                            &uavDesc,
                            (ID3D11UnorderedAccessView**)buffer.GetUAVInstancePtr()
                    );
                    LogDebugMessage();
                }
                else if (bufferViewType == cBuffer::eViewType::SRV_UAV)
                {
                    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
                    srvDesc.Format = DXGI_FORMAT_UNKNOWN;
                    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
                    srvDesc.Buffer.ElementWidth = buffer.GetNumElements();

                    s_Device->CreateShaderResourceView(
                        (ID3D11Resource*)buffer.GetInstance(),
                        &srvDesc,
                        (ID3D11ShaderResourceView**)buffer.GetSRVInstancePtr()
                    );
                    LogDebugMessage();

                    D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
                    uavDesc.Format = DXGI_FORMAT_UNKNOWN;
                    uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
                    uavDesc.Buffer.NumElements = buffer.GetNumElements();

                    s_Device->CreateUnorderedAccessView(
                        (ID3D11Resource*)buffer.GetInstance(),
                        &uavDesc,
                        (ID3D11UnorderedAccessView**)buffer.GetUAVInstancePtr()
                    );
                    LogDebugMessage();
                }

                FreeInitialData(initialData);
            }

            void UnbindListBufferVS(const cBuffer &buffer)
            {
                static ID3D11ShaderResourceView* nullInstance = nullptr;
                s_ImmContext->VSSetShaderResources(buffer.GetSlot(), 1, (ID3D11ShaderResourceView**)&nullInstance);
                LogDebugMessage();
            }

            void BindListBufferGS(cBuffer &buffer)
            {
                s_ImmContext->GSSetShaderResources(buffer.GetSlot(), 1, (ID3D11ShaderResourceView**)buffer.GetSRVInstancePtr());
                LogDebugMessage();
            }

            void UnbindListBufferGS(const cBuffer &buffer)
            {
                static ID3D11ShaderResourceView* nullInstance = nullptr;
                s_ImmContext->GSSetShaderResources(buffer.GetSlot(), 1, (ID3D11ShaderResourceView**)&nullInstance);
                LogDebugMessage();
            }

            void BindListBufferPS(cBuffer &buffer)
            {
                s_ImmContext->PSSetShaderResources(buffer.GetSlot(), 1, (ID3D11ShaderResourceView**)buffer.GetSRVInstancePtr());
                LogDebugMessage();
            }

            void UnbindListBufferPS(const cBuffer &buffer)
            {
                static ID3D11ShaderResourceView* nullInstance = nullptr;
                s_ImmContext->PSSetShaderResources(buffer.GetSlot(), 1, (ID3D11ShaderResourceView**)&nullInstance);
                LogDebugMessage();
            }

            void BindListBufferCS(cBuffer &buffer)
            {
                s_ImmContext->CSSetShaderResources(buffer.GetSlot(), 1, (ID3D11ShaderResourceView**)buffer.GetSRVInstancePtr());
                LogDebugMessage();
            }

            void UnbindListBufferCS(const cBuffer &buffer)
            {
                static ID3D11ShaderResourceView* nullInstance = nullptr;
                s_ImmContext->CSSetShaderResources(buffer.GetSlot(), 1, (ID3D11ShaderResourceView**)&nullInstance);
                LogDebugMessage();
            }

            void BindItemBufferVS(const cBuffer &buffer)
            {

            }

            void UnbindItemBufferVS(const cBuffer &buffer)
            {
                static ID3D11ShaderResourceView* nullInstance = nullptr;
                s_ImmContext->VSSetConstantBuffers(buffer.GetSlot(), 1, (ID3D11Buffer**)&nullInstance);
                LogDebugMessage();
            }

            void BindItemBufferGS(cBuffer &buffer)
            {
                s_ImmContext->GSSetConstantBuffers(buffer.GetSlot(), 1, (ID3D11Buffer**)buffer.GetInstancePtr());
                LogDebugMessage();
            }

            void UnbindItemBufferGS(const cBuffer &buffer)
            {
                static ID3D11ShaderResourceView* nullInstance = nullptr;
                s_ImmContext->GSSetConstantBuffers(buffer.GetSlot(), 1, (ID3D11Buffer**)&nullInstance);
                LogDebugMessage();
            }

            void BindItemBufferPS(cBuffer &buffer)
            {
                s_ImmContext->PSSetConstantBuffers(buffer.GetSlot(), 1, (ID3D11Buffer**)buffer.GetInstancePtr());
                LogDebugMessage();
            }

            void UnbindItemBufferPS(const cBuffer &buffer)
            {
                static ID3D11ShaderResourceView* nullInstance = nullptr;
                s_ImmContext->PSSetConstantBuffers(buffer.GetSlot(), 1, (ID3D11Buffer**)&nullInstance);
                LogDebugMessage();
            }

            void BindItemBufferCS(cBuffer &buffer)
            {
                s_ImmContext->CSSetConstantBuffers(buffer.GetSlot(), 1, (ID3D11Buffer**)buffer.GetInstancePtr());
                LogDebugMessage();
            }

            void UnbindItemBufferCS(const cBuffer &buffer)
            {
                static ID3D11ShaderResourceView* nullInstance = nullptr;
                s_ImmContext->CSSetConstantBuffers(buffer.GetSlot(), 1, (ID3D11Buffer**)&nullInstance);
                LogDebugMessage();
            }

            void* Map(
                    const cResource &resource,
                    u32 subresourceIndex,
                    cResource::eMapType mapType
            ) {
                D3D11_MAPPED_SUBRESOURCE mappedResource = {};

                s_ImmContext->Map((ID3D11Resource*) resource.GetInstance(), subresourceIndex, k_MapTypeTable.at(mapType), 0, &mappedResource);
                LogDebugMessage();

                return mappedResource.pData;
            }

            void Unmap(const cResource& resource)
            {
                s_ImmContext->Unmap((ID3D11Resource*)resource.GetInstance(), 0);
                LogDebugMessage();
            }

            void UpdateSubData(const cResource& resource, u32 subresourceIndex, const void* data, u32 rowPitch, u32 depthPitch)
            {
                s_ImmContext->UpdateSubresource((ID3D11Resource*) resource.GetInstance(), subresourceIndex, nullptr, data, rowPitch, depthPitch);
                LogDebugMessage();
            }

            void FreeBuffer(cBuffer& buffer)
            {
                if (buffer.GetInstance() != nullptr)
                {
                    ((ID3D11Buffer*)buffer.GetInstance())->Release();
                    LogDebugMessage();
                    //buffer.GetInstance() = nullptr;
                }
            }

            void BindVertexBuffer(cBuffer &buffer)
            {
                if (buffer.GetViewType() == cBuffer::eViewType::NONE) {
                    UINT stride = buffer.GetStructureSize();
                    UINT offset = 0;
                    s_ImmContext->IASetVertexBuffers(buffer.GetSlot(), 1, (ID3D11Buffer**)buffer.GetInstancePtr(), &stride, &offset);
                    LogDebugMessage();
                }
                else if (buffer.GetViewType() == cBuffer::eViewType::SRV) {
                    s_ImmContext->VSSetShaderResources(buffer.GetSlot(), 1, (ID3D11ShaderResourceView**)buffer.GetSRVInstancePtr());
                    LogDebugMessage();
                }
            }

            void BindIndexBuffer(cBuffer &buffer)
            {
                if (buffer.GetViewType() == cBuffer::eViewType::NONE)
                {
                    // we can skip 16-bit index type
                    // it's very rare that we will bind index buffer with index range [0, 65536]
                    DXGI_FORMAT format = DXGI_FORMAT_R32_UINT;
                    s_ImmContext->IASetIndexBuffer((ID3D11Buffer*)buffer.GetInstance(), format, 0);
                    LogDebugMessage();
                }
                else if (buffer.GetViewType() == cBuffer::eViewType::SRV) 
                {
                    s_ImmContext->VSSetShaderResources(buffer.GetSlot(), 1, (ID3D11ShaderResourceView**)buffer.GetSRVInstancePtr());
                    LogDebugMessage();
                }
            }

            void VSBindBuffer(const cResource::eViewType& viewType, const cBuffer::eType& type, u32 slot, void* instance, void* viewInstance)
            {
                if (type == cBuffer::eType::CONSTANT) {
                    s_ImmContext->VSSetConstantBuffers(slot, 1, (ID3D11Buffer**)&instance);
                    LogDebugMessage();
                } else if (viewType == cBuffer::eViewType::SRV) {
                    s_ImmContext->VSSetShaderResources(slot, 1, (ID3D11ShaderResourceView**)&viewInstance);
                    LogDebugMessage();
                }
            }

            void PSBindBuffer(const cResource::eViewType& viewType, const cBuffer::eType& type, u32 slot, void* instance, void* viewInstance)
            {
                if (type == cBuffer::eType::CONSTANT) {
                    s_ImmContext->PSSetConstantBuffers(slot, 1, (ID3D11Buffer**)&instance);
                    LogDebugMessage();
                } else if (viewType == cBuffer::eViewType::SRV) {
                    s_ImmContext->PSSetShaderResources(slot, 1, (ID3D11ShaderResourceView**)&viewInstance);
                    LogDebugMessage();
                }
            }

            void PSBindTexture(const cResource::eViewType& viewType, u32 slot, void* instance, void* viewInstance)
            {
                if (viewType == cTexture::eViewType::SRV) {
                    s_ImmContext->PSSetShaderResources(slot, 1, (ID3D11ShaderResourceView**)&viewInstance);
                    LogDebugMessage();
                }
            }

            void GSBindBuffer(const cResource::eViewType& viewType, const cBuffer::eType& type, u32 slot, void* instance, void* viewInstance)
            {
                if (type == cBuffer::eType::CONSTANT) {
                    s_ImmContext->GSSetConstantBuffers(slot, 1, (ID3D11Buffer**)&instance);
                    LogDebugMessage();
                } else if (viewType == cBuffer::eViewType::SRV) {
                    s_ImmContext->GSSetShaderResources(slot, 1, (ID3D11ShaderResourceView**)&viewInstance);
                    LogDebugMessage();
                }
            }

            void CSBindBuffer(const cResource::eViewType& viewType, const cBuffer::eType& type, u32 slot, void* instance, void* viewInstance)
            {
                if (type == cBuffer::eType::CONSTANT) {
                    s_ImmContext->CSSetConstantBuffers(slot, 1, (ID3D11Buffer**)&instance);
                    LogDebugMessage();
                } else if (viewType == cBuffer::eViewType::SRV) {
                    s_ImmContext->CSSetShaderResources(slot, 1, (ID3D11ShaderResourceView**)&viewInstance);
                    LogDebugMessage();
                } else if (viewType == cBuffer::eViewType::UAV) {
                    s_ImmContext->CSSetUnorderedAccessViews(slot, 1, (ID3D11UnorderedAccessView**)&viewInstance, nullptr);
                    LogDebugMessage();
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
                        (ID3D11InputLayout**)&inputLayout.Instance
                );
                LogDebugMessage();

                main_free(attributes);
            }

            void BindInputLayout(const sInputLayout& inputLayout)
            {
                s_ImmContext->IASetInputLayout((ID3D11InputLayout*) inputLayout.Instance);
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
                if (inputLayout.Instance != nullptr)
                {
                    ((ID3D11InputLayout*) inputLayout.Instance)->Release();
                    LogDebugMessage();
                    inputLayout.Instance = nullptr;
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

                s_Device->CreateRasterizerState(&desc, (ID3D11RasterizerState**) &rasterizer.State);
                LogDebugMessage();
            }

            void BindRasterizer(void* state)
            {
                s_ImmContext->RSSetState((ID3D11RasterizerState*) state);
                LogDebugMessage();
            }

            void FreeRasterizer(sRasterizer& rasterizer)
            {
                if (rasterizer.State != nullptr) {
                    ((ID3D11RasterizerState*) rasterizer.State)->Release();
                    LogDebugMessage();
                    rasterizer.State = nullptr;
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

            void DrawQuads(usize count)
            {
                s_ImmContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
                LogDebugMessage();

                s_ImmContext->DrawInstanced(4, count, 0, 0);
                LogDebugMessage();
            }

            void Dispatch(const glm::ivec3& threadGroupCount)
            {
                s_ImmContext->Dispatch(threadGroupCount.x, threadGroupCount.y, threadGroupCount.z);
            }

            void* GetDevice()
            {
                return s_Device;
            }

        }

    }

}