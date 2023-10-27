#include <rendering/passes/fxaa_pass.hpp>
#include <rendering/core/context.hpp>

namespace xpe
{
    namespace render
    {

        FXAAPass::FXAAPass(const core::vector<RenderPassBinding>& bindings, Viewport* viewport)
        : RenderPass(eType::POSTFX, bindings)
        {
            Texture* color = new Texture();
            color->Width = viewport->Width;
            color->Height = viewport->Height;
            color->Format = eTextureFormat::RGBA8;
            color->SampleCount = 1;
            color->InitializeData = false;
            color->EnableRenderTarget = true;
            color->Init();

            Texture* depth = new Texture();
            depth->Type = Texture::eType::TEXTURE_2D_DEPTH_STENCIL;
            depth->Width = viewport->Width;
            depth->Height = viewport->Height;
            depth->Format = eTextureFormat::R32_TYPELESS;
            depth->SampleCount = 1;
            depth->InitializeData = false;
            depth->EnableRenderTarget = true;
            depth->Init();

            m_Pipeline->RenderTarget = new RenderTarget({ color }, depth, *viewport);

            m_Sampler.Slot = 0;
            context::CreateSampler(m_Sampler);
            m_Pipeline->Samplers.emplace_back(&m_Sampler);
        }

        FXAAPass::~FXAAPass()
        {
            context::FreeSampler(m_Sampler);
            delete m_Pipeline->RenderTarget;
        }

    }
}