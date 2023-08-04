#include <core/core.hpp>

#include "test_config.h"
#include "launcher.h"

using namespace xpe::core;
using namespace xpe::ecs;
using namespace xpe::render;
using namespace xpe::control;
using namespace xpe::text;
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

        CameraManager::Init(Application::context);

        m_TestConfig = TestConfigReader::Read("test_config.json");

        Input::WindowClosedEvents->AddEvent(this, OnWindowClosed<GameApp>, 1);
        Input::KeyPressedEvents->AddEvent(this, OnKeyPressed<GameApp>, 1);
        Input::KeyHoldEvents->AddEvent(this, OnKeyHold<GameApp>, 1);
        Input::CursorMovedEvents->AddEvent(this, OnCursorMoved<GameApp>, 1);

        m_Canvas = new Canvas(WindowManager::GetWidth(), WindowManager::GetHeight(), context);
        m_ECS = new ECSManager();
        m_BatchManager = new BatchManager(context);
        m_TextBatchManager = new TextBatchManager(context);

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

        PlaneGeometry plane;
        m_BatchManager->StoreGeometryIndexed("PlaneGeometry", plane);

        RenderInstance planeInstance;
        TransformComponent planeTransform("PlaneTransform", 0);
        planeTransform.Position = { 0, -60, 0 };
        TransformManager::AddTransform(planeTransform);
        m_BatchManager->AddInstance("PlaneGeometry", planeInstance);

//        CubeGeometry cube;
//        m_BatchManager->StoreGeometryIndexed("CubeGeometry", cube);
//        m_BatchManager->ResizeInstances("CubeGeometry", 1000);

        SphereGeometry sphere;
        m_BatchManager->StoreGeometryIndexed("SphereGeometry", sphere);
//        m_BatchManager->ReserveInstances("SphereGeometry", 1000);

        // Put instances of geometry
        u32 transformIndex = 1;
        u32 materialIndex = 0;
        u32 textureIndex = 0;
        for (f32 y = -4.0f; y < 4.0f; y += 4.0f)
        {
            for (f32 x = -4.0f; x < 4.0f; x += 4.0f)
            {
                for (f32 z = -4.0f; z < 4.0f; z += 4.0f)
                {
                    float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
                    float g = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
                    float b = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);

                    TransformComponent transformComponent("Transform_" + transformIndex, transformIndex);
                    transformComponent.Position = { x, y, z };
                    transformComponent.Rotation = { r * 360.0f, g * 360.0f, b * 360.0f };
//                    transformComponent.Scale    = { r, g, b };

                    TransformManager::AddTransform(transformComponent);

                    RenderInstance instance;
                    instance.TransformIndex = transformIndex;
                    instance.MaterialIndex = materialIndex;

                    Material* material = MaterialManager::CreateMaterial("Material_" + materialIndex);
                    material->Data->BaseColor = { 1, 1, 1, 1 };
                    material->Data->MetallicFactor = r;
                    material->Data->RoughnessFactor = g;
                    material->Data->AOFactor = b;

//                    material->Data->EnableAlbedo = false;
//                    material->AlbedoIndex = textureIndex;
//                    MaterialManager::AddAlbedoFromFile(*material, "resources/materials/steel/albedo.png");
//
//                    material->Data->EnableBumping = false;
//                    material->BumpingIndex = textureIndex;
//                    MaterialManager::AddBumpFromFile(*material, "resources/materials/steel/bump.png");
//
//                    material->Data->EnableMetallic = false;
//                    material->MetallicIndex = textureIndex;
//                    MaterialManager::AddMetallicFromFile(*material, "resources/materials/steel/metallic.png");
//
//                    material->Data->EnableRoughness = false;
//                    material->RoughnessIndex = textureIndex;
//                    MaterialManager::AddRoughnessFromFile(*material, "resources/materials/steel/roughness.png");
//
//                    material->Data->EnableAO = false;
//                    material->AOIndex = textureIndex;
//                    MaterialManager::AddAOFromFile(*material, "resources/materials/steel/ao.png");

                    m_BatchManager->AddInstance("CubeGeometry", instance);
                    m_BatchManager->AddInstance("CubeMesh", instance);
                    m_BatchManager->AddInstance("SphereGeometry", instance);
                    m_BatchManager->AddInstance("Triangle", instance);

                    transformIndex++;
                    materialIndex++;
                    if (++textureIndex > 1) {
                        textureIndex = 0;
                    }
                }
            }
        }

        m_DirectLightComponent.Position = { 0, 0, 0 };
        m_DirectLightComponent.Color = { 1, 1, 1 };
        LightManager::AddDirectLight(m_DirectLightComponent);

        // it will flush all instance data into GPU memory
        m_BatchManager->FlushInstances("CubeGeometry");
        m_BatchManager->FlushInstances("CubeMesh");
        m_BatchManager->FlushInstances("SphereGeometry");
        m_BatchManager->FlushInstances("Triangle");
        m_BatchManager->FlushInstances("PlaneGeometry");

        InitPipeline();
        InitCamera();
        InitCamera2D();

        TextRenderer::Init(context, m_TextBatchManager, m_Canvas);
    }

    void Update() override final
    {
        {
            LockFPSFromConfig();

            UpdateCamera();

            Simulate();

            m_Canvas->Clear(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));

            TransformComponent transform("TextTransform");
            transform.Position = { 0.0f, 0.0f, 0.0f };
            transform.Scale = { 0.01f, 0.01f, 0.01f };
            m_Font.GlyphNewLineExtraOffset = 32.0f;

            TextComponent text("TextComponent");
            text.Text = "Hello!\nNew line";

            TextRenderer::Get().Draw(&m_Font, &transform, &text);

            m_Canvas->Present();
        }
    }

    void Free()
    {
        LogInfo("GameApp::Free()");

        delete m_ECS;
        delete m_Canvas;

        delete m_Camera;
        delete m_BatchManager;
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

        if (key == eKey::R)
        {
            ShaderManager::ReloadStage("shaders/window.vs");
            ShaderManager::ReloadStage("shaders/window.ps");
        }

        MoveLight(key);
    }

    void KeyHold(const eKey key)
    {
        MoveLight(key);
    }

    void CursorMoved(const double x, const double y)
    {
        if (Input::MousePressed(eMouse::ButtonRight)) {
            m_Camera->EnableLook = false;
            Input::CaptureCursor(x, y);
            auto& cursorDelta = Input::GetMouseCursor().Delta;
            m_Camera->Pan(cursorDelta);
        } else {
            m_Camera->EnableLook = true;
        }
    }

