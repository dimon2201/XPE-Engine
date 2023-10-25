#pragma once

#include <rendering/core/context.hpp>
#include <rendering/buffers/viewport_buffer.h>

namespace xpe {

    namespace render {

        class ENGINE_API Canvas : public Object
        {

        public:
            Canvas(s32 width, s32 height, Shader* shader, s32 msaaSampleCount = 1);
            Canvas(const glm::ivec2& size, Shader* shader, s32 msaaSampleCount = 1);
            ~Canvas();

            void Clear(const glm::vec4& color);
            void Present();

            void WindowFrameResized(s32 width, s32 height);

            inline RenderTarget* GetRenderTarget() { return m_RenderTarget.Get(); }
            inline Shader* GetShader() { return m_Shader; }
            inline void SetViewport(const Viewport& viewport) { m_ViewportBuffer.FlushItem(m_BoundTargetIndex, viewport); }
            inline void SetClearColor(const glm::vec4& color) { m_ClearColor = color; }
            inline glm::vec2 GetDimension() { return { m_ViewportBuffer[m_BoundTargetIndex]->Width, m_ViewportBuffer[m_BoundTargetIndex]->Height }; }
            inline ViewportBuffer* GetBuffer() { return &m_ViewportBuffer; }
            inline Viewport* GetViewport(u32 index) { return GetBuffer()->Get(index); }
            inline s32 GetMSAA() { return m_MsaaSampleCount; }
            inline void SetMSAA(s32 msaaSampleCount) { m_MsaaSampleCount = msaaSampleCount; }

        private:
            void CreateRenderTarget(int width, int height);
            void CreatePresentTarget();
            void CreatePresentSampler();

        private:
            ViewportBuffer m_ViewportBuffer;
            glm::vec4 m_ClearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
            Scope<RenderTarget> m_RenderTarget;
            Shader* m_Shader = nullptr;
            TextureSampler m_PresentSampler;
            Scope<RenderTarget> m_PresentTarget;
            u32 m_BoundTargetIndex = 0;
            s32 m_MsaaSampleCount = 1;
        };

    }
}