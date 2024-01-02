#include <rendering/core/render_target.hpp>
#include <rendering/core/context.hpp>

namespace xpe {

    namespace render {

        sRenderTarget::sRenderTarget(const vector<cTexture*> &colors, sViewport* viewport)
        {
            Colors.reserve(colors.size());
            ColorViews.reserve(colors.size());
            for (auto& color : colors)
            {
                Colors.emplace_back(color);
                ColorViews.emplace_back(nullptr);
            }
            Viewport = viewport;
            Init();
        }

        sRenderTarget::sRenderTarget(const cTexture* depthStencil, sViewport* viewport)
        {
            DepthStencil = (cTexture*)depthStencil;
            DepthStencilView = nullptr;
            Viewport = viewport;
            Init();
        }

        sRenderTarget::sRenderTarget(
            const vector<cTexture*> &colors,
            const cTexture* depthStencil,
            sViewport* viewport
        ) {
            Colors.reserve(colors.size());
            ColorViews.reserve(colors.size());
            for (auto& color : colors)
            {
                Colors.emplace_back(color);
                ColorViews.emplace_back(nullptr);
            }
            DepthStencil = (cTexture*)depthStencil;
            DepthStencilView = nullptr;
            Viewport = viewport;
            Init();
        }

        sRenderTarget::sRenderTarget(const vector<void*> &colorViews, sViewport* viewport)
        {
            ColorViews.reserve(colorViews.size());
            for (auto& colorView : colorViews)
            {
                ColorViews.emplace_back(colorView);
            }
            Viewport = viewport;
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
            context::PSBindTexture(*Colors[index]);
        }

        void sRenderTarget::BindDepth()
        {
            context::PSBindTexture(*DepthStencil);
        }

        void sRenderTarget::Unbind()
        {
            context::UnbindRenderTarget();
        }

        void sRenderTarget::Clear()
        {
            usize size = ClearColors.size();
            for (int i = 0 ; i < size ; i++) {
                context::ClearColorTarget(ColorViews[i], ClearColors[i]);
            }
            if (ClearDepth != K_DEPTH_INVALID) {
                context::ClearDepthTarget(DepthStencilView, ClearDepth);
            }
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