#pragma once

#include <rendering/core/input_layout.h>
#include <rendering/core/shader.h>
#include <rendering/core/render_target.h>
#include <rendering/core/rasterizer.h>

namespace xpe {

    namespace render {

        struct ENGINE_API Pipeline final
        {
            render::Shader* Shader = nullptr;
            render::RenderTarget* RenderTarget = nullptr;
            ePrimitiveTopology PrimitiveTopology = ePrimitiveTopology::DEFAULT;
            render::InputLayout InputLayout;
            Buffer* VertexBuffer = nullptr;
            Buffer* IndexBuffer = nullptr;
            vector<Buffer*> VSBuffers; // buffers defined in vertex shader
            vector<Buffer*> PSBuffers; // buffers defined in pixel shader
            vector<Texture*> Textures;
            vector<TextureSampler*> Samplers;
            render::DepthStencilState DepthStencilState;
            render::BlendState BlendState;
            render::Rasterizer Rasterizer;
        };

    }

}