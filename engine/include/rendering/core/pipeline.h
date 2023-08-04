#pragma once

#include <rendering/core/input_layout.h>
#include <rendering/core/shader.h>
#include <rendering/core/render_target.h>
#include <rendering/core/rasterizer.h>

namespace xpe {

    namespace render {

        struct ENGINE_API Pipeline final {
            InputLayout InputLayout;
            vector<Buffer*> VSBuffers; // buffers defined in vertex shader
            vector<Buffer*> PSBuffers; // buffers defined in pixel shader
            vector<Texture*> Textures;
            vector<TextureSampler*> Samplers;
            Shader* Shader = nullptr;
            RenderTarget* RenderTarget = nullptr;
            DepthStencilState DepthStencilState;
            BlendState BlendState;
            Rasterizer Rasterizer;
        };

    }

}