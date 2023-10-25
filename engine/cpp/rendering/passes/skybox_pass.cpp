#include <rendering/passes/skybox_pass.h>
#include <rendering/geometry/geometry_manager.h>

#include <ecs/globals.h>

namespace xpe {

    namespace render {

        SkyboxPass::SkyboxPass(eType type, const vector<RenderPassBinding>& bindings) : RenderPass(type, bindings)
        {
            m_Cube = GeometryManager::AddGeometry(Cube());

            m_Pipeline->PrimitiveTopology = m_Cube->PrimitiveTopology;

            context::CreateSampler(m_Sampler);
            m_Pipeline->Textures.emplace_back(nullptr);
            m_Pipeline->Samplers.emplace_back(&m_Sampler);

            m_Pipeline->DepthStencil.EnableDepth = true;
            m_Pipeline->DepthStencil.DepthFunc = eDepthStencilFunc::LESS_EQUAL;
        }

        SkyboxPass::~SkyboxPass()
        {
            context::FreeSampler(m_Sampler);
        }

        void SkyboxPass::DrawOpaque(Scene *scene)
        {
            Skybox* skybox = scene->GetGlobal<Skybox>();
            m_Pipeline->Textures[0] = skybox->CubeTexture.Get();
            context::DrawIndexed(36);
        }

    }

}