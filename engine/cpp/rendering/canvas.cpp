#include <rendering/canvas.hpp>
#include <rendering/camera_manager.h>
#include <rendering/bindings.h>

namespace xpe {

    namespace render {

        cCanvas::cCanvas()
        {
            m_Shader = new cDefaultShader(cShader::eCategory::NONE, "canvas");
            m_Shader->VertexStage = cShaderManager::GetFromFile(sShaderStage::eType::VERTEX, "engine_shaders/passes/screen.vs");
            m_Shader->PixelStage = cShaderManager::GetFromFile(sShaderStage::eType::PIXEL, "engine_shaders/passes/canvas.ps");
            m_Viewport = cCameraManager::GetViewport();
            cShaderManager::SetShader(m_Shader);

            CreatePresentTarget();
            CreatePresentSampler();
            AddWindowFrameResized(cCanvas, 1);
        }

        cCanvas::~cCanvas()
        {
            delete m_Shader;
            delete m_PresentTarget;
            delete m_PresentSampler;
            RemoveWindowFrameResized();
        }

        void cCanvas::Draw()
        {
            context::PSBindTextureSlot(0);
            context::BindRenderTarget(m_PresentTarget->ColorViews, m_PresentTarget->DepthStencilView, m_PresentTarget->Viewport);
            context::BindVSStage(*m_Shader->VertexStage);
            context::BindPSStage(*m_Shader->PixelStage);
            context::PSBindSampler(*m_PresentSampler);
            context::PSBindTexture(*Textures::Canvas, 0);

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
            m_PresentSampler          = new sSampler();
            m_PresentSampler->Filter   = sSampler::eFilter::MIN_MAG_MIP_POINT;
            m_PresentSampler->AddressU = sSampler::eAddress::CLAMP;
            m_PresentSampler->AddressV = sSampler::eAddress::CLAMP;
            m_PresentSampler->AddressW = sSampler::eAddress::CLAMP;
            m_PresentSampler->AnisotropyLevel = 1;
            m_PresentSampler->Init();
        }

        void cCanvas::SetViewport(sViewport* viewport)
        {
            m_Viewport = viewport;
            WindowFrameResized(viewport->Width, viewport->Height);
        }

    }

}
