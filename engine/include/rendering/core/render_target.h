#pragma once

#include <rendering/core/texture.h>
#include <rendering/core/viewport.h>

namespace xpe {

    namespace render {

        struct ENGINE_API RenderTarget : public core::Object
        {
            vector<Texture*> Colors;
            Texture* DepthStencil = nullptr;
            vector<void*> ColorViews;
            void* DepthStencilView = nullptr;
            vector<Viewport>* Viewports = nullptr;
        };

        struct ENGINE_API RenderPass : public core::Object
        {
            vector<Texture> Colors;
            Texture DepthStencil;
            vector<Viewport> Viewports;
            RenderTarget Target;

            RenderPass(const vector<Texture>& colors);
            RenderPass(const vector<Texture>& colors, const Texture& depthStencil);

            RenderPass(const vector<Texture>& colors, const Viewport& viewport);
            RenderPass(const vector<Texture>& colors, const vector<Viewport>& viewports);

            RenderPass(const Texture& depthStencil, const Viewport& viewport);
            RenderPass(const Texture& depthStencil, const vector<Viewport>& viewports);

            RenderPass(const vector<Texture>& colors, const Texture& depthStencil, const Viewport& viewport);
            RenderPass(const vector<Texture>& colors, const Texture& depthStencil, const vector<Viewport>& viewports);

            ~RenderPass();

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