#pragma once

#include <render_context.hpp>

namespace xpe
{
    namespace render
    {
        class ENGINE_API cCanvas : public cObject
        {

        public:
            cCanvas(sViewport* viewport);
            ~cCanvas();

            void Draw();
            void WindowFrameResized(s32 width, s32 height);

        private:
            void CreatePresentTarget();
            void CreatePresentSampler();

            cDefaultShader* m_Shader;
            sViewport* m_Viewport;
            cSampler* m_PresentSampler;
            cRenderTarget* m_PresentTarget;
            u32 m_BoundTargetIndex = 0;
        };
    }
}