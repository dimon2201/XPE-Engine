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
                RenderTarget CreateRenderTarget(GPUResource* color, GPUResource* depth) override final;
                void FreeRenderTarget(const RenderTarget& renderTarget) override final;
                void ClearRenderTarget(const RenderTarget& renderTarget, const glm::vec4& color, const f32 depth) override final;

                inline GPUResource* GetSwapChainTexture() { return &_swapChainTexture; }

            private:
                IDXGISwapChain* _swapChain;
                ID3D11Device* _device;
                ID3D11DeviceContext* _immContext;
                GPUResource _swapChainTexture;
        };
    }
}