#include <rendering/core/render_target.h>
#include <rendering/core/context.hpp>

#include <core/user_input.hpp>

namespace xpe {

    namespace render {

        RenderPass::RenderPass(const vector<Texture*> &colors, const Viewport &viewport)
        : Viewports({ viewport })
        {
            Target.Colors.reserve(colors.size());
            Target.ColorViews.reserve(colors.size());
            for (auto color : colors)
            {
                Target.Colors.emplace_back(color);
                Target.ColorViews.emplace_back(nullptr);
            }
            Target.Viewports = &Viewports;
            Init();
        }

        RenderPass::RenderPass(const vector<Texture*> &colors, const vector<Viewport> &viewports)
        : Viewports(viewports)
        {
            Target.Colors.reserve(colors.size());
            Target.ColorViews.reserve(colors.size());
            for (auto color : colors)
            {
                Target.Colors.emplace_back(color);
                Target.ColorViews.emplace_back(nullptr);
            }
            Target.Viewports = &Viewports;
            Init();
        }

        RenderPass::RenderPass(const Texture* depthStencil, const Viewport &viewport)
        : Viewports({ viewport })
        {
            Target.DepthStencil = (Texture*)depthStencil;
            Target.DepthStencilView = nullptr;
            Target.Viewports = &Viewports;
            Init();
        }

        RenderPass::RenderPass(const Texture* depthStencil, const vector<Viewport> &viewports)
        : Viewports(viewports)
        {
            Target.DepthStencil = (Texture*)depthStencil;
            Target.DepthStencilView = nullptr;
            Target.Viewports = &Viewports;
            Init();
        }

        RenderPass::RenderPass(
            const vector<Texture*> &colors,
            const Texture* depthStencil,
            const Viewport& viewport
        ) : Viewports({ viewport })
        {
            Target.Colors.reserve(colors.size());
            Target.ColorViews.reserve(colors.size());
            for (auto color : colors)
            {
                Target.Colors.emplace_back(color);
                Target.ColorViews.emplace_back(nullptr);
            }
            Target.DepthStencil = (Texture*)depthStencil;
            Target.DepthStencilView = nullptr;
            Target.Viewports = &Viewports;
            Init();
        }

        RenderPass::RenderPass(
            const vector<Texture*> &colors,
            const Texture* depthStencil,
            const vector<Viewport> &viewports
        ) : Viewports(viewports)
        {
            Target.Colors.reserve(colors.size());
            Target.ColorViews.reserve(colors.size());
            for (auto color : colors)
            {
                Target.Colors.emplace_back(color);
                Target.ColorViews.emplace_back(nullptr);
            }
            Target.DepthStencil = (Texture*)depthStencil;
            Target.DepthStencilView = nullptr;
            Target.Viewports = &Viewports;
            Init();
        }

        RenderPass::RenderPass(
                RenderTarget* renderTarget,
                Viewport* viewport
        )
        {
            Target.Colors.reserve(renderTarget->Colors.size());
            Target.ColorViews.reserve(renderTarget->Colors.size());
            for (auto color : renderTarget->Colors)
            {
                Target.Colors.emplace_back(color);
                Target.ColorViews.emplace_back(nullptr);
            }
            Target.DepthStencil = renderTarget->DepthStencil;
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
            context::BindTexture(*Target.Colors[index]);
        }

        void RenderPass::BindDepth()
        {
            context::BindTexture(*Target.DepthStencil);
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