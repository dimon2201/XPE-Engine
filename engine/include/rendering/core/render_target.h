#pragma once

#include <rendering/core/texture.h>
#include <rendering/core/viewport.h>

namespace xpe {

    namespace render {

        struct ENGINE_API RenderTarget : public Object
        {
            vector<void*> ColorViews;
            void* DepthStencilView = nullptr;
            vector<Texture*> Colors;
            Texture* DepthStencil;
            vector<Viewport> Viewports;

            RenderTarget(const vector<Texture*>& colors, const Viewport& viewport);
            RenderTarget(const vector<Texture*>& colors, const vector<Viewport>& viewports);

            RenderTarget(const Texture* depthStencil, const Viewport& viewport);
            RenderTarget(const Texture* depthStencil, const vector<Viewport>& viewports);

            RenderTarget(const vector<Texture*>& colors, const Texture* depthStencil, const Viewport& viewport);
            RenderTarget(const vector<Texture*>& colors, const Texture* depthStencil, const vector<Viewport>& viewports);

            RenderTarget(const vector<void*>& colorViews, const Viewport& viewport);
            RenderTarget(const vector<void*>& colorViews, const vector<Viewport>& viewports);

            ~RenderTarget();

            void Resize(s32 width, s32 height);

            void Bind();
            void BindColor(u32 index);
            void BindDepth();

            void WindowFrameResized(s32 width, s32 height);

            void ClearColor(u32 index, const glm::vec4& color);
            void ClearDepth(const float depth);

        private:
            void Init();
        };

    }

}