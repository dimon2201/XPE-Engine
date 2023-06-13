#pragma once

#include <d3d11.h>

#include "rendering.hpp"

namespace xpe
{
    namespace core
    {
        class D3D11RenderingContext : public RenderingContext_Interface
        {
            public:
                D3D11RenderingContext() {}
                ~D3D11RenderingContext() {}

                void Init(Window& window) override final;
                void Free() override final;
                RenderTarget CreateRenderTarget(GPUResource* texture) override final;

            private:
                IDXGISwapChain* _swapChain;
                ID3D11Texture2D* _swapChainTexture;
                ID3D11Device* _device;
                ID3D11DeviceContext* _immContext;
        };
    }
}