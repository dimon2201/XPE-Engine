#include <rendering/draw/skybox_drawer.h>

#include <rendering/storages/geometry_storage.h>

#include <ecs/scene.h>
#include <ecs/globals.h>

namespace xpe {

    namespace render {

        SkyboxDrawer::SkyboxDrawer(CameraBuffer* cameraBuffer, Shader* shader, GeometryStorage* geometryStorage)
        : Drawer(cameraBuffer, shader)
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

            Init();
        }

        SkyboxDrawer::~SkyboxDrawer()
        {
            context::FreeSampler(m_Sampler);
        }

        void SkyboxDrawer::Draw(Scene* scene)
        {
            Skybox* skybox = scene->GetGlobal<Skybox>();
            m_Pipeline->Textures[0] = skybox->CubeTexture.Get();
            context::DrawIndexed(0, 0, 36, 1);
        }

    }

}