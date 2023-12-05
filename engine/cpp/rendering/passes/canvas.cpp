#include <rendering/passes/canvas.hpp>

namespace xpe {

    namespace render {

        cCanvas::cCanvas(sShader* shader, sTexture* texture, sViewport* viewport)
        : m_Shader(shader), m_Texture(texture), m_Viewport(viewport)
        {
            CreatePresentTarget();
            CreatePresentSampler();
            AddWindowFrameResized(cCanvas, 1);
        }

        cCanvas::~cCanvas()
        {
            delete m_PresentTarget;
            context::FreeSampler(m_PresentSampler);
            RemoveWindowFrameResized();
        }

        void cCanvas::Draw()
        {
            context::PSBindTextureSlot(0);
            context::BindRenderTarget(m_PresentTarget->ColorViews, m_PresentTarget->DepthStencilView, m_PresentTarget->Viewport);
            context::BindShader(*m_Shader);
            context::PSBindSampler(m_PresentSampler);
            context::PSBindTexture(*m_Texture, 0);

            context::ClearColorTarget(m_PresentTarget->ColorViews[m_BoundTargetIndex], glm::vec4(0, 0, 0, 1));

            context::DrawQuad();

            context::Present();
        }

        void cCanvas::WindowFrameResized(s32 width, s32 height)
        {
            context::ResizeSwapchain(*m_PresentTarget, width, height);
        }

        void cCanvas::CreatePresentTarget()
        {
            m_PresentTarget = new sRenderTarget(
                    vector<void*> { context::SwapchainTargetView },
                    m_Viewport
            );
            m_PresentTarget->SetResizable(false);
        }

        void cCanvas::CreatePresentSampler()
        {
            m_PresentSampler.Filter   = sSampler::eFilter::MIN_MAG_MIP_POINT;
            m_PresentSampler.AddressU = sSampler::eAddress::CLAMP;
            m_PresentSampler.AddressV = sSampler::eAddress::CLAMP;
            m_PresentSampler.AddressW = sSampler::eAddress::CLAMP;
            m_PresentSampler.AnisotropyLevel = 1;

            context::CreateSampler(m_PresentSampler);
        }

    }

}
