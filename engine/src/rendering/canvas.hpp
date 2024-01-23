#pragma once

#include "rendering/core/context.hpp"

namespace xpe {

    namespace render {

        class cDefaultShader;

        class ENGINE_API cCanvas : public cObject
        {

        public:
            cCanvas();
            ~cCanvas();

            void Draw();

            void WindowFrameResized(s32 width, s32 height);

            void SetViewport(sViewport* viewport);
            inline sViewport* GetViewport() { return m_Viewport; }

        private:
            void CreatePresentTarget();
            void CreatePresentSampler();

        private:
            cDefaultShader* m_Shader;
            sViewport* m_Viewport;
            sSampler* m_PresentSampler;
            cRenderTarget* m_PresentTarget;
            u32 m_BoundTargetIndex = 0;

        };

    }
}