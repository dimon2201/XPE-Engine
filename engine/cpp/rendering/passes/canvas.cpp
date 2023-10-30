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
            delete m_PresentTarget;
            context::FreeSampler(m_PresentSampler);
            RemoveWindowFrameResized();
        }

        void Canvas::Draw()
        {
            context::BindTextureSlot(0);
            context::BindRenderTarget(m_PresentTarget->ColorViews, m_PresentTarget->DepthStencilView, m_PresentTarget->Viewports);
            context::BindShader(*m_Shader);
            context::BindSampler(m_PresentSampler);
            context::BindTexture(*m_RenderTarget->Colors[m_BoundTargetIndex], 0);

            context::ClearColorTarget(m_PresentTarget->ColorViews[m_BoundTargetIndex], glm::vec4(0, 0, 0, 1));

            context::DrawQuad();

            context::Present();
        }

        void Canvas::WindowFrameResized(s32 width, s32 height)
        {
            context::ResizeSwapchain(*m_PresentTarget, width, height);
        }

        void Canvas::CreatePresentTarget()
        {
            m_PresentTarget = new RenderTarget(
                    vector<void*> { context::SwapchainTargetView },
                    m_ViewportBuffer->GetList()
            );
            m_PresentTarget->RemoveEventListeners();
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
