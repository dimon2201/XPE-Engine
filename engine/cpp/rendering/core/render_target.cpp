#include <rendering/core/render_target.h>
#include <rendering/core/context.hpp>

namespace xpe {

    namespace render {

        RenderTarget::RenderTarget(const vector<Texture*> &colors, const Viewport &viewport)
        {
            Colors.reserve(colors.size());
            ColorViews.reserve(colors.size());
            for (auto& color : colors)
            {
                Colors.emplace_back(color);
                ColorViews.emplace_back(nullptr);
            }
            Viewports.emplace_back(viewport);
            Init();
        }

        RenderTarget::RenderTarget(const vector<Texture*> &colors, const vector<Viewport> &viewports)
        {
            Colors.reserve(colors.size());
            ColorViews.reserve(colors.size());
            for (auto& color : colors)
            {
                Colors.emplace_back(color);
                ColorViews.emplace_back(nullptr);
            }
            Viewports = viewports;
            Init();
        }

        RenderTarget::RenderTarget(const Texture* depthStencil, const Viewport &viewport)
        {
            DepthStencil = (Texture*)depthStencil;
            DepthStencilView = nullptr;
            Viewports.emplace_back(viewport);
            Init();
        }

        RenderTarget::RenderTarget(const Texture* depthStencil, const vector<Viewport> &viewports)
        {
            DepthStencil = (Texture*)depthStencil;
            DepthStencilView = nullptr;
            Viewports = viewports;
            Init();
        }

        RenderTarget::RenderTarget(
            const vector<Texture*> &colors,
            const Texture* depthStencil,
            const Viewport& viewport
        ) {
            Colors.reserve(colors.size());
            ColorViews.reserve(colors.size());
            for (auto& color : colors)
            {
                Colors.emplace_back(color);
                ColorViews.emplace_back(nullptr);
            }
            DepthStencil = (Texture*)depthStencil;
            DepthStencilView = nullptr;
            Viewports.emplace_back(viewport);
            Init();
        }

        RenderTarget::RenderTarget(
            const vector<Texture*> &colors,
            const Texture* depthStencil,
            const vector<Viewport> &viewports
        ) {
            Colors.reserve(colors.size());
            ColorViews.reserve(colors.size());
            for (auto& color : colors)
            {
                Colors.emplace_back(color);
                ColorViews.emplace_back(nullptr);
            }
            DepthStencil = (Texture*)depthStencil;
            DepthStencilView = nullptr;
            Viewports = viewports;
            Init();
        }

        RenderTarget::RenderTarget(const vector<void*> &colorViews, const Viewport &viewport)
        {
            ColorViews.reserve(colorViews.size());
            for (auto& colorView : colorViews)
            {
                ColorViews.emplace_back(colorView);
            }
            Viewports.emplace_back(viewport);
        }

        RenderTarget::RenderTarget(const vector<void*> &colorViews, const vector <Viewport> &viewports)
        {
            ColorViews.reserve(colorViews.size());
            for (auto& colorView : colorViews)
            {
                ColorViews.emplace_back(colorView);
            }
            Viewports = viewports;
        }

        RenderTarget::~RenderTarget()
        {
            context::FreeRenderTarget(*this);
        }

        void RenderTarget::Init()
        {
            context::CreateRenderTarget(*this);
            AddWindowFrameResized(RenderTarget, 1);
        }

        void RenderTarget::Resize(s32 width, s32 height)
        {
            context::ResizeRenderTarget(*this, width, height);
        }

        void RenderTarget::WindowFrameResized(s32 width, s32 height)
        {
            Resize(width, height);
        }

        void RenderTarget::Bind()
        {
            context::BindRenderTarget(ColorViews, DepthStencilView);
        }

        void RenderTarget::BindColor(u32 index)
        {
            context::BindTexture(*Colors[index]);
        }

        void RenderTarget::BindDepth()
        {
            context::BindTexture(*DepthStencil);
        }

        void RenderTarget::ClearColor(u32 index, const glm::vec4 &color)
        {
            context::ClearColorTarget(ColorViews[index], color);
        }

        void RenderTarget::ClearDepth(const float depth)
        {
            context::ClearDepthTarget(DepthStencilView, depth);
        }

    }

}