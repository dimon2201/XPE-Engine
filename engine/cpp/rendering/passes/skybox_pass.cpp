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
            for (auto* stage : m_Pipeline->Shader->Stages)
            {
                if (stage->Type == sShaderStage::eType::PIXEL) {
                    stage->Samplers.emplace_back(&m_Sampler);
                }
            }
            m_Pipeline->DepthStencil.EnableDepth = true;
            m_Pipeline->DepthStencil.DepthFunc = eDepthStencilFunc::LESS_EQUAL;
            m_Pipeline->Rasterizer.CullMode = eCullMode::NONE;
        }

        void cSkyboxPass::DrawOpaque(cScene *scene)
        {
            auto& skybox = cSkyboxManager::Get().Skybox;
            if (skybox.Texture) {
                sGeometryInfo geometryInfo = skybox.GeometryInfo;
                sTexture& skyboxTexture = *skybox.Texture;
                context::BindPrimitiveTopology(geometryInfo.PrimitiveTopology);
                context::PSBindTexture(skyboxTexture);
                context::DrawIndexed(
                    geometryInfo.IndexCount,
                    1,
                    geometryInfo.VertexOffset,
                    geometryInfo.IndexOffset
                );
            }
        }

    }

}