#include <rendering/passes/ssao_pass.hpp>
#include <rendering/core/context.hpp>

namespace xpe
{
    namespace render
    {
        cSsaoPass::cSsaoPass(const vector<sRenderPassBinding>& bindings, sViewport* viewport, u32 sampleCount)
        : cRenderPass(eType::POSTFX, bindings)
        {
            sTexture* ssaoColor = new sTexture();
            ssaoColor->Width = viewport->Width;
            ssaoColor->Height = viewport->Height;
            ssaoColor->Format = eTextureFormat::RGBA8;
            ssaoColor->SampleCount = sampleCount;
            ssaoColor->InitializeData = false;
            ssaoColor->EnableRenderTarget = true;
            ssaoColor->SetResizable(true);
            ssaoColor->Init();

            sTexture* ssaoDepth = new sTexture();
            ssaoDepth->Type = sTexture::eType::TEXTURE_2D_DEPTH_STENCIL;
            ssaoDepth->Width = viewport->Width;
            ssaoDepth->Height = viewport->Height;
            ssaoDepth->Format = eTextureFormat::R32_TYPELESS;
            ssaoDepth->SampleCount = sampleCount;
            ssaoDepth->InitializeData = false;
            ssaoDepth->EnableRenderTarget = true;
            ssaoDepth->SetResizable(true);
            ssaoDepth->Init();

            m_Pipeline->RenderTarget = new sRenderTarget({ ssaoColor }, ssaoDepth, viewport);
            m_Pipeline->RenderTarget->SetResizable(true);

            m_Pipeline->PSItemBuffers.emplace_back(&m_Buffer);
        }
    }
}