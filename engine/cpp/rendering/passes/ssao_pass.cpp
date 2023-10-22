#include <rendering/passes/ssao_pass.hpp>
#include <rendering/core/context.hpp>
#include <rendering/core/viewport.h>
#include <rendering/core/pipeline.h>
#include <ecs/scene.h>

namespace xpe
{
    namespace render
    {
        SSAOPass::SSAOPass(const core::vector<RenderPassBinding>& bindings, Viewport* viewport, core::Boolean useMSAA, core::usize msaaSampleCount)
            : RenderPass(bindings)
        {
            m_Buffer.Type = eBufferType::CONSTANT;
            m_Buffer.Usage = eBufferUsage::DYNAMIC;
            m_Buffer.NumElements = 1;
            m_Buffer.StructureSize = sizeof(SSAOBufferLayout);
            m_Buffer.InitialData = nullptr;
            m_Buffer.Slot = 0;
            context::CreateBuffer(m_Buffer);

            m_Pipeline->VSBuffers.emplace_back(&m_Buffer);

            Texture* color = new Texture();
            color->Width = viewport->Width;
            color->Height = viewport->Height;
            color->Format = eTextureFormat::RGBA8;
            color->SampleCount = useMSAA == core::K_TRUE ? msaaSampleCount : 1;
            color->InitializeData = false;
            color->EnableRenderTarget = true;
            color->Init();

            Texture* depth = new Texture();
            depth->Type = Texture::eType::TEXTURE_2D_DEPTH_STENCIL;
            depth->Width = viewport->Width;
            depth->Height = viewport->Height;
            depth->Format = eTextureFormat::R32_TYPELESS;
            depth->SampleCount = useMSAA == core::K_TRUE ? msaaSampleCount : 1;
            depth->InitializeData = false;
            depth->EnableRenderTarget = true;
            depth->Init();

            m_Pipeline->RenderTarget = new RenderTarget({ color }, depth, *viewport);

            BlendTarget target;
            target.Enable = false;
            m_Pipeline->Blending.Targets.push_back(target);
            m_Pipeline->Blending.IndependentBlendEnable = true;

            context::CreatePipeline(*m_Pipeline);
        }

        SSAOPass::~SSAOPass()
        {
        }

        void SSAOPass::Draw(Scene* scene)
        {
            context::DrawQuad();
        }
    }
}