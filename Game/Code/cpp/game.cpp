#include <core/core.hpp>

#include <test_config.h>

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

        ShaderManager::WatchShaders("shaders", true);

        m_TestConfig = TestConfigReader::Read("test_config.json");

        Input::WindowClosedEvents->AddEvent(this, OnWindowClosed<GameApp>, 1);
        Input::KeyPressedEvents->AddEvent(this, OnKeyPressed<GameApp>, 1);
        Input::KeyHoldEvents->AddEvent(this, OnKeyHold<GameApp>, 1);
        Input::CursorMovedEvents->AddEvent(this, OnCursorMoved<GameApp>, 1);

        m_Canvas = new Canvas(WindowManager::GetWidth(), WindowManager::GetHeight(), context);
        m_ECS = new ECSManager();
        m_BatchManager = new BatchManager(context);
        m_BatchManager2d = new BatchManager2d(context);

        Font font = TTFManager::Get().Load("resources/fonts/Roboto-Italic.ttf", 32);
        TTFManager::Get().Free(font);

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
        TransformComponent planeTransform("PlaneTransform");
        planeTransform.Position = { 0, -60, 0 };
        TransformManager::FlushTransform(0, planeTransform);
        m_BatchManager->AddInstance("PlaneGeometry", planeInstance);
        m_BatchManager->FlushInstances("PlaneGeometry");

        CubeGeometry cube;
        m_BatchManager->StoreGeometryIndexed("CubeGeometry", cube);
//        m_BatchManager->ReserveInstances("CubeGeometry", 1000);

        SphereGeometry sphere;
        m_BatchManager->StoreGeometryIndexed("SphereGeometry", sphere);
//        m_BatchManager->ReserveInstances("SphereGeometry", 1000);

        Quad2d quad2D;
        RenderInstance2d quad2DInstance;
        Transform2DComponent quad2DTransform("Quad2DTransform");
        quad2DTransform.Position = { 0, -10 };
        quad2DTransform.Scale = { 5, 1 };
        TransformManager::UpdateTransform2D(0, quad2DTransform);
        m_BatchManager2d->StoreGeometryIndexed("Quad2D", quad2D);
        m_BatchManager2d->AddInstance("Quad2D", quad2DInstance);
        m_BatchManager2d->FlushInstances("Quad2D");

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

                    TransformComponent transformComponent("Transform_" + transformIndex);
                    transformComponent.Position = { x, y, z };
                    transformComponent.Rotation = { r * 360.0f, g * 360.0f, b * 360.0f };