private:

    void InitPipeline() {
        // setup buffers
        m_Pipeline.VSBuffers.emplace_back(CameraManager::GetBuffer());
        m_Pipeline.VSBuffers.emplace_back(TransformManager::GetBuffer());
        m_Pipeline.VSBuffers.emplace_back(MaterialManager::GetBuffer());
        m_Pipeline.PSBuffers.emplace_back(LightManager::GetDirectBuffer());
        m_Pipeline.PSBuffers.emplace_back(LightManager::GetPointBuffer());
        m_Pipeline.PSBuffers.emplace_back(LightManager::GetSpotBuffer());
        
        // setup shader
        m_Pipeline.Shader = xpe::render::ShaderManager::CreateShader("window");
        xpe::render::ShaderManager::AddVertexStageFromFile(m_Pipeline.Shader, "shaders/window.vs");
        xpe::render::ShaderManager::AddPixelStageFromFile(m_Pipeline.Shader, "shaders/window.ps");
        xpe::render::ShaderManager::BuildShader(m_Pipeline.Shader);

        // setup input layout
        m_Layout.Format = Vertex3D::Format;
        m_Pipeline.InputLayout = m_Layout;

        // setup render target
        m_Pipeline.RenderTarget = m_Canvas->GetRenderTarget();
        m_Pipeline.DepthStencilState.UseDepthTest = K_TRUE;
        m_Pipeline.BlendState.UseBlending = xpe::core::K_TRUE;

        // init pipeline
        context->CreatePipeline(m_Pipeline);
    }

    void InitCamera() {
        m_PerspectiveCameraComponent.Projection.Far = m_TestConfig.CameraFar;
        m_Camera = new PerspectiveCamera(CameraManager::GetBuffer(), &m_PerspectiveCameraComponent);
        m_Camera->MoveSpeed = m_TestConfig.CameraMoveSpeed;
        m_Camera->ZoomAcceleration = m_TestConfig.CameraZoomAcceleration;
        m_Camera->PanAcceleration = m_TestConfig.CameraPanAcceleration;
        m_Camera->HorizontalSensitivity = m_TestConfig.CameraHorizontalSens;
        m_Camera->VerticalSensitivity = m_TestConfig.CameraVerticalSens;
    }

    void InitCamera2D() {
    }

    void UpdateCamera() {
        if (Input::MousePressed(eMouse::ButtonLeft)) {
            m_Camera->LookMode = Camera::eLookMode::EDITOR;
        } else {
            m_Camera->LookMode = Camera::eLookMode::GAME;
        }
        m_Camera->Move();
    }

    void MoveLight(const eKey key) {
        
    }

    void Simulate() {
        
    }

private:
    Canvas* m_Canvas;
    ECSManager* m_ECS;

    BatchManager* m_BatchManager;
    TextBatchManager* m_TextBatchManager;
    Font m_Font;

    Pipeline m_Pipeline;

    InputLayout m_Layout;

    PerspectiveCamera* m_Camera;

    DirectLightComponent m_DirectLightComponent = string("DirectLight");

    PerspectiveCameraComponent m_PerspectiveCameraComponent = string("PerspectiveCamera");

    TestConfig m_TestConfig;

};

Application* CreateApplication() {
    return new GameApp();
}
