#pragma once

#include <rendering/input_layout.h>
#include <rendering/shader.h>
#include <rendering/render_target.h>

namespace xpe {

    namespace render {

        struct ENGINE_API Pipeline final {
            InputLayout InputLayout;
            Buffer* VertexBuffer;
            Buffer* IndexBuffer;
            vector<Buffer*> VSBuffers; // buffers defined in vertex shader
            vector<Buffer*> PSBuffers; // buffers defined in pixel shader
            Shader* Shader;
            RenderTarget* RenderTarget;
            DepthStencilState DepthStencilState;
        };

    }

}