#include <core/core.hpp>

#include "test_config.h"

using namespace xpe::core;
using namespace xpe::render;
using namespace xpe::control;
using namespace xpe::ttf;
using namespace xpe::io;
using namespace xpe::math;
using namespace xpe::gltf;

class GameApp;

class GameApp : public Application
{
public:
    GameApp() {}
    ~GameApp() {}

    void Init() override final
    {
        LogInfo("GameApp::Init()");

        m_TestConfig = TestConfigReader::Read("test_config.json");

        Input::WindowClosedEvents.AddEvent(this, OnWindowClosed<GameApp>, 1);
        Input::KeyPressedEvents.AddEvent(this, OnKeyPressed<GameApp>, 1);
        Input::KeyHoldEvents.AddEvent(this, OnKeyHold<GameApp>, 1);
        Input::CursorMovedEvents.AddEvent(this, OnCursorMoved<GameApp>, 1);

        m_Canvas = new Canvas(WindowManager::GetWidth(), WindowManager::GetHeight(), context);
        m_ECS = new ECSManager();
        m_BatchManager = new BatchManager(context);

        Font font = TTFManager::Load("resources/fonts/Roboto-Italic.ttf", 32);
        TTFManager::Free(font);

        TextureCubeFile textureCubeFile;
        textureCubeFile.Name = "test";
        textureCubeFile.FrontFilepath = "resources/skybox/front.jpg";
        textureCubeFile.BackFilepath = "resources/skybox/back.jpg";
        textureCubeFile.RightFilepath = "resources/skybox/right.jpg";
        textureCubeFile.LeftFilepath = "resources/skybox/left.jpg";
        textureCubeFile.TopFilepath = "resources/skybox/top.jpg";
        textureCubeFile.BottomFilepath = "resources/skybox/bottom.jpg";

        Texture* textureCube = TextureManager::LoadTextureCubeFile(textureCubeFile, Texture::eFormat::RGBA8);

//        Model3D cubeModel;
//        bool cubeImported = GLTFImporter::Import("resources/cube.gltf", cubeModel);
//        Mesh& cubeMesh = cubeModel[0];

//        Model3D cubeModel = GLTFImporter::Import("resources/cube.gltf");
//        Mesh& cubeMesh = cubeModel[0];
//        m_BatchManager->StoreGeometryIndexed("CubeMesh", cubeMesh);

        PlaneGeometry plane = 100;
        m_BatchManager->StoreGeometryIndexed("PlaneGeometry", plane);

        RenderInstance planeInstance;
        TransformComponent planeTransform("PlaneTransform");
        planeTransform.Position = { 0, -60, 0 };
        TransformManager::UpdateTransform(0, planeTransform);
        m_BatchManager->AddInstance("PlaneGeometry", planeInstance);
        m_BatchManager->FlushInstances("PlaneGeometry");

//        CubeGeometry cube;
//        m_BatchManager->StoreGeometryIndexed("CubeGeometry", cube);
//
        SphereGeometry sphere = { 16, 16 };
        m_BatchManager->StoreGeometryIndexed("SphereGeometry", sphere);
//
//        Triangle3d triangle;
//        m_BatchManager->StoreGeometryVertexed("Triangle", triangle);

        // Put instances of geometry
        u32 transformIndex = 1;
        for (f32 y = -50.0f; y < 50.0f; y += 4.0f)
        {
            for (f32 x = -50.0f; x < 50.0f; x += 4.0f)
            {
                u32 materialIndex = 0;
                for (f32 z = -50.0f; z < 50.0f; z += 4.0f)
                {
                    float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
                    float g = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
                    float b = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);

                    TransformComponent transformComponent("Transform_" + transformIndex);
                    transformComponent.Position = { x, y, z };
                    transformComponent.Rotation = { r * 360.0f, g * 360.0f, b * 360.0f };
//                    transformComponent.Scale    = { r, g, b };

                    TransformManager::UpdateTransform(transformIndex, transformComponent);

                    RenderInstance instance;
                    instance.TransformIndex = transformIndex;
                    instance.MaterialIndex = materialIndex;

                    Material* material = MaterialManager::Builder().Build("Material_" + materialIndex);
                    material->Index = materialIndex;
                    material->Data = MaterialManager::GetMaterialData(materialIndex);
                    material->Data->BaseColor = { r, g, b, 1 };
                    material->Data->MetallicFactor = r;
                    material->Data->RoughnessFactor = g;
                    material->Data->AOFactor = b;

                    m_BatchManager->AddInstance("CubeGeometry", instance);
                    m_BatchManager->AddInstance("CubeMesh", instance);
                    m_BatchManager->AddInstance("SphereGeometry", instance);
                    m_BatchManager->AddInstance("Triangle", instance);

                    transformIndex++;
                    materialIndex++;
                }
            }
        }

        m_BatchManager->FlushInstances("CubeGeometry");
        m_BatchManager->FlushInstances("CubeMesh");
        m_BatchManager->FlushInstances("SphereGeometry");
        m_BatchManager->FlushInstances("Triangle");

        // it will flush all materials data into GPU memory
        MaterialManager::UpdateMaterials();

        // Create render pipeline data

