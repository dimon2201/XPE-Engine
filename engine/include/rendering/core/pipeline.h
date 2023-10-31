#pragma once

#include <rendering/core/input_layout.h>
#include <rendering/core/shader.h>
#include <rendering/core/render_target.h>
#include <rendering/core/rasterizer.h>
#include <rendering/core/depth_stencil_mode.h>
#include <rendering/core/blend_mode.h>

namespace xpe {

    namespace render {

        struct ENGINE_API sPipeline final
        {
            render::sShader* Shader = nullptr;
            render::sRenderTarget* RenderTarget = nullptr;
            render::ePrimitiveTopology PrimitiveTopology = ePrimitiveTopology::DEFAULT;
            render::sInputLayout InputLayout;
            vector<sBuffer*> VSBuffers; // buffers defined in vertex shader
            vector<sBuffer*> PSBuffers; // buffers defined in pixel shader
            vector<sTexture*> Textures;
            vector<sSampler*> Samplers;
            render::sRasterizer Rasterizer;
            render::sDepthStencilMode DepthStencil;
            render::sBlendMode Blending;
        };

    }

}