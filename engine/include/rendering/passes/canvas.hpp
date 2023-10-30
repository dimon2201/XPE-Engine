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

            void Draw();

            void WindowFrameResized(s32 width, s32 height);

            inline void SetRenderTarget(RenderTarget* renderTarget) { m_RenderTarget = renderTarget; }

        private:
            void CreatePresentTarget();
            void CreatePresentSampler();

        private:
            RenderTarget* m_RenderTarget;
            Shader* m_Shader;
            ViewportBuffer* m_ViewportBuffer;
            TextureSampler m_PresentSampler;
            RenderTarget* m_PresentTarget;
            u32 m_BoundTargetIndex = 0;
        };

    }
}