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

            void WindowFrameResized(int width, int height);

            inline RenderTarget* GetRenderTarget() { return &m_RenderTarget; }
            inline Shader* GetShader() { return m_Shader; }
            inline void SetViewport(const Viewport& viewport) { m_Viewport = viewport; }
            inline void SetClearColor(const glm::vec4& color) { m_ClearColor = color; }

        private:
            void CreateRenderTarget(int width, int height);
            void CreatePresentTarget();
            void CreatePresentSampler();
            void FreePresentTarget();

        private:
            Context* m_Context;
            Viewport m_Viewport;
            glm::vec4 m_ClearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
            RenderTarget m_RenderTarget;
            Shader* m_Shader = nullptr;
            Texture m_ColorTexture;
            Texture m_DepthTexture;
            TextureSampler m_PresentSampler;
            RenderTarget m_PresentTarget;
        };

    }
}