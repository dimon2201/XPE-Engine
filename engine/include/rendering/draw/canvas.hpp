#pragma once

#include <rendering/core/context.hpp>
#include <rendering/buffers/viewport_buffer.h>

namespace xpe {

    namespace render {

        class ENGINE_API Canvas : public Object
        {

        public:
            Canvas(s32 width, s32 height, Shader* shader);
            Canvas(const glm::ivec2& size, Shader* shader);
            ~Canvas();

            void Clear(const glm::vec4& color);
            void Present();

            void WindowFrameResized(s32 width, s32 height);

            inline RenderTarget* GetRenderTarget() { return &m_RenderTarget; }
            inline Shader* GetShader() { return m_Shader; }
            inline void SetViewport(const Viewport& viewport) { m_ViewportBuffer.FlushItem(viewport); }
            inline void SetClearColor(const glm::vec4& color) { m_ClearColor = color; }
            inline glm::vec2 GetDimension() { return { m_ViewportBuffer.Item.Width, m_ViewportBuffer.Item.Height }; }
            inline ViewportBuffer* GetBuffer() { return &m_ViewportBuffer; }

        private:
            void CreateRenderTarget(int width, int height);
            void CreatePresentTarget();
            void CreatePresentSampler();
            void FreePresentTarget();

        private:
            ViewportBuffer m_ViewportBuffer;
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