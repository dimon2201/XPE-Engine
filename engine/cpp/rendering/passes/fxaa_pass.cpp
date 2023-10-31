#include <rendering/passes/fxaa_pass.hpp>
#include <rendering/core/context.hpp>

namespace xpe
{
    namespace render
    {

        cFxaaPass::cFxaaPass(const core::vector<sRenderPassBinding>& bindings, sViewport* viewport)
        : cRenderPass(eType::POSTFX, bindings)
        {
            sTexture* color = new sTexture();
            color->Width = viewport->Width;
            color->Height = viewport->Height;
            color->Format = eTextureFormat::HDR;
            color->SampleCount = 1;
            color->InitializeData = false;
            color->EnableRenderTarget = true;
            color->Init();

            sTexture* depth = new sTexture();
            depth->Type = sTexture::eType::TEXTURE_2D_DEPTH_STENCIL;
            depth->Width = viewport->Width;
            depth->Height = viewport->Height;
            depth->Format = eTextureFormat::R32_TYPELESS;
            depth->SampleCount = 1;
            depth->InitializeData = false;
            depth->EnableRenderTarget = true;
            depth->Init();

            m_Pipeline->RenderTarget = new sRenderTarget({color }, depth, *viewport);

            m_Sampler.Slot = 0;
            context::CreateSampler(m_Sampler);
            m_Pipeline->Samplers.emplace_back(&m_Sampler);
        }

        cFxaaPass::~cFxaaPass()
        {
            context::FreeSampler(m_Sampler);
            delete m_Pipeline->RenderTarget;
        }

    }
}