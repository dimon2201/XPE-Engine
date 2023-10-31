#include <rendering/passes/skybox_pass.h>
#include <rendering/skybox_manager.h>

namespace xpe {

    namespace render {

        cSkyboxPass::cSkyboxPass(const vector<sRenderPassBinding>& bindings)
        : cRenderPass(eType::OPAQUE, bindings) {
            m_Sampler.Filter = sSampler::eFilter::MIN_MAG_MIP_LINEAR;
            context::CreateSampler(m_Sampler);
        }

        cSkyboxPass::~cSkyboxPass()
        {
            context::FreeSampler(m_Sampler);
        }

        void cSkyboxPass::InitOpaque()
        {
            cRenderPass::InitOpaque();
            m_Pipeline->Samplers.emplace_back(&m_Sampler);
            m_Pipeline->DepthStencil.EnableDepth = true;
            m_Pipeline->DepthStencil.DepthFunc = eDepthStencilFunc::LESS_EQUAL;
            m_Pipeline->Rasterizer.CullMode = eCullMode::NONE;
        }

        void cSkyboxPass::DrawOpaque(cScene *scene)
        {
            auto& skybox = cSkyboxManager::Get().Skybox;
            if (skybox.Texture) {
                sGeometry& skyboxGeometry = skybox.Geometry;
                sTexture& skyboxTexture = *skybox.Texture;
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