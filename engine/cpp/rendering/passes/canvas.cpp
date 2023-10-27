#include <rendering/passes/canvas.hpp>

namespace xpe {

    namespace render {

        Canvas::Canvas(Shader* shader, RenderTarget* renderTarget, ViewportBuffer* viewportBuffer)
        : m_Shader(shader), m_RenderTarget(renderTarget), m_ViewportBuffer(viewportBuffer)
        {
            CreatePresentTarget();
            CreatePresentSampler();
            AddWindowFrameResized(Canvas, 1);
        }

        Canvas::~Canvas()
        {
            context::FreeSampler(m_PresentSampler);
        }

        void Canvas::Clear(const glm::vec4& color)
        {
            context::BindTextureSlot(0);
            m_RenderTarget->Bind();
            m_RenderTarget->ClearColor(m_BoundTargetIndex, color);
            m_RenderTarget->ClearDepth(1.0f);
        }

        void Canvas::Present()
        {
            context::BindRenderTarget(m_PresentTarget->ColorViews, m_PresentTarget->DepthStencilView, m_PresentTarget->Viewports);
            context::BindShader(*m_Shader);
            context::BindSampler(m_PresentSampler);
            m_RenderTarget->BindColor(m_BoundTargetIndex);

            context::ClearColorTarget(m_PresentTarget->ColorViews[m_BoundTargetIndex], m_ClearColor);

            context::DrawQuad();

            context::Present();
        }

        void Canvas::WindowFrameResized(s32 width, s32 height)
        {
            context::ResizeSwapchain(*m_PresentTarget, width, height);
        }

        void Canvas::CreatePresentTarget()
        {
            m_PresentTarget.Create(
                    vector<void*> { context::SwapchainTargetView },
                    m_ViewportBuffer->GetList()
            );
        }

        void Canvas::CreatePresentSampler()
        {
            m_PresentSampler.Filter   = TextureSampler::eFilter::MIN_MAG_MIP_POINT;
            m_PresentSampler.AddressU = TextureSampler::eAddress::CLAMP;
            m_PresentSampler.AddressV = TextureSampler::eAddress::CLAMP;
            m_PresentSampler.AddressW = TextureSampler::eAddress::CLAMP;
            m_PresentSampler.AnisotropyLevel = 1;

            context::CreateSampler(m_PresentSampler);
        }

    }

}
