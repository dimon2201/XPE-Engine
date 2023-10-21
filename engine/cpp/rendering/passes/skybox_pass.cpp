#include <rendering/passes/skybox_pass.h>

#include <ecs/scene.h>
#include <ecs/globals.h>

namespace xpe {

    namespace render {

        SkyboxPass::SkyboxPass(
                const vector<RenderPassBinding>& bindings
        ) : RenderPass(bindings)
        {
            m_Cube = GeometryManager::AddGeometry(Cube());

            m_Pipeline->PrimitiveTopology = m_Cube->PrimitiveTopology;

            context::CreateSampler(m_Sampler);
            m_Pipeline->Textures.emplace_back(nullptr);
            m_Pipeline->Samplers.emplace_back(&m_Sampler);

            m_Pipeline->DepthStencil.EnableDepth = true;
            m_Pipeline->DepthStencil.DepthFunc = eDepthStencilFunc::LESS_EQUAL;
            
            BlendTarget target;
            target.Enable = false;
            m_Pipeline->Blending.Targets.push_back(target);

            context::CreatePipeline(*m_Pipeline);
        }

        SkyboxPass::~SkyboxPass()
        {
            context::FreeSampler(m_Sampler);
        }

        void SkyboxPass::Draw(Scene* scene)
        {
            Skybox* skybox = scene->GetGlobal<Skybox>();
            m_Pipeline->Textures[0] = skybox->CubeTexture.Get();
            context::DrawIndexed(36);
        }

    }

}