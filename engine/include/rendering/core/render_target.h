#pragma once

#include <rendering/core/texture.h>
#include <rendering/core/viewport.h>

namespace xpe {

    namespace render {

        struct ENGINE_API sRenderTarget : public cObject
        {
            vector<void*> ColorViews;
            void* DepthStencilView = nullptr;
            vector<sTexture*> Colors;
            sTexture* DepthStencil = nullptr;
            sViewport* Viewport = nullptr;

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

            void WindowFrameResized(s32 width, s32 height);

            void ClearColor(u32 index, const glm::vec4& color);
            void ClearDepth(const float depth);

            void SetResizable(bool resizable);
            [[nodiscard]] inline bool IsResizable() const { return m_Resizable; }

        private:
            void Init();

            bool m_Resizable = false;
        };

    }

}