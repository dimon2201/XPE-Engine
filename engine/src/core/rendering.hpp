#pragma once

#include <glm.hpp>

#include "windowing.hpp"

namespace xpe
{
    namespace core
    {
        struct GPUResource
        {
            void* Instance;
        };

        struct RenderTarget
        {
            GPUResource* ColorTexture;
            GPUResource* DepthTexture;
            void* ColorTargetInstance;
            void* DepthTargetInstance;
        };

        class RenderingContext_Interface
        {
            public:
                RenderingContext_Interface() {}
                ~RenderingContext_Interface() {}

                virtual void Init(Window& window) = 0;
                virtual void Free() = 0;
                virtual RenderTarget CreateRenderTarget(GPUResource* color, GPUResource* depth) = 0;
                virtual void FreeRenderTarget(const RenderTarget& renderTarget) = 0;
                virtual void ClearRenderTarget(const RenderTarget& renderTarget, const glm::vec4& color, const f32 depth) = 0;
        };
    }
}