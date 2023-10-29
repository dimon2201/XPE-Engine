#include <rendering/passes/skybox_pass.h>

#include <ecs/globals.h>

namespace xpe {

    namespace render {

        SkyboxPass::SkyboxPass(const vector<RenderPassBinding>& bindings)
        : RenderPass(eType::OPAQUE, bindings) {
            m_Sampler.Filter = TextureSampler::eFilter::MIN_MAG_MIP_LINEAR;
            context::CreateSampler(m_Sampler);
        }

        SkyboxPass::~SkyboxPass()
        {
            context::FreeSampler(m_Sampler);
        }

        void SkyboxPass::InitOpaque()
        {
            RenderPass::InitOpaque();
            m_Pipeline->Samplers.emplace_back(&m_Sampler);
            m_Pipeline->DepthStencil.EnableDepth = true;
            m_Pipeline->DepthStencil.DepthFunc = eDepthStencilFunc::LESS_EQUAL;
            m_Pipeline->Rasterizer.CullMode = eCullMode::NONE;
        }

        void SkyboxPass::DrawOpaque(Scene *scene)
        {
            Skybox* skybox = scene->GetGlobal<Skybox>();
            if (skybox) {
                if (skybox->Texture) {
                    Geometry& skyboxGeometry = skybox->Geometry;
                    Texture& skyboxTexture = *skybox->Texture;
                    context::BindPrimitiveTopology(skyboxGeometry.PrimitiveTopology);
                    context::BindTexture(skyboxTexture);
                    context::DrawIndexed(
                            skyboxGeometry.Indices.size(),
                            1,
                            skyboxGeometry.VertexOffset,
                            skyboxGeometry.IndexOffset
                    );
                }
            }
        }

    }

}