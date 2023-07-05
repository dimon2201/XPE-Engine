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
            Buffer* InstanceBuffer;
            vector<Buffer*> ConstantBuffers;
            Shader* Shader;
            RenderTarget* RenderTarget;
            DepthStencilState DepthStencilState;
        };

    }

}