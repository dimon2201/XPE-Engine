#include <rendering/core/render_target.hpp>
#include <rendering/core/context.hpp>

namespace xpe {

    namespace render {

        cRenderTarget::cRenderTarget(const vector<cTexture*> &colors)
        {
            m_Colors.reserve(colors.size());
            m_ColorViews.reserve(colors.size());
            for (auto& color : colors)
            {
                m_Colors.emplace_back(color);
                m_ColorViews.emplace_back(nullptr);
            }

            Init();
        }

        cRenderTarget::cRenderTarget(const cTexture* depthStencil)
        {
            m_DepthStencil = (cTexture*)depthStencil;
            m_DepthStencilView = nullptr;

            Init();
        }

        cRenderTarget::cRenderTarget(
            const vector<cTexture*> &colors,
            const cTexture* depthStencil
        ) {
            m_Colors.reserve(colors.size());
            m_ColorViews.reserve(colors.size());
            for (auto& color : colors)
            {
                m_Colors.emplace_back(color);
                m_ColorViews.emplace_back(nullptr);
            }
            m_DepthStencil = (cTexture*)depthStencil;
            m_DepthStencilView = nullptr;

            Init();
        }

        cRenderTarget::cRenderTarget(const vector<void*> &colorViews)
        {
            m_ColorViews.reserve(colorViews.size());
            for (auto& colorView : colorViews)
            {
                m_ColorViews.emplace_back(colorView);
            }
        }

        cRenderTarget::~cRenderTarget()
        {
            context::FreeRenderTarget(*this);
            RemoveWindowFrameResized();
        }

        void cRenderTarget::Init()
        {
            context::CreateRenderTarget(*this);
        }

        void cRenderTarget::Resize(s32 width, s32 height)
        {
            context::ResizeRenderTarget(*this, width, height);
        }

        void cRenderTarget::ResizeColors(s32 width, s32 height)
        {
            for (auto* color : m_Colors) {
                color->Resize(width, height);
            }
        }

        void cRenderTarget::WindowFrameResized(s32 width, s32 height)
        {
            Resize(width, height);
        }

        void cRenderTarget::Bind()
        {
            context::BindRenderTarget(m_ColorViews, m_DepthStencilView);
        }

        void cRenderTarget::BindColor(u32 index)
        {
            context::PSBindTexture(cResource::eViewType::SRV, 0, m_Colors[index]->GetInstance(), m_Colors[index]->GetSRVInstance());
        }

        void cRenderTarget::BindDepth()
        {
            context::PSBindTexture(cResource::eViewType::SRV, 0, m_DepthStencil->GetInstance(), m_DepthStencil->GetSRVInstance());
        }

        void cRenderTarget::Unbind()
        {
            context::UnbindRenderTarget();
        }

        void cRenderTarget::Clear()
        {
            usize size = m_ClearColors.size();
            for (int i = 0 ; i < size ; i++) {
                context::ClearColorTarget(m_ColorViews[i], m_ClearColors[i]);
            }
            if (m_ClearDepth != K_DEPTH_INVALID && m_DepthStencilView != nullptr) {
                context::ClearDepthTarget(m_DepthStencilView, m_ClearDepth);
            }
        }

        void cRenderTarget::SetResizable(bool resizable)
        {
            m_Resizable = resizable;
            if (resizable) {
                AddWindowFrameResized(cRenderTarget, eWindowFrameResizedPriority::RENDER_TARGET);
            } else {
                RemoveWindowFrameResized();
            }
        }

    }

}