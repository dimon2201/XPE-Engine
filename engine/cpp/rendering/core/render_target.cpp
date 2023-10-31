#include <rendering/core/render_target.h>
#include <rendering/core/context.hpp>

namespace xpe {

    namespace render {

        sRenderTarget::sRenderTarget(const vector<sTexture*> &colors, const sViewport &viewport)
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

        sRenderTarget::sRenderTarget(const vector<sTexture*> &colors, const vector<sViewport> &viewports)
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

        sRenderTarget::sRenderTarget(const sTexture* depthStencil, const sViewport &viewport)
        {
            DepthStencil = (sTexture*)depthStencil;
            DepthStencilView = nullptr;
            Viewports.emplace_back(viewport);
            Init();
        }

        sRenderTarget::sRenderTarget(const sTexture* depthStencil, const vector<sViewport> &viewports)
        {
            DepthStencil = (sTexture*)depthStencil;
            DepthStencilView = nullptr;
            Viewports = viewports;
            Init();
        }

        sRenderTarget::sRenderTarget(
            const vector<sTexture*> &colors,
            const sTexture* depthStencil,
            const sViewport& viewport
        ) {
            Colors.reserve(colors.size());
            ColorViews.reserve(colors.size());
            for (auto& color : colors)
            {
                Colors.emplace_back(color);
                ColorViews.emplace_back(nullptr);
            }
            DepthStencil = (sTexture*)depthStencil;
            DepthStencilView = nullptr;
            Viewports.emplace_back(viewport);
            Init();
        }

        sRenderTarget::sRenderTarget(
            const vector<sTexture*> &colors,
            const sTexture* depthStencil,
            const vector<sViewport> &viewports
        ) {
            Colors.reserve(colors.size());
            ColorViews.reserve(colors.size());
            for (auto& color : colors)
            {
                Colors.emplace_back(color);
                ColorViews.emplace_back(nullptr);
            }
            DepthStencil = (sTexture*)depthStencil;
            DepthStencilView = nullptr;
            Viewports = viewports;
            Init();
        }

        sRenderTarget::sRenderTarget(const vector<void*> &colorViews, const sViewport &viewport)
        {
            ColorViews.reserve(colorViews.size());
            for (auto& colorView : colorViews)
            {
                ColorViews.emplace_back(colorView);
            }
            Viewports.emplace_back(viewport);
        }

        sRenderTarget::sRenderTarget(const vector<void*> &colorViews, const vector <sViewport> &viewports)
        {
            ColorViews.reserve(colorViews.size());
            for (auto& colorView : colorViews)
            {
                ColorViews.emplace_back(colorView);
            }
            Viewports = viewports;
        }

        sRenderTarget::~sRenderTarget()
        {
            context::FreeRenderTarget(*this);
            RemoveWindowFrameResized();
        }

        void sRenderTarget::Init()
        {
            context::CreateRenderTarget(*this);
        }

        void sRenderTarget::Resize(s32 width, s32 height)
        {
            context::ResizeRenderTarget(*this, width, height);
        }

        void sRenderTarget::ResizeColors(s32 width, s32 height)
        {
            for (auto* color : Colors) {
                color->Resize(width, height);
            }
        }

        void sRenderTarget::WindowFrameResized(s32 width, s32 height)
        {
            Resize(width, height);
        }

        void sRenderTarget::Bind()
        {
            context::BindRenderTarget(ColorViews, DepthStencilView);
        }

        void sRenderTarget::BindColor(u32 index)
        {
            context::BindTexture(*Colors[index]);
        }

        void sRenderTarget::BindDepth()
        {
            context::BindTexture(*DepthStencil);
        }

        void sRenderTarget::ClearColor(u32 index, const glm::vec4 &color)
        {
            context::ClearColorTarget(ColorViews[index], color);
        }

        void sRenderTarget::ClearDepth(const float depth)
        {
            context::ClearDepthTarget(DepthStencilView, depth);
        }

        void sRenderTarget::SetResizable(bool resizable)
        {
            m_Resizable = resizable;
            if (resizable) {
                AddWindowFrameResized(sRenderTarget, eWindowFrameResizedPriority::RENDER_TARGET);
            } else {
                RemoveWindowFrameResized();
            }
        }

    }

}