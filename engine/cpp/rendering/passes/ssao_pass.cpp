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
            : RenderPass(bindings, nullptr)
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

            context::CreatePipeline(*m_Pipeline);
        }

        SSAOPass::~SSAOPass()
        {
        }

        void SSAOPass::Update(Scene* scene)
        {
            // TODO: very strange buffer update. In that case, better to replace CONSTANT buffer with STRUCTURED buffer.
            m_BufferData.SSAODirectionCount = 4;
            m_BufferData.SSAOSampleCount = 4;
            void* memory = context::Map(m_Buffer, 0, eMapType::WRITE_DISCARD);
            memcpy(memory, &m_BufferData, sizeof(SSAOBufferLayout));
            context::Unmap(m_Buffer);
        }

        void SSAOPass::Draw(Scene* scene)
        {
            context::DrawQuad();
        }
    }
}