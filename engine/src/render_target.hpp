#pragma once

#include <texture_manager.hpp>

namespace xpe
{
    namespace render
    {
        struct ENGINE_API cRenderTarget : public cObject
        {
            vector<void*> ColorViews;
            void* DepthStencilView = nullptr;
            vector<cTexture*> Colors;
            cTexture* DepthStencil = nullptr;
            sViewport* Viewport = nullptr;
            vector<glm::vec4> ClearColors;
            float ClearDepth = K_DEPTH_INVALID;

            cRenderTarget(const vector<cTexture*>& colors, sViewport* viewport);
            cRenderTarget(const cTexture* depthStencil, sViewport* viewport);
            cRenderTarget(const vector<cTexture*>& colors, const cTexture* depthStencil, sViewport* viewport);
            cRenderTarget(const vector<void*>& colorViews, sViewport* viewport);

            ~cRenderTarget();

            void Resize(s32 width, s32 height);
            void ResizeColors(s32 width, s32 height);

            void Bind();
            void BindColor(u32 index);
            void BindDepth();
            void Unbind();

            void WindowFrameResized(s32 width, s32 height);

            void Clear();

            void SetResizable(bool resizable);

            [[nodiscard]] inline bool IsResizable() const
            {
                return m_Resizable;
            }

        private:
            void Init();

            bool m_Resizable = false;
        };
    }
}