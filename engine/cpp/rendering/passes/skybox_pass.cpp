#include <rendering/passes/skybox_pass.h>

#include <ecs/scene.h>
#include <ecs/globals.h>

namespace xpe {

    namespace render {

        SkyboxPass::SkyboxPass(
                const vector<RenderPassBinding>& bindings,
                RenderTarget* output
        ) : RenderPass(bindings, output)
        {
            m_Cube = GeometryManager::AddGeometry(Cube());

            m_Pipeline->InputLayout.Format = Vertex3D::Format;
            m_Pipeline->PrimitiveTopology = m_Cube->PrimitiveTopology;

            context::CreateSampler(m_Sampler);
            m_Pipeline->Textures.emplace_back(nullptr);
            m_Pipeline->Samplers.emplace_back(&m_Sampler);

            m_Pipeline->DepthStencil.EnableDepth = true;
            m_Pipeline->DepthStencil.DepthFunc = eDepthStencilFunc::LESS_EQUAL;
            m_Pipeline->Blending.Targets[0].Enable = false;

            context::CreatePipeline(*m_Pipeline);
        }

        SkyboxPass::~SkyboxPass()
        {
            context::FreeSampler(m_Sampler);
        }

        void SkyboxPass::Update(Scene* scene)
        {
        }

        void SkyboxPass::Draw(Scene* scene)
        {
            Skybox* skybox = scene->GetGlobal<Skybox>();
            m_Pipeline->Textures[0] = skybox->CubeTexture.Get();
            context::DrawIndexed(36);
        }

    }

}