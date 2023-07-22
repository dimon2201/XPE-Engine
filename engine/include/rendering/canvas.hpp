#pragma once

#include <rendering/context.hpp>
#include <rendering/shader.h>
#include <rendering/render_target.h>

namespace xpe {

    namespace render {

        class ENGINE_API Canvas : public Object {

            public:
                Canvas(s32 width, s32 height, Context* context);
                Canvas(const glm::ivec2& size, Context* context);
                ~Canvas();

                void Clear(const glm::vec4& color);
                void Present();

                inline s32 GetWidth() const { return m_Size.x; }
                inline s32 GetHeight() const { return m_Size.y; }
                inline RenderTarget* GetRenderTarget() { return &m_RenderTarget; }
                inline Shader* GetShader() { return &m_Shader; }

            private:
                glm::ivec2 m_Size;
                Context* m_Context;
                RenderTarget m_RenderTarget;
                Shader m_Shader;
                Texture m_ColorTexture;
                Texture m_DepthTexture;
        };

    }
}