//                    transformComponent.Scale    = { r, g, b };

                    TransformManager::FlushTransform(transformIndex, transformComponent);

                    RenderInstance instance;
                    instance.TransformIndex = transformIndex;
                    instance.MaterialIndex = materialIndex;

                    Material* material = MaterialManager::CreateMaterial("Material_" + materialIndex);
                    material->Index = materialIndex;
                    material->Data = MaterialManager::GetMaterialData(materialIndex);
                    material->Data->BaseColor = { 1, 1, 1, 1 };
                    material->Data->MetallicFactor = r;
                    material->Data->RoughnessFactor = g;
                    material->Data->AOFactor = b;

                    material->Data->EnableAlbedo = true;
                    material->AlbedoIndex = textureIndex;
                    MaterialManager::AddAlbedoFromFile(*material, "resources/materials/steel/albedo.png");

                    material->Data->EnableBumping = false;
                    material->BumpingIndex = textureIndex;
                    MaterialManager::AddBumpFromFile(*material, "resources/materials/steel/bump.png");

                    material->Data->EnableMetallic = false;
                    material->MetallicIndex = textureIndex;
                    MaterialManager::AddMetallicFromFile(*material, "resources/materials/steel/metallic.png");

                    material->Data->EnableRoughness = false;
                    material->RoughnessIndex = textureIndex;
                    MaterialManager::AddRoughnessFromFile(*material, "resources/materials/steel/roughness.png");

                    material->Data->EnableAO = false;
                    material->AOIndex = textureIndex;
                    MaterialManager::AddAOFromFile(*material, "resources/materials/steel/ao.png");

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

        m_BatchManager->FlushInstances("CubeGeometry");
        m_BatchManager->FlushInstances("CubeMesh");
        m_BatchManager->FlushInstances("SphereGeometry");
        m_BatchManager->FlushInstances("Triangle");

        // it will flush all materials data into GPU memory
        MaterialManager::FlushMaterials();

        InitPipeline();
        InitPipeline2D();
        InitCamera();
        InitCamera2D();

        LightManager::InitLight(m_DirectLightComponent.Light);
        m_DirectLightComponent.Light.Data->Position = {0, 0, 0 };
        m_DirectLightComponent.Light.Data->Color = {1, 1, 1 };
        LightManager::FlushLight(m_DirectLightComponent.Light);
    }

    void Update() override final
    {
        LockFPSFromConfig();

        UpdateCamera();

        Simulate();

        MaterialManager::BindMaterials();

        m_Canvas->Clear(glm::vec4(1.0f));

        context->BindPipeline(&m_Pipeline);
        m_BatchManager->DrawAll();

        context->BindPipeline(&m_Pipeline2d);
        m_BatchManager2d->DrawAll();

        m_Canvas->Present();
    }

    void Free()
    {
        LogInfo("GameApp::Free()");

        delete m_ECS;
        delete m_Canvas;

        delete m_Camera;
        delete m_BatchManager;

        delete m_Camera2D;
        delete m_BatchManager2d;
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
        m_Pipeline.VSBuffers.emplace_back(&m_CameraBuffer);
        m_Pipeline.VSBuffers.emplace_back(TransformManager::GetBuffer());
        m_Pipeline.PSBuffers.emplace_back(MaterialManager::GetBuffer());
        m_Pipeline.PSBuffers.emplace_back(LightManager::GetDirectBuffer());
        m_Pipeline.PSBuffers.emplace_back(LightManager::GetPointBuffer());
        m_Pipeline.PSBuffers.emplace_back(LightManager::GetSpotBuffer());

        // setup shader
        m_Pipeline.Shader = ShaderManager::CreateShader("window");
        ShaderManager::AddVertexStageFromFile(m_Pipeline.Shader, "shaders/window.vs");
        ShaderManager::AddPixelStageFromFile(m_Pipeline.Shader, "shaders/window.ps");
        ShaderManager::BuildShader(m_Pipeline.Shader);

        // setup input layout
        m_Layout.Format = Vertex3D::Format;
        m_Pipeline.InputLayout = m_Layout;

        // setup render target
        m_Pipeline.RenderTarget = m_Canvas->GetRenderTarget();
        m_Pipeline.DepthStencilState.UseDepthTest = K_TRUE;

        // init pipeline
        context->CreatePipeline(m_Pipeline);
    }

    void InitPipeline2D() {
        // setup buffers
        m_Pipeline2d.VSBuffers.emplace_back(&m_CameraBuffer2d);
        m_Pipeline2d.VSBuffers.emplace_back(TransformManager::GetBuffer2D());

        // setup shader
        m_Pipeline2d.Shader = ShaderManager::CreateShader("window2d");
        ShaderManager::AddVertexStageFromFile(m_Pipeline2d.Shader, "shaders/window2d.vs");
        ShaderManager::AddPixelStageFromFile(m_Pipeline2d.Shader, "shaders/window2d.ps");
        ShaderManager::BuildShader(m_Pipeline2d.Shader);

        // setup input layout
        m_Layout2d.Format = Vertex2D::Format;
        m_Pipeline2d.InputLayout = m_Layout2d;

        // setup render target
        m_Pipeline2d.RenderTarget = m_Canvas->GetRenderTarget();
        m_Pipeline2d.DepthStencilState.UseDepthTest = K_FALSE;

        // init pipeline
        context->CreatePipeline(m_Pipeline2d);
    }

    void InitCamera() {
        m_PerspectiveCameraComponent.Projection.Far = m_TestConfig.CameraFar;
        // todo BUG: after moving camera, the camera resets position
        m_PerspectiveCameraComponent.Position = { 5, 5, 20 };
        m_Camera = new PerspectiveCamera(&m_CameraBuffer, &m_PerspectiveCameraComponent);
        m_Camera->MoveSpeed = m_TestConfig.CameraMoveSpeed;
        m_Camera->ZoomAcceleration = m_TestConfig.CameraZoomAcceleration;
        m_Camera->PanAcceleration = m_TestConfig.CameraPanAcceleration;
        m_Camera->HorizontalSensitivity = m_TestConfig.CameraHorizontalSens;
        m_Camera->VerticalSensitivity = m_TestConfig.CameraVerticalSens;
    }

    void InitCamera2D() {
        m_OrthoCameraComponent.Position = { 0, 0, -1 };
        m_Camera2D = new OrthoCamera(&m_CameraBuffer2d, &m_OrthoCameraComponent);
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
        if (key == eKey::Up) {
            glm::vec3& pos = m_DirectLightComponent.Light.Data->Position;
            pos.y += 1;
            LightManager::FlushLight(m_DirectLightComponent.Light);
        }

        if (key == eKey::Down) {
            glm::vec3& pos = m_DirectLightComponent.Light.Data->Position;
            pos.y -= 1;
            LightManager::FlushLight(m_DirectLightComponent.Light);
        }

        if (key == eKey::Left) {
            glm::vec3& pos = m_DirectLightComponent.Light.Data->Position;
            pos.x -= 1;
            LightManager::FlushLight(m_DirectLightComponent.Light);
        }

        if (key == eKey::Right) {
            glm::vec3& pos = m_DirectLightComponent.Light.Data->Position;
            pos.x += 1;
            LightManager::FlushLight(m_DirectLightComponent.Light);
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

            LightManager::FlushLight(m_DirectLightComponent.Light);
        }
    }

private:
    Canvas* m_Canvas;
    ECSManager* m_ECS;

    BatchManager* m_BatchManager;
    BatchManager2d* m_BatchManager2d;

    Pipeline m_Pipeline;
    Pipeline m_Pipeline2d;

    InputLayout m_Layout;
    InputLayout m_Layout2d;

    PerspectiveCamera* m_Camera;
    OrthoCamera* m_Camera2D;

    DirectLightComponent m_DirectLightComponent = string("DirectLight");

    PerspectiveCameraComponent m_PerspectiveCameraComponent = string("PerspectiveCamera");
    OrthoCameraComponent m_OrthoCameraComponent = string("OrthoCamera");

    TestConfig m_TestConfig;

};

Application* CreateApplication() {
    return new GameApp();
}
