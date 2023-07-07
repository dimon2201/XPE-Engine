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
            _swapChainTexture.Width = window.GetWidth();
            _swapChainTexture.Height = window.GetHeight();
            CreateTexture(_swapChainTexture, _swapChainTexture.Instance);
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

                if (errorBlob != nullptr) {
                    LogError((char*)errorBlob->GetBufferPointer());
                    errorBlob->Release();
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

        static const unordered_map<Texture::eFormat, u32> s_TexturePitchTable = {

                { Texture::eFormat::R8, 1 },
                { Texture::eFormat::R16, 1 },
                { Texture::eFormat::R32, 1 },

                { Texture::eFormat::RG8, 2 },
                { Texture::eFormat::RG16, 2 },
                { Texture::eFormat::RG32, 2 },

                { Texture::eFormat::RGB8, 3 },
                { Texture::eFormat::RGB16, 3 },
                { Texture::eFormat::RGB32, 3 },

                { Texture::eFormat::RGBA8, 4 },
                { Texture::eFormat::RGBA16, 4 },
                { Texture::eFormat::RGBA32, 4 },

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

                D3D11_TEXTURE1D_DESC texture1DDesc = {};
                D3D11_TEXTURE2D_DESC texture2DDesc = {};
                D3D11_TEXTURE3D_DESC texture3DDesc = {};

                D3D11_SUBRESOURCE_DATA initialData;
                D3D11_SUBRESOURCE_DATA* pInitialData = nullptr;

                D3D11_SHADER_RESOURCE_VIEW_DESC srv = {};

                if (texture.Pixels != nullptr) {
                    initialData.pSysMem = texture.Pixels;
                    initialData.SysMemPitch = texture.Width * s_TexturePitchTable.at(texture.Format);
                    pInitialData = &initialData;
                }

                switch (texture.Type) {

                    case Texture::eType::TEXTURE_1D:
                        texture1DDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
                        texture1DDesc.Usage = s_TextureUsageTable.at(texture.Usage);
                        texture1DDesc.Format = s_TextureFormatTable.at(texture.Format);
                        texture1DDesc.Width = texture.Width;
                        texture1DDesc.ArraySize = texture.ArraySize;
                        texture1DDesc.MipLevels = texture.MipLevels;

                        _device->CreateTexture1D(&texture1DDesc, pInitialData, (ID3D11Texture1D**)&texture.Instance);

                        srv.Format = s_TextureFormatTable.at(texture.Format);
                        srv.Texture1D.MipLevels = texture.MipLevels;
                        srv.Texture1D.MostDetailedMip = texture.MostDetailedMip;
                        srv.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1D;

                        _device->CreateShaderResourceView((ID3D11Resource*)texture.Instance, &srv, (ID3D11ShaderResourceView**)&texture.ViewInstance);

                        break;

                    case Texture::eType::TEXTURE_2D:
                        texture2DDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
                        texture2DDesc.Usage = s_TextureUsageTable.at(texture.Usage);
                        texture2DDesc.Format = s_TextureFormatTable.at(texture.Format);
                        texture2DDesc.Width = texture.Width;
                        texture2DDesc.Height = texture.Height;
                        texture2DDesc.ArraySize = texture.ArraySize;
                        texture2DDesc.SampleDesc.Count = 1;
                        texture2DDesc.SampleDesc.Quality = 0;
                        texture2DDesc.MipLevels = texture.MipLevels;
                        texture2DDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

                        _device->CreateTexture2D(&texture2DDesc, pInitialData, (ID3D11Texture2D**)&texture.Instance);

                        LogDebugMessage();

                        srv.Format = s_TextureFormatTable.at(texture.Format);
                        srv.Texture2D.MipLevels = texture.MipLevels;
                        srv.Texture2D.MostDetailedMip = texture.MostDetailedMip;
                        srv.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;

                        _device->CreateShaderResourceView((ID3D11Resource*)texture.Instance, &srv, (ID3D11ShaderResourceView**)&texture.ViewInstance);

                        break;

                    case Texture::eType::TEXTURE_3D:
                        texture3DDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
                        texture3DDesc.Usage = s_TextureUsageTable.at(texture.Usage);
                        texture3DDesc.Format = s_TextureFormatTable.at(texture.Format);
                        texture3DDesc.Width = texture.Width;
                        texture3DDesc.Height = texture.Height;
                        texture3DDesc.MipLevels = texture.MipLevels;

                        _device->CreateTexture3D(&texture3DDesc, pInitialData, (ID3D11Texture3D**)&texture.Instance);

                        srv.Format = s_TextureFormatTable.at(texture.Format);
                        srv.Texture3D.MipLevels = texture.MipLevels;
                        srv.Texture3D.MostDetailedMip = texture.MostDetailedMip;
                        srv.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;

                        _device->CreateShaderResourceView((ID3D11Resource*)texture.Instance, &srv, (ID3D11ShaderResourceView**)&texture.ViewInstance);

                        break;

                    case Texture::eType::TEXTURE_CUBE:
                        texture2DDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
                        texture2DDesc.Usage = s_TextureUsageTable.at(texture.Usage);
                        texture2DDesc.Format = s_TextureFormatTable.at(texture.Format);
                        texture2DDesc.Width = texture.Width;
                        texture2DDesc.Height = texture.Height;
                        texture2DDesc.MipLevels = texture.MipLevels;
                        texture2DDesc.ArraySize = 6;
                        texture2DDesc.CPUAccessFlags = 0;
                        texture2DDesc.SampleDesc.Count = 1;
                        texture2DDesc.SampleDesc.Quality = 0;
                        texture2DDesc.CPUAccessFlags = 0;
                        texture2DDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

                        srv.Format = s_TextureFormatTable.at(texture.Format);
                        srv.TextureCube.MipLevels = texture.MipLevels;
                        srv.TextureCube.MostDetailedMip = texture.MostDetailedMip;
                        srv.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;

//                        D3D11_SHADER_RESOURCE_VIEW_DESC SMViewDesc;
//                        SMViewDesc.Format = texDesc.Format;
//                        SMViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
//                        SMViewDesc.TextureCube.MipLevels =  texDesc.MipLevels;
//                        SMViewDesc.TextureCube.MostDetailedMip = 0;
//
//                        D3D11_SUBRESOURCE_DATA pData[6];
//                        std::vector<vector4b> d[6]; // 6 images of type vector4b = 4 * unsigned char
//
//                        for (int cubeMapFaceIndex = 0; cubeMapFaceIndex < 6; cubeMapFaceIndex++)
//                        {
//                            d[cubeMapFaceIndex].resize(description.width * description.height);
//
//                            // fill with red color
//                            std::fill(
//                                    d[cubeMapFaceIndex].begin(),
//                                    d[cubeMapFaceIndex].end(),
//                                    vector4b(255,0,0,255));
//
//                            pData[cubeMapFaceIndex].pSysMem = &d[cubeMapFaceIndex][0];// description.data;
//                            pData[cubeMapFaceIndex].SysMemPitch = description.width * 4;
//                            pData[cubeMapFaceIndex].SysMemSlicePitch = 0;
//                        }
//
//                        HRESULT hr = renderer->getDevice()->CreateTexture2D(&texDesc,
//                                                                            description.data[0] ? &pData[0] : nullptr, &m_pCubeTexture);
//                        assert(hr == S_OK);
//
//                        hr = renderer->getDevice()->CreateShaderResourceView(
//                                m_pCubeTexture, &SMViewDesc, &m_pShaderResourceView);
//                        assert(hr == S_OK);
                        break;

                }

            }

            else {

                switch (texture.Type) {

                    case Texture::eType::TEXTURE_1D:
                        texture.Instance = (ID3D11Texture1D*)instance;
                        break;

                    case Texture::eType::TEXTURE_2D:
                        texture.Instance = (ID3D11Texture2D*)instance;
                        break;

                    case Texture::eType::TEXTURE_3D:
                        texture.Instance = (ID3D11Texture3D*)instance;
                        break;

                    case Texture::eType::TEXTURE_CUBE:
                        texture.Instance = (ID3D11Texture2D*)instance;
                        break;

                }

            }

        }

        void D3D11Context::BindTexture(const Texture* texture)
        {
            if (texture != nullptr) {
                _immContext->PSSetShaderResources(texture->Slot, 1, (ID3D11ShaderResourceView**)&texture->ViewInstance);
            }
        }

        void D3D11Context::BindTextureSlot(u32 slot) {
            ID3D11ShaderResourceView* views = nullptr;
            _immContext->PSSetShaderResources(slot, 1, &views);
        }

        void D3D11Context::FreeTexture(const Texture* texture)
        {
            if (texture->Instance != nullptr)
            {

                switch (texture->Type) {

                    case Texture::eType::TEXTURE_1D:
                        ((ID3D11Texture1D*)texture->Instance)->Release();
                        break;

                    case Texture::eType::TEXTURE_2D:
                        ((ID3D11ShaderResourceView*)texture->ViewInstance)->Release();
                        ((ID3D11Texture2D*)texture->Instance)->Release();
                        break;

                    case Texture::eType::TEXTURE_3D:
                        ((ID3D11Texture3D*)texture->Instance)->Release();
                        break;

                    case Texture::eType::TEXTURE_CUBE:
                        ((ID3D11Texture2D*)texture->Instance)->Release();
                        break;

                }

            }
        }

        void D3D11Context::WriteTexture(const Texture &texture, const void *pixels, usize pixelsSize)
        {
            D3D11_MAPPED_SUBRESOURCE mappedResource = {};

            _immContext->Map((ID3D11Resource*)texture.Instance, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
            memcpy(mappedResource.pData, pixels, pixelsSize);
            _immContext->Unmap((ID3D11Resource*)texture.Instance, 0);
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
        }

        void D3D11Context::BindSampler(const TextureSampler* sampler)
        {
            _immContext->PSSetSamplers(sampler->Slot, 1, (ID3D11SamplerState**)&sampler->Instance);
        }

        void D3D11Context::FreeSampler(const TextureSampler* sampler)
        {
            if (sampler->Instance != nullptr)
            {
                ((ID3D11SamplerState*)sampler->Instance)->Release();
            }
        }

        void D3D11Context::CreateBuffer(Buffer& buffer, boolean duplicate)
        {
            eBufferType bufferType = buffer.Type;
            usize byteSize = buffer.ByteSize;
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

            if (bufferType == eBufferType::STRUCTURED)
            {
                D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
                srvDesc.Format = DXGI_FORMAT_UNKNOWN;
                srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
                srvDesc.BufferEx.FirstElement = buffer.FirstElement;
                srvDesc.BufferEx.NumElements = buffer.NumElements;

                _device->CreateShaderResourceView((ID3D11Resource*)buffer.Instance, &srvDesc, (ID3D11ShaderResourceView**)&buffer.ViewInstance);
            }

        }

        void D3D11Context::BindVertexBuffer(const Buffer *buffer) {
            UINT stride = xpe::gltf::cGLTFModel::k_vertexSize;
            UINT offset = 0;
            _immContext->IASetVertexBuffers(buffer->Slot, 1, (ID3D11Buffer**)&buffer->Instance, &stride, &offset);
        }

        void D3D11Context::BindIndexBuffer(const Buffer *buffer) {
            DXGI_FORMAT format = xpe::gltf::cGLTFModel::k_indexSize == 4 ? DXGI_FORMAT_R32_UINT : DXGI_FORMAT_R16_UINT;
            _immContext->IASetIndexBuffer((ID3D11Buffer*)buffer->Instance, format, 0);
        }

        void D3D11Context::BindVSBuffer(const Buffer *buffer) {
            if (buffer->Type == eBufferType::STRUCTURED)
            {
                _immContext->VSSetShaderResources(buffer->Slot, 1, (ID3D11ShaderResourceView**)&buffer->ViewInstance);
            }
            else if (buffer->Type == eBufferType::CONSTANT)
            {
                _immContext->VSSetConstantBuffers(buffer->Slot, 1, (ID3D11Buffer**)&buffer->Instance);
            }
        }

        void D3D11Context::BindPSBuffer(const Buffer *buffer) {
            if (buffer->Type == eBufferType::STRUCTURED)
            {
                _immContext->PSSetShaderResources(buffer->Slot, 1, (ID3D11ShaderResourceView**)&buffer->ViewInstance);
            }
            else if (buffer->Type == eBufferType::CONSTANT)
            {
                _immContext->PSSetConstantBuffers(buffer->Slot, 1, (ID3D11Buffer**)&buffer->Instance);
            }
        }

        void D3D11Context::WriteBuffer(const Buffer& buffer, const void* data, usize dataByteSize)
        {
            D3D11_MAPPED_SUBRESOURCE mappedResource = {};

            _immContext->Map((ID3D11Resource*)buffer.Instance, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
            memcpy(mappedResource.pData, data, dataByteSize);
            _immContext->Unmap((ID3D11Resource*)buffer.Instance, 0);
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
            if (buffer.Instance != nullptr)
            {
                ((ID3D11Buffer*)buffer.Instance)->Release();
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
            BindVertexBuffer(_boundPipeline->VertexBuffer);
            BindIndexBuffer(_boundPipeline->IndexBuffer);
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
