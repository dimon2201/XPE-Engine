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
            vector<sBuffer*> VSListBuffers; // list buffers defined in vertex shader
            vector<sBuffer*> VSItemBuffers; // item buffers defined in vertex shader
            vector<sBuffer*> PSListBuffers; // list buffers defined in pixel shader
            vector<sBuffer*> PSItemBuffers; // item buffers defined in pixel shader
            vector<sBuffer*> GSListBuffers; // list buffers defined in geometry shader
            vector<sBuffer*> GSItemBuffers; // item buffers defined in geometry shader
            vector<sTexture*> Textures;
            vector<sSampler*> Samplers;
            sShader* Shader = nullptr;
            sRenderTarget* RenderTarget = nullptr;
        };

        struct ENGINE_API sComputePipeline final
        {
            vector<sBuffer*> CSListBuffers; // list buffers defined in compute shader
            vector<sBuffer*> CSItemBuffers; // item buffers defined in compute shader
            vector<sTexture*> Textures;
            vector<sSampler*> Samplers;
            sShader* Shader = nullptr;
            sRenderTarget* RenderTarget = nullptr;
        };

    }

}