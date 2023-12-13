#pragma once

#include <rendering/core/texture.h>

namespace xpe {

    namespace render {

        struct ENGINE_API sRenderTarget : public cObject
        {
            vector<void*> ColorViews;
            void* DepthStencilView = nullptr;
            vector<sTexture*> Colors;
            sTexture* DepthStencil = nullptr;
            sViewport* Viewport = nullptr;
            vector<glm::vec4> ClearColors;
            float ClearDepth = K_DEPTH_INVALID;

            sRenderTarget(const vector<sTexture*>& colors, sViewport* viewport);
            sRenderTarget(const sTexture* depthStencil, sViewport* viewport);
            sRenderTarget(const vector<sTexture*>& colors, const sTexture* depthStencil, sViewport* viewport);
            sRenderTarget(const vector<void*>& colorViews, sViewport* viewport);

            ~sRenderTarget();

            void Resize(s32 width, s32 height);
            void ResizeColors(s32 width, s32 height);

            void Bind();
            void BindColor(u32 index);
            void BindDepth();
            void Unbind();

            void WindowFrameResized(s32 width, s32 height);

            void Clear();

            void SetResizable(bool resizable);
            [[nodiscard]] inline bool IsResizable() const { return m_Resizable; }

        private:
            void Init();

            bool m_Resizable = false;
        };

    }

}