#include <rendering/core/render_target.h>
#include <rendering/core/context.hpp>

#include <core/user_input.hpp>

namespace xpe {

    namespace render {

        RenderPass::RenderPass(const vector<Texture> &colors, const Viewport &viewport)
        : Colors(colors), Viewports({ viewport })
        {
            Target.Colors.reserve(Colors.size());
            Target.ColorViews.reserve(Colors.size());
            for (auto& color : Colors)
            {
                Target.Colors.emplace_back(&color);
                Target.ColorViews.emplace_back(nullptr);
            }
            Target.Viewports = &Viewports;
            Init();
        }

        RenderPass::RenderPass(const vector<Texture> &colors, const vector<Viewport> &viewports)
        : Colors(colors), Viewports(viewports)
        {
            Target.Colors.reserve(Colors.size());
            Target.ColorViews.reserve(Colors.size());
            for (auto& color : Colors)
            {
                Target.Colors.emplace_back(&color);
                Target.ColorViews.emplace_back(nullptr);
            }
            Target.Viewports = &Viewports;
            Init();
        }

        RenderPass::RenderPass(const Texture &depthStencil, const Viewport &viewport)
        : DepthStencil(depthStencil), Viewports({ viewport })
        {
            Target.DepthStencil = &DepthStencil;
            Target.DepthStencilView = nullptr;
            Target.Viewports = &Viewports;
            Init();
        }

        RenderPass::RenderPass(const Texture &depthStencil, const vector<Viewport> &viewports)
        : DepthStencil(depthStencil), Viewports(viewports)
        {
            Target.DepthStencil = &DepthStencil;
            Target.DepthStencilView = nullptr;
            Target.Viewports = &Viewports;
            Init();
        }

        RenderPass::RenderPass(
            const vector<Texture> &colors,
            const Texture &depthStencil,
            const Viewport& viewport
        ) : Colors(colors), DepthStencil(depthStencil), Viewports({ viewport })
        {
            Target.Colors.reserve(Colors.size());
            Target.ColorViews.reserve(Colors.size());
            for (auto& color : Colors)
            {
                Target.Colors.emplace_back(&color);
                Target.ColorViews.emplace_back(nullptr);
            }
            Target.DepthStencil = &DepthStencil;
            Target.DepthStencilView = nullptr;
            Target.Viewports = &Viewports;
            Init();
        }

        RenderPass::RenderPass(
            const vector<Texture> &colors,
            const Texture &depthStencil,
            const vector<Viewport> &viewports
        ) : Colors(colors), DepthStencil(depthStencil), Viewports(viewports)
        {
            Target.Colors.reserve(Colors.size());
            Target.ColorViews.reserve(Colors.size());
            for (auto& color : Colors)
            {
                Target.Colors.emplace_back(&color);
                Target.ColorViews.emplace_back(nullptr);
            }
            Target.DepthStencil = &DepthStencil;
            Target.DepthStencilView = nullptr;
            Target.Viewports = &Viewports;
            Init();
        }

        RenderPass::~RenderPass()
        {
            context::FreeRenderTarget(Target);
        }

        void RenderPass::Init()
        {
            context::CreateRenderTarget(Target);
            AddWindowFrameResized(RenderPass, 1);
        }

        void RenderPass::Resize(s32 width, s32 height)
        {
            context::ResizeRenderTarget(Target, width, height);
        }

        void RenderPass::WindowFrameResized(s32 width, s32 height)
        {
            Resize(width, height);
        }

        void RenderPass::Bind()
        {
            context::BindRenderTarget(Target.ColorViews, Target.DepthStencilView);
        }

        void RenderPass::BindColor(u32 index)
        {
            context::BindTexture(Colors[index]);
        }

        void RenderPass::BindDepth()
        {
            context::BindTexture(DepthStencil);
        }

        void RenderPass::ClearColor(u32 index, const glm::vec4 &color)
        {
            context::ClearColorTarget(Target.ColorViews[index], color);
        }

        void RenderPass::ClearDepth(const float depth)
        {
            context::ClearDepthTarget(Target.DepthStencilView, depth);
        }

    }

}