        // setup buffers
        m_Pipeline.VSBuffers.emplace_back(TransformManager::GetBuffer());
        m_Pipeline.VSBuffers.emplace_back(TransformManager::GetBuffer2D());
        m_Pipeline.VSBuffers.emplace_back(&m_CameraBuffer);
        m_Pipeline.PSBuffers.emplace_back(MaterialManager::GetBuffer());
        m_Pipeline.PSBuffers.emplace_back(LightManager::GetDirectBuffer());
        m_Pipeline.PSBuffers.emplace_back(LightManager::GetPointBuffer());
        m_Pipeline.PSBuffers.emplace_back(LightManager::GetSpotBuffer());

        // setup shader
        m_Pipeline.Shader = ShaderManager::Builder()
                .AddVertexStageFromFile("shaders/window.vs")
                .AddPixelStageFromFile("shaders/window.ps")
                .Build();
        m_Pipeline.Shader->PrimitiveTopology = ePrimitiveTopology::TRIANGLE_STRIP;

        // setup input layout
        m_Layout.PrimitiveTopology = ePrimitiveTopology::TRIANGLE_STRIP;
        m_Layout.Format = Vertex3D::Format;
        m_Pipeline.InputLayout = m_Layout;

        // setup render target
        m_Pipeline.RenderTarget = m_Canvas->GetRenderTarget();
        m_Pipeline.DepthStencilState.UseDepthTest = K_TRUE;

        // init pipeline
        context->CreateRenderPipeline(m_Pipeline);

        static PerspectiveCameraComponent perspectiveCamera("PerspectiveCamera");
        perspectiveCamera.Projection.Far = m_TestConfig.CameraFar;
        m_PerspectiveCamera = new PerspectiveCamera(&m_CameraBuffer, &perspectiveCamera, &DeltaTime);
        m_PerspectiveCamera->MoveSpeed = m_TestConfig.CameraMoveSpeed;
        m_PerspectiveCamera->ZoomSpeed = m_TestConfig.CameraZoomSpeed;
        m_PerspectiveCamera->HorizontalSensitivity = m_TestConfig.CameraHorizontalSens;
        m_PerspectiveCamera->VerticalSensitivity = m_TestConfig.CameraVerticalSens;

        // todo maybe we will automate it in future and make it more easy to use
        LightManager::InitLight(m_DirectLightComponent.Light);
        m_DirectLightComponent.Light.Data->Position = {0, 0, 0 };
        m_DirectLightComponent.Light.Data->Color = {1, 1, 1 };
        LightManager::UpdateLight(m_DirectLightComponent.Light);
    }

    void Update() override final
    {
        {
            LockFPSFromConfig();

            m_PerspectiveCamera->Move();

            Simulate();

            m_Canvas->Clear(glm::vec4(1.0f));

            context->BindRenderPipeline(&m_Pipeline);

            // todo bug: canvas is not update or resized because of BindMaterials()
//            MaterialManager::BindMaterials();

            m_BatchManager->DrawAll();

            m_Canvas->Present();
        }
    }

    void Free()
    {
        LogInfo("GameApp::Free()");
        delete m_PerspectiveCamera;
        delete m_ECS;
        delete m_BatchManager;
        delete m_Canvas;
    }

    void WindowClosed()
    {
        LogWarning("GameApp::WindowClosed()");
    }

    void KeyPressed(const eKey key)
    {
        if (key == eKey::Esc)
        {
            WindowManager::Close();
        }

        MoveLight(key);
    }

    void KeyHold(const eKey key)
    {
        MoveLight(key);
    }

    void CursorMoved(const double x, const double y)
    {
    }

private:

    void MoveLight(const eKey key) {
        if (key == eKey::Up) {
            glm::vec3& pos = m_DirectLightComponent.Light.Data->Position;
            pos.y += 1;
            LightManager::UpdateLight(m_DirectLightComponent.Light);
        }

        if (key == eKey::Down) {
            glm::vec3& pos = m_DirectLightComponent.Light.Data->Position;
            pos.y -= 1;
            LightManager::UpdateLight(m_DirectLightComponent.Light);
        }

        if (key == eKey::Left) {
            glm::vec3& pos = m_DirectLightComponent.Light.Data->Position;
            pos.x -= 1;
            LightManager::UpdateLight(m_DirectLightComponent.Light);
        }

        if (key == eKey::Right) {
            glm::vec3& pos = m_DirectLightComponent.Light.Data->Position;
            pos.x += 1;
            LightManager::UpdateLight(m_DirectLightComponent.Light);
        }
    }

    void Simulate() {
        if (m_TestConfig.AnimateLight) {
            auto& pos = m_DirectLightComponent.Light.Data->Position;

            // translation light up and down every N ticks
            static int tick = 1;
            pos.y = 100 * sin(tick++ / 3000.0f);

            // update light color every N ticks
            if (tick % 10000 == 0) {
                auto& color = m_DirectLightComponent.Light.Data->Color;
                float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
                float g = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
                float b = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
                color = { r, g, b };
            }

            LightManager::UpdateLight(m_DirectLightComponent.Light);
        }
    }

private:
    Canvas* m_Canvas;
    ECSManager* m_ECS;

    BatchManager* m_BatchManager;
//    BatchManager2d* m_BatchManager2d;

    Pipeline m_Pipeline;
//    Pipeline m_Pipeline2D;

    InputLayout m_Layout;
//    InputLayout m_Layout2D;

    PerspectiveCamera* m_PerspectiveCamera;
//    OrthoCamera* m_OrthoCamera;

    DirectLightComponent m_DirectLightComponent = string("DirectLight");

    TestConfig m_TestConfig;

};

Application* CreateApplication() {
    return new GameApp();
}
