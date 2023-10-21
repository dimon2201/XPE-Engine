#include "rendering/passes/fxaa_pass.hpp"
#include "rendering/core/context.hpp"
#include "ecs/scene.h"

namespace xpe
{
    namespace render
    {
        FXAAPass::FXAAPass(const core::vector<RenderPassBinding>& bindings, Viewport* viewport)
            : RenderPass(bindings)
        {
            Texture* color = new Texture();
            color->Width = viewport->Width;
            color->Height = viewport->Height;
            color->Format = eTextureFormat::RGBA8;
            color->InitializeData = false;
            color->EnableRenderTarget = true;
            color->Init();

            Texture* depth = new Texture();
            depth->Type = Texture::eType::TEXTURE_2D_DEPTH_STENCIL;
            depth->Width = viewport->Width;
            depth->Height = viewport->Height;
            depth->Format = eTextureFormat::R32_TYPELESS;
            depth->InitializeData = false;
            depth->EnableRenderTarget = true;
            depth->Init();

            m_Pipeline->RenderTarget = new RenderTarget({ color }, depth, *viewport);

            m_Sampler.Slot = 0;
            context::CreateSampler(m_Sampler);
            m_Pipeline->Samplers.emplace_back(&m_Sampler);

            BlendTarget target;
            target.Enable = false;
            m_Pipeline->Blending.Targets.push_back(target);

            context::CreatePipeline(*m_Pipeline);
        }

        FXAAPass::~FXAAPass()
        {
            delete m_Pipeline->RenderTarget;
        }

        void FXAAPass::Draw(Scene* scene)
        {
            context::DrawQuad();
        }
    }
}