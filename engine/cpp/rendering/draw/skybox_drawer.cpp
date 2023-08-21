#include <rendering/draw/skybox_drawer.h>

#include <ecs/scene.h>
#include <ecs/globals.h>

namespace xpe {

    namespace render {

        SkyboxDrawer::SkyboxDrawer(CameraBuffer* cameraBuffer, Shader* shader) : Drawer(cameraBuffer, shader)
        {
            auto cube = Cube();
            m_VertexBuffer.Create(cube.Vertices);
            m_IndexBuffer.Create(cube.Indices);

            m_Sampler.AddressU = TextureSampler::eAddress::CLAMP;
            m_Sampler.AddressV = TextureSampler::eAddress::CLAMP;
            m_Sampler.AddressW = TextureSampler::eAddress::CLAMP;

            m_Pipeline->InputLayout.Format = Vertex3D::Format;
            m_Pipeline->VertexBuffer = m_VertexBuffer.Get();
            m_Pipeline->IndexBuffer = m_IndexBuffer.Get();
            m_Pipeline->DepthStencilState.UseDepthTest = K_TRUE;
            m_Pipeline->BlendState.UseBlending = K_TRUE;
            m_Pipeline->Textures.emplace_back(nullptr);
            m_Pipeline->Samplers.emplace_back(&m_Sampler);

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
            context::DrawIndexed(0, 0, skybox->Cube.Indices.Count(), 1);
        }

    }

}