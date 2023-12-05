#include <rendering/core/render_target.h>
#include <rendering/core/context.hpp>

namespace xpe {

    namespace render {

        sRenderTarget::sRenderTarget(const vector<sTexture*> &colors, sViewport* viewport)
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

        sRenderTarget::sRenderTarget(const sTexture* depthStencil, sViewport* viewport)
        {
            DepthStencil = (sTexture*)depthStencil;
            DepthStencilView = nullptr;
            Viewport = viewport;
            Init();
        }

        sRenderTarget::sRenderTarget(
            const vector<sTexture*> &colors,
            const sTexture* depthStencil,
            sViewport* viewport
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