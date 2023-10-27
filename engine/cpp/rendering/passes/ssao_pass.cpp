#include <rendering/passes/ssao_pass.hpp>
#include <rendering/core/context.hpp>

namespace xpe
{
    namespace render
    {
        SSAOPass::SSAOPass(const vector<RenderPassBinding>& bindings, Viewport* viewport, u32 sampleCount)
        : RenderPass(eType::POSTFX, bindings)
        {
            m_Pipeline->PSBuffers.emplace_back(&m_Buffer);

            Texture* ssaoColor = new Texture();
            ssaoColor->Width = viewport->Width;
            ssaoColor->Height = viewport->Height;
            ssaoColor->Format = eTextureFormat::RGBA8;
            ssaoColor->SampleCount = sampleCount;
            ssaoColor->InitializeData = false;
            ssaoColor->EnableRenderTarget = true;
            ssaoColor->Init();

            Texture* ssaoDepth = new Texture();
            ssaoDepth->Type = Texture::eType::TEXTURE_2D_DEPTH_STENCIL;
            ssaoDepth->Width = viewport->Width;
            ssaoDepth->Height = viewport->Height;
            ssaoDepth->Format = eTextureFormat::R32_TYPELESS;
            ssaoDepth->SampleCount = sampleCount;
            ssaoDepth->InitializeData = false;
            ssaoDepth->EnableRenderTarget = true;
            ssaoDepth->Init();

            m_Pipeline->RenderTarget = new RenderTarget({ ssaoColor }, ssaoDepth, *viewport);

            BlendTarget target;
            target.Enable = false;
            m_Pipeline->Blending.Targets.push_back(target);
            m_Pipeline->Blending.IndependentBlendEnable = true;
        }

        void SSAOPass::Draw(Scene* scene)
        {
            context::DrawQuad();
        }

    }
}