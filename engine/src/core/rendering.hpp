#pragma once

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
            GPUResource* ColorTargetResource;
        };

        class RenderingContext_Interface
        {
            public:
                RenderingContext_Interface() {}
                ~RenderingContext_Interface() {}

                virtual void Init(Window& window) = 0;
                virtual void Free() = 0;
                virtual RenderTarget CreateRenderTarget(GPUResource* texture) = 0;
        };
    }
}