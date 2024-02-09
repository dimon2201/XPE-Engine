#include <canvas.hpp>
#include <bindings.hpp>

namespace xpe
{
    namespace render
    {
        cCanvas::cCanvas(sViewport* viewport)
        {
            m_Shader = new cDefaultShader(cShader::eCategory::NONE, "canvas");
            m_Shader->VertexStage = MShader::GetFromFile(cShaderStage::eType::VERTEX, "res/shaders/screen.vs");
            m_Shader->PixelStage = MShader::GetFromFile(cShaderStage::eType::PIXEL, "res/shaders/canvas.ps");
            m_Viewport = viewport;
            Viewports::Canvas = viewport;
            MShader::SetShader(m_Shader);

            CreatePresentTarget();
            CreatePresentSampler();

            MEvent::WindowFrameResized.Add((u64)this, 1, [this](int w, int h)
            {
                WindowFrameResized(w, h);
            });
        }

        cCanvas::~cCanvas()
        {
            delete m_Shader;
            delete m_PresentTarget;
            delete m_PresentSampler;
            MEvent::WindowFrameResized.Remove((u64) this);
        }

        void cCanvas::Draw()
        {
            context::PSBindTextureSlot(0);
            m_PresentTarget->Bind();
            context::VSBindStage(*m_Shader->VertexStage);
            context::PSBindStage(*m_Shader->PixelStage);
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
            m_PresentTarget = new cRenderTarget(
                    vector<void*> { context::SwapchainTargetView },
                    Viewports::Canvas
            );
            m_PresentTarget->SetResizable(false);
        }

        void cCanvas::CreatePresentSampler()
        {
            m_PresentSampler          = new cSampler();
            m_PresentSampler->Filter   = cSampler::eFilter::MIN_MAG_MIP_POINT;
            m_PresentSampler->AddressU = cSampler::eAddress::CLAMP;
            m_PresentSampler->AddressV = cSampler::eAddress::CLAMP;
            m_PresentSampler->AddressW = cSampler::eAddress::CLAMP;
            m_PresentSampler->AnisotropyLevel = 1;
            m_PresentSampler->Init();
        }
    }
}
