#include <render_target.hpp>
#include <render_context.hpp>

namespace xpe
{
    namespace render
    {
        cRenderTarget::cRenderTarget(const vector<cTexture*> &colors, sViewport* viewport)
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

        cRenderTarget::cRenderTarget(const cTexture* depthStencil, sViewport* viewport)
        {
            DepthStencil = (cTexture*)depthStencil;
            DepthStencilView = nullptr;
            Viewport = viewport;
            Init();
        }

        cRenderTarget::cRenderTarget(
            const vector<cTexture*> &colors,
            const cTexture* depthStencil,
            sViewport* viewport
        )
        {
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

        cRenderTarget::cRenderTarget(const vector<void*> &colorViews, sViewport* viewport)
        {
            ColorViews.reserve(colorViews.size());
            for (auto& colorView : colorViews)
            {
                ColorViews.emplace_back(colorView);
            }
            Viewport = viewport;
        }

        cRenderTarget::~cRenderTarget()
        {
            context::FreeRenderTarget(*this);
            MEvent::WindowFrameResized.Remove((u64)this);
        }

        void cRenderTarget::Init()
        {
            context::CreateRenderTarget(*this);
        }

        void cRenderTarget::Resize(s32 width, s32 height)
        {
            for (auto* color : Colors)
            {
                color->Resize(width, height);
            }
            if (DepthStencil != nullptr)
            {
                DepthStencil->Resize(width, height);
            }
            context::ResizeRenderTarget(*this, width, height);
        }

        void cRenderTarget::ResizeColor(s32 width, s32 height)
        {
            for (auto* color : Colors)
            {
                color->Resize(width, height);
            }
            context::ResizeRenderTarget(*this, width, height);
        }

        void cRenderTarget::WindowFrameResized(s32 width, s32 height)
        {
            Resize(width, height);
        }

        void cRenderTarget::Bind()
        {
            context::BindRenderTarget(ColorViews, DepthStencilView);
            context::BindViewport(Viewport);
        }

        void cRenderTarget::Unbind()
        {
            context::UnbindRenderTarget();
            context::UnbindViewport();
        }

        void cRenderTarget::Clear()
        {
            usize size = ClearColors.size();
            for (int i = 0 ; i < size ; i++)
            {
                context::ClearColorTarget(ColorViews[i], ClearColors[i]);
            }
            if (ClearDepth != K_DEPTH_INVALID)
            {
                context::ClearDepthTarget(DepthStencilView, ClearDepth);
            }
        }

        void cRenderTarget::SetResizable(bool resizable)
        {
            m_Resizable = resizable;
            if (resizable)
            {
                MEvent::WindowFrameResized.Add((u64)this, eWindowFrameResizedPriority::RENDER_TARGET, [this](int w, int h)
                {
                    WindowFrameResized(w, h);
                });
            }
            else
            {
                MEvent::WindowFrameResized.Remove((u64)this);
            }
        }
    }
}