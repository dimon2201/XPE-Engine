#pragma once

#include <rendering/core/context.hpp>
#include <rendering/buffers/viewport_buffer.h>

namespace xpe {

    namespace render {

        class ENGINE_API Canvas : public Object
        {

        public:
            Canvas(Shader* shader, RenderTarget* renderTarget, ViewportBuffer* viewportBuffer);
            ~Canvas();

            void Clear(const glm::vec4& color);
            void Present();

            void WindowFrameResized(s32 width, s32 height);

            inline void SetRenderTarget(RenderTarget* renderTarget) { m_RenderTarget = renderTarget; }
            inline void SetClearColor(const glm::vec4& color) { m_ClearColor = color; }

        private:
            void CreatePresentTarget();
            void CreatePresentSampler();

        private:
            RenderTarget* m_RenderTarget;
            Shader* m_Shader;
            ViewportBuffer* m_ViewportBuffer;
            TextureSampler m_PresentSampler;
            Scope<RenderTarget> m_PresentTarget;
            u32 m_BoundTargetIndex = 0;
            glm::vec4 m_ClearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
        };

    }
}