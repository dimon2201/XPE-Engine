#include <rendering/skybox/skybox.hpp>
#include <rendering/core/pipeline.h>
#include <rendering/camera/camera_manager.h>
#include <rendering/dx11/d3d11_context.hpp>
#include <rendering/draw/batching.h>
#include <rendering/draw/canvas.hpp>
#include <rendering/core/texture.h>
#include <geometry/cube_geometry.h>

xpe::skybox::Skybox::Skybox(render::Context* context, render::BatchManager* batchManager, render::Canvas* canvas, render::Texture* cubemap)
{
    m_Context = context;
    m_BatchManager = batchManager;
    m_InputLayout = new render::InputLayout();
    m_Pipeline = new render::Pipeline();
    m_Pipeline->VSBuffers.emplace_back(render::CameraManager::GetBuffer());

    m_Pipeline->Shader = render::ShaderManager::CreateShader("skybox");
    render::ShaderManager::AddVertexStageFromFile(m_Pipeline->Shader, "shaders/skybox.vs");
    render::ShaderManager::AddPixelStageFromFile(m_Pipeline->Shader, "shaders/skybox.ps");
    render::ShaderManager::BuildShader(m_Pipeline->Shader);

    m_InputLayout->Format = render::Vertex3D::Format;
    m_Pipeline->InputLayout = *m_InputLayout;

    m_Pipeline->RenderTarget = canvas->GetRenderTarget();
    m_Pipeline->DepthStencilState.UseDepthTest = core::K_TRUE;
    m_Pipeline->BlendState.UseBlending = core::K_TRUE;

    m_Pipeline->Textures.push_back(nullptr);

    // Init pipeline
    m_Context->CreatePipeline(*m_Pipeline);

    m_Cubemap = cubemap;

    // Store cube geometry for skybox rendering
    math::CubeGeometry cube;
    cube.PrimitiveTopology = render::ePrimitiveTopology::TRIANGLE_LIST;
    m_BatchManager->StoreGeometryIndexed("__Skybox", cube);
}

xpe::skybox::Skybox::~Skybox()
{
    delete m_InputLayout;
    delete m_Pipeline;
}

void xpe::skybox::Skybox::Draw()
{
    m_BatchManager->ClearInstances("__Skybox");

    render::RenderInstance instance;
    instance.CameraIndex = 0;
    m_BatchManager->AddInstance("__Skybox", instance);
    
    m_BatchManager->FlushInstances("__Skybox");

    m_BatchManager->DrawAll();
}