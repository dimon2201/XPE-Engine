#pragma once

#include <rendering/core/context.hpp>

namespace xpe {

    namespace render {

        class ENGINE_API cCanvas : public cObject
        {

        public:
            cCanvas(sShader* shader, sTexture* texture, sViewport* viewport);
            ~cCanvas();

            void Draw();

            void WindowFrameResized(s32 width, s32 height);

            inline void SetTexture(sTexture* texture) { m_Texture = texture; }
            inline sTexture* GetTexture() { return m_Texture; }

        private:
            void CreatePresentTarget();
            void CreatePresentSampler();

        private:
            sTexture* m_Texture;
            sShader* m_Shader;
            sViewport* m_Viewport;
            sSampler m_PresentSampler;
            sRenderTarget* m_PresentTarget;
            u32 m_BoundTargetIndex = 0;
        };

    }
}