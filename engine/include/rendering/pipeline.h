#pragma once

#include <rendering/input_layout.h>
#include <rendering/shader.h>
#include <rendering/render_target.h>
#include <rendering/texture.h>

namespace xpe {

    namespace render {

        struct ENGINE_API Pipeline final : public Object {
            InputLayout InputLayout;
            vector<Buffer*> VSBuffers; // buffers defined in vertex shader
            vector<Buffer*> PSBuffers; // buffers defined in pixel shader
            vector<Texture*> Textures; // textures defined in pixel shader
            vector<TextureSampler*> Samplers; // samplers defined in pixel shader
            Shader* Shader;
            RenderTarget* RenderTarget;
            DepthStencilState DepthStencilState;
        };

    }

}