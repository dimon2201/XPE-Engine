#pragma once

#include <rendering/core/input_layout.h>
#include <rendering/core/shader.h>
#include <rendering/core/render_target.h>
#include <rendering/core/rasterizer.h>
#include <rendering/core/depth_stencil_mode.h>
#include <rendering/core/blend_mode.h>

namespace xpe {

    namespace render {

        struct ENGINE_API Pipeline final
        {
            render::Shader* Shader = nullptr;
            render::RenderTarget* RenderTarget = nullptr;
            render::ePrimitiveTopology PrimitiveTopology = ePrimitiveTopology::DEFAULT;
            render::InputLayout InputLayout;
            vector<Buffer*> VSBuffers; // buffers defined in vertex shader
            vector<Buffer*> PSBuffers; // buffers defined in pixel shader
            vector<Texture*> Textures;
            vector<TextureSampler*> Samplers;
            render::Rasterizer Rasterizer;
            render::DepthStencilMode DepthStencil;
            render::BlendMode Blending;
        };

    }

}