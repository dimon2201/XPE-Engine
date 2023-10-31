#pragma once

#include <rendering/core/context.hpp>
#include <rendering/buffers/viewport_buffer.h>

namespace xpe {

    namespace render {

        class ENGINE_API cCanvas : public cObject
        {

        public:
            cCanvas(sShader* shader, sRenderTarget* renderTarget, sViewportBuffer* viewportBuffer);
            ~cCanvas();

            void Draw();

            void WindowFrameResized(s32 width, s32 height);

            inline void SetRenderTarget(sRenderTarget* renderTarget) { m_RenderTarget = renderTarget; }

        private:
            void CreatePresentTarget();
            void CreatePresentSampler();

        private:
            sRenderTarget* m_RenderTarget;
            sShader* m_Shader;
            sViewportBuffer* m_ViewportBuffer;
            sSampler m_PresentSampler;
            sRenderTarget* m_PresentTarget;
            u32 m_BoundTargetIndex = 0;
        };

    }
}