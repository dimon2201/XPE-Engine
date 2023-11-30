#pragma once

#include <rendering/core/input_layout.h>
#include <rendering/core/shader.h>
#include <rendering/core/render_target.h>
#include <rendering/core/rasterizer.h>
#include <rendering/core/depth_stencil_mode.h>
#include <rendering/core/blend_mode.h>

namespace xpe {

    namespace render {

        struct ENGINE_API sVertexPipeline final
        {
            ePrimitiveTopology PrimitiveTopology = ePrimitiveTopology::DEFAULT;
            sInputLayout InputLayout;
            sRasterizer Rasterizer;
            sDepthStencilMode DepthStencil;
            sBlendMode Blending;
            sShader* Shader = nullptr;
            sRenderTarget* RenderTarget = nullptr;
        };

        struct ENGINE_API sComputePipeline final
        {
            sShader* Shader = nullptr;
            sRenderTarget* RenderTarget = nullptr;
        };

    }

}