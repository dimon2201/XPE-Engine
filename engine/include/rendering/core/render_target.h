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
            vector<sViewport> Viewports;

            sRenderTarget(const vector<sTexture*>& colors, const sViewport& viewport);
            sRenderTarget(const vector<sTexture*>& colors, const vector<sViewport>& viewports);

            sRenderTarget(const sTexture* depthStencil, const sViewport& viewport);
            sRenderTarget(const sTexture* depthStencil, const vector<sViewport>& viewports);

            sRenderTarget(const vector<sTexture*>& colors, const sTexture* depthStencil, const sViewport& viewport);
            sRenderTarget(const vector<sTexture*>& colors, const sTexture* depthStencil, const vector<sViewport>& viewports);

            sRenderTarget(const vector<void*>& colorViews, const sViewport& viewport);
            sRenderTarget(const vector<void*>& colorViews, const vector<sViewport>& viewports);

            ~sRenderTarget();

            void Resize(s32 width, s32 height);

            void Bind();
            void BindColor(u32 index);
            void BindDepth();

            void WindowFrameResized(s32 width, s32 height);

            void ClearColor(u32 index, const glm::vec4& color);
            void ClearDepth(const float depth);

            void AddEventListeners();
            void RemoveEventListeners();

        private:
            void Init();
        };

    }

}