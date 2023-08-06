#include <rendering/skybox/skybox.hpp>
#include <rendering/camera/camera_manager.h>
#include <rendering/draw/batching.h>
#include <rendering/draw/canvas.hpp>
#include <geometry/cube_geometry.h>

namespace xpe {

    namespace render {

        Skybox::Skybox(BatchManager* batchManager, Canvas* canvas, Texture* cubemap)
        {
            m_BatchManager = batchManager;
            m_InputLayout = new InputLayout();
            m_Pipeline = new Pipeline();
            m_Pipeline->VSBuffers.emplace_back(CameraManager::GetBuffer());

            m_Pipeline->Shader = ShaderManager::CreateShader("skybox");
            ShaderManager::AddVertexStageFromFile(m_Pipeline->Shader, "shaders/skybox.vs");
            ShaderManager::AddPixelStageFromFile(m_Pipeline->Shader, "shaders/skybox.ps");
            ShaderManager::BuildShader(m_Pipeline->Shader);

            m_InputLayout->Format = Vertex3D::Format;
            m_Pipeline->InputLayout = *m_InputLayout;

            m_Pipeline->RenderTarget = canvas->GetRenderTarget();
            m_Pipeline->DepthStencilState.UseDepthTest = core::K_TRUE;
            m_Pipeline->BlendState.UseBlending = core::K_TRUE;

            m_Pipeline->Textures.push_back(nullptr);

            // Init pipeline
            context::CreatePipeline(*m_Pipeline);

            m_Cubemap = cubemap;

            // Store cube geometry for skybox rendering
            math::CubeGeometry cube;
            cube.PrimitiveTopology = ePrimitiveTopology::TRIANGLE_LIST;
            m_BatchManager->StoreGeometryIndexed("__Skybox", cube, 1);
        }

        Skybox::~Skybox()
        {
            delete m_InputLayout;
            delete m_Pipeline;
        }

        void Skybox::Draw()
        {
            m_BatchManager->DrawAll();
        }

    }

}