#include <rendering/passes/skybox_pass.h>

#include <rendering/storages/geometry_storage.h>

#include <ecs/scene.h>
#include <ecs/globals.h>

namespace xpe {

    namespace render {

        SkyboxPass::SkyboxPass(
            const core::vector<RenderPassBinding>& bindings,
            RenderTarget* output,
            GeometryStorage* geometryStorage
        ) : RenderPass(bindings, output)
        {
            auto& cube = geometryStorage->GetGeometryIndexed3D("SkyCube");
            if (cube.Get() == nullptr) {
                cube = geometryStorage->AddGeometryIndexed3D("SkyCube", Cube());
            }
            m_Pipeline->InputLayout.Format = Vertex3D::Format;
            m_Pipeline->PrimitiveTopology = cube->PrimitiveTopology;
            m_Pipeline->VertexBuffer = &cube->Vertices;
            m_Pipeline->IndexBuffer = &cube->Indices;

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
            context::DrawIndexed(0, 0, 36, 1);
        }

    }

}