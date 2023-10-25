#include <rendering/passes/ssao_pass.hpp>
#include <rendering/geometry/geometry_manager.h>

namespace xpe
{
    namespace render
    {
        SSAOPass::SSAOPass(const vector<RenderPassBinding>& bindings, Viewport* viewport) : RenderPass(bindings)
        {
            m_Buffer.Type = eBufferType::CONSTANT;
            m_Buffer.Usage = eBufferUsage::DYNAMIC;
            m_Buffer.NumElements = 1;
            m_Buffer.StructureSize = sizeof(SSAOBufferLayout);
            m_Buffer.InitialData = nullptr;
            m_Buffer.Slot = 0;

            context::CreateBuffer(m_Buffer);

            m_Pipeline->VSBuffers.emplace_back(&m_Buffer);

            Texture* ssaoColor = new Texture();
            ssaoColor->Width = viewport->Width;
            ssaoColor->Height = viewport->Height;
            ssaoColor->Format = eTextureFormat::RGBA8;
            ssaoColor->InitializeData = false;
            ssaoColor->EnableRenderTarget = true;
            ssaoColor->Init();

            Texture* ssaoDepth = new Texture();
            ssaoDepth->Type = Texture::eType::TEXTURE_2D_DEPTH_STENCIL;
            ssaoDepth->Width = viewport->Width;
            ssaoDepth->Height = viewport->Height;
            ssaoDepth->Format = eTextureFormat::R32_TYPELESS;
            ssaoDepth->InitializeData = false;
            ssaoDepth->EnableRenderTarget = true;
            ssaoDepth->Init();

            m_Pipeline->RenderTarget = new RenderTarget({ ssaoColor }, ssaoDepth, *viewport);

            BlendTarget target;
            target.Enable = false;
            m_Pipeline->Blending.Targets.push_back(target);
            m_Pipeline->Blending.IndependentBlendEnable = true;
        }

        void SSAOPass::Update(Scene *scene)
        {
            // TODO: very strange buffer update. In that case, better to replace CONSTANT buffer with STRUCTURED buffer.
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