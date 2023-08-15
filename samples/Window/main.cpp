#include <core/app.hpp>
#include <launcher.h>

#include <rendering/renderer.h>

#include <rendering/draw/instance_drawer.h>

#include <ecs/entities.hpp>
#include <ecs/scenes.hpp>

#include <model_loader.h>
#include <material_loader.h>
#include <font_loader.h>
#include <texture_loader.h>

#include "test_config.h"

using namespace xpe::core;
using namespace xpe::ecs;
using namespace xpe::render;
using namespace xpe::control;
using namespace xpe::math;
using namespace xpe::res;

class GameApp : public Application
{
public:
    GameApp() {}
    ~GameApp() {}

protected:

    void InitRenderer() override
    {
        Application::InitRenderer();

        Shader* shader = ShaderManager::CreateShader("window");
        ShaderManager::AddVertexStageFromFile(shader, "shaders/window.vs");
        ShaderManager::AddPixelStageFromFile(shader, "shaders/window.ps");
        ShaderManager::BuildShader(shader);
        m_Renderer->AddDrawer<InstanceDrawer>(
                m_Renderer->GetCameraBuffer(),
                shader,
                m_GeometryStorage,
                m_MaterialStorage
        );
    }

public:

    void Init() override final {
        LogInfo("GameApp::Init()");

        // read test configs from file
        if (!xpe::res::ReadJsonFile("config/test_config.json", m_TestConfig))
        {
            LogError("test_config.json file not found in config/test_config.json path.");
        }

        AddWindowClosed(GameApp, 1);
        AddKeyPressed(GameApp, 1);
        AddKeyHold(GameApp, 1);
        AddCursorMove(GameApp, 1);

        m_ModelLoader.Create(m_GeometryStorage);
        m_MaterialLoader.Create(m_MaterialStorage);
        m_TextureLoader.Create(m_TextureStorage);
        m_FontLoader.Create(m_FontStorage);

        InitCamera();
        InitCamera2D();

        // setup text 3D entity
        {
            m_Text3D = {"Text3D", m_MainScene};

            Text3DComponent text3D("Text3D");
            text3D.FontResFilepath = "res/fonts/Roboto-Bold.ttf";
            text3D.Font = m_FontLoader->Load(text3D.FontResFilepath.c_str(), 22);
            text3D.Text = "Hi,\nWelcome to Example Window\nThis is a testing version of application";
            text3D.Font->NewLineOffset = 1.0f;
            text3D.Transform.Position = { 0, 0, 0 };
            text3D.Transform.Scale = { 1, 1, 1 };

            m_Text3D.AddComponent<Text3DComponent>(text3D);
        }

        // setup skybox global
        {
            m_MainScene->Skybox->FrontResFilepath = "res/skybox/front.jpg";
            m_MainScene->Skybox->BackResFilepath = "res/skybox/back.jpg";
            m_MainScene->Skybox->RightResFilepath = "res/skybox/right.jpg";
            m_MainScene->Skybox->LeftResFilepath = "res/skybox/left.jpg";
            m_MainScene->Skybox->TopResFilepath = "res/skybox/top.jpg";
            m_MainScene->Skybox->BottomResFilepath = "res/skybox/bottom.jpg";

            TextureCubeFilepath skyboxPath;
            skyboxPath.Name = m_MainScene->Skybox->GetTag();
            skyboxPath.FrontFilepath = m_MainScene->Skybox->FrontResFilepath;
            skyboxPath.BackFilepath = m_MainScene->Skybox->BackResFilepath;
            skyboxPath.LeftFilepath = m_MainScene->Skybox->LeftResFilepath;
            skyboxPath.RightFilepath = m_MainScene->Skybox->RightResFilepath;
            skyboxPath.TopFilepath = m_MainScene->Skybox->TopResFilepath;
            skyboxPath.BottomFilepath = m_MainScene->Skybox->BottomResFilepath;

            m_MainScene->Skybox->CubeTexture = m_TextureLoader->LoadCube(skyboxPath, eTextureFormat::RGBA8);
//            m_MainScene->Skybox->CubeTexture->GenerateMips();
        }

        // setup plane
        {
            m_Plane = { "Plane", m_MainScene };

            GeometryIndexed3DComponent plane("G_Plane");
            plane.Geometry = m_GeometryStorage->AddGeometryIndexed3D("G_Plane", Cube());
            plane.Instance.Transform.Position = { 0, -10, 0 };
            plane.Instance.Transform.Scale = { 100, 0.01, 100 };
            plane.Instance.Material = m_MaterialStorage->Add("MT_Plane", Material());
            plane.Instance.Material->BaseColor = { 0, 1, 0, 1 };
            m_MaterialStorage->Set("MT_Plane", *plane.Instance.Material);

            m_Plane.AddComponent<GeometryIndexed3DComponent>(plane);
        }

        // setup direct light
        {
            m_DirectLight = { "DirectLight", m_MainScene };

            DirectLightComponent directLight("L_Direct");
            directLight.Position = { 0, 0, 0 };
            directLight.Color = { 1, 1, 1 };

            m_DirectLight.AddComponent<DirectLightComponent>(directLight);
        }

        // setup 3D model
        {
            m_WinterGirl = { "WinterGirl", m_MainScene };

            ModelListComponent winterGirlModel("M_WinterGirl");
            winterGirlModel.Model = m_ModelLoader->Load("res/models/winter-girl/source/winter_girl.fbx");

            {
                Transform transform;
                transform.Position = { -2, -10, -2 };
                transform.Rotation = { -90, 0, 0 };
                transform.Scale = { 0.25, 0.25, 0.25 };
                winterGirlModel.Transforms.emplace_back(transform);
            }

            {
                Transform transform;
                transform.Position = { -2, -10, 2 };
                transform.Rotation = { -90, 0, 0 };
                transform.Scale = { 0.25, 0.25, 0.25 };
                winterGirlModel.Transforms.emplace_back(transform);
            }

            {
                Transform transform;
                transform.Position = { 2, -10, -2 };
                transform.Rotation = { -90, 0, 0 };
                transform.Scale = { 0.25, 0.25, 0.25 };
                winterGirlModel.Transforms.emplace_back(transform);
            }

            {
                Transform transform;
                transform.Position = { 2, -10, 2 };
                transform.Rotation = { -90, 0, 0 };
                transform.Scale = { 0.25, 0.25, 0.25 };
                winterGirlModel.Transforms.emplace_back(transform);
            }

            {
                MaterialFilepath materialFilepath;
                materialFilepath.Name = "niz";
                materialFilepath.AlbedoFilepath = "res/models/winter-girl/textures/LOW_niz_BaseColor.png";
                materialFilepath.BumpFilepath = "res/models/winter-girl/textures/LOW_niz_Normal.png";
                materialFilepath.MetallicFilepath = "res/models/winter-girl/textures/LOW_niz_Metallic.png";
                materialFilepath.RoughnessFilepath = "res/models/winter-girl/textures/LOW_niz_Roughness.png";
                winterGirlModel.Model->Meshes[0].Material = m_MaterialLoader->Load(materialFilepath);
            }

            {
                MaterialFilepath materialFilepath;
                materialFilepath.Name = "04";
                materialFilepath.AlbedoFilepath = "res/models/winter-girl/textures/LOW_04_BaseColor.png";
                materialFilepath.BumpFilepath = "res/models/winter-girl/textures/LOW_04_Normal.png";
                materialFilepath.MetallicFilepath = "res/models/winter-girl/textures/LOW_04_Metallic.png";
                materialFilepath.RoughnessFilepath = "res/models/winter-girl/textures/LOW_04_Roughness.png";
                winterGirlModel.Model->Meshes[1].Material = m_MaterialLoader->Load(materialFilepath);
            }

            {
                MaterialFilepath materialFilepath;
                materialFilepath.Name = "R1";
                materialFilepath.AlbedoFilepath = "res/models/winter-girl/textures/LOW_R1_BaseColor.png";
                materialFilepath.BumpFilepath = "res/models/winter-girl/textures/LOW_R1_Normal.png";
                materialFilepath.MetallicFilepath = "res/models/winter-girl/textures/LOW_R1_Metallic.png";
                materialFilepath.RoughnessFilepath = "res/models/winter-girl/textures/LOW_R1_Roughness.png";
                winterGirlModel.Model->Meshes[2].Material = m_MaterialLoader->Load(materialFilepath);
            }

            {
                MaterialFilepath materialFilepath;
                materialFilepath.Name = "verx";
                materialFilepath.AlbedoFilepath = "res/models/winter-girl/textures/LOW_verx_BaseColor.png";
                materialFilepath.BumpFilepath = "res/models/winter-girl/textures/LOW_verx_Normal.png";
                materialFilepath.MetallicFilepath = "res/models/winter-girl/textures/LOW_verx_Metallic.png";
                materialFilepath.RoughnessFilepath = "res/models/winter-girl/textures/LOW_verx_Roughness.png";
                winterGirlModel.Model->Meshes[3].Material = m_MaterialLoader->Load(materialFilepath);
            }

            {
                MaterialFilepath materialFilepath;
                materialFilepath.Name = "pngegg";
                materialFilepath.AlbedoFilepath = "res/models/winter-girl/textures/pngegg.png";
                winterGirlModel.Model->Meshes[4].Material = m_MaterialLoader->Load(materialFilepath);
            }

            m_WinterGirl.AddComponent<ModelListComponent>(winterGirlModel);
        }
    }

    void Update() override final
    {
        LockFPSFromConfig();
        UpdateCamera();
        Simulate();
    }

    void Free()
    {
        LogInfo("GameApp::Free()");
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
        auto& camera = *m_MainScene->PerspectiveCamera;
        if (Input::MousePressed(eMouse::ButtonRight)) {
            camera.EnableLook = false;
            Input::CaptureCursor(x, y);
            auto& cursorDelta = Input::GetMouseCursor().Delta;
            camera.Pan(cursorDelta);
        } else {
            camera.EnableLook = true;
        }
    }

private:

    void InitCamera() {
        m_MainScene->PerspectiveCamera->Component = string("PerspectiveCamera");
        m_MainScene->PerspectiveCamera->Component.Projection.Far = m_TestConfig.CameraFar;
        // todo(cheerwizard): BUG - after moving camera, the camera resets position
        m_MainScene->PerspectiveCamera->Component.Position = { 5, 5, 20 };
        m_MainScene->PerspectiveCamera->MoveSpeed = m_TestConfig.CameraMoveSpeed;
        m_MainScene->PerspectiveCamera->ZoomAcceleration = m_TestConfig.CameraZoomAcceleration;
        m_MainScene->PerspectiveCamera->PanAcceleration = m_TestConfig.CameraPanAcceleration;
        m_MainScene->PerspectiveCamera->HorizontalSensitivity = m_TestConfig.CameraHorizontalSens;
        m_MainScene->PerspectiveCamera->VerticalSensitivity = m_TestConfig.CameraVerticalSens;

        m_MainScene->PerspectiveCamera->Init(WindowManager::GetWidth(), WindowManager::GetHeight());
    }

    void InitCamera2D()
    {
        m_MainScene->OrthoCamera->Component = string("OrthoCamera");
        m_MainScene->OrthoCamera->Component.Position = { 0, 0, -1 };
    }

    void UpdateCamera()
    {
        auto& camera = *m_MainScene->PerspectiveCamera;
        if (Input::MousePressed(eMouse::ButtonLeft)) {
            camera.LookMode = Camera::eLookMode::EDITOR;
        } else {
            camera.LookMode = Camera::eLookMode::GAME;
        }
        camera.Move();
    }

    void MoveLight(const eKey key)
    {
        auto* directLight = m_DirectLight.GetComponent<DirectLightComponent>("L_Direct");
        auto& pos = directLight->Position;

        if (key == eKey::Up)
        {
            pos.y += 1;
        }

        if (key == eKey::Down)
        {
            pos.y -= 1;
        }

        if (key == eKey::Left)
        {
            pos.x -= 1;
        }

        if (key == eKey::Right)
        {
            pos.x += 1;
        }
    }

    void Simulate()
    {
        if (m_TestConfig.AnimateLight)
        {
            auto* directLight = m_DirectLight.GetComponent<DirectLightComponent>("L_Direct");
            auto& pos = directLight->Position;

            // translation light up and down every N ticks
            static int tick = 1;

            pos.x = 100 * sin(tick / 3000.0f);
            pos.z = 100 * sin(tick / 3000.0f);

            tick++;
        }
    }

private:

    Ref<ModelLoader> m_ModelLoader;
    Ref<MaterialLoader> m_MaterialLoader;
    Ref<TextureLoader> m_TextureLoader;
    Ref<FontLoader> m_FontLoader;

    Entity m_DirectLight;
    Entity m_Text3D;
    Entity m_Plane;
    Entity m_WinterGirl;

    TestConfig m_TestConfig = string("TestConfig");
};

Application* CreateApplication() {
    Application* app = new GameApp();

    // read app configs
    app->Config = string("AppConfig");
    if (!xpe::res::ReadJsonFile("config/config.json", app->Config))
    {
        FMT_ASSERT(false, "Failed to read app config from config/config.json file. Please provide config file!");
        return 0;
    }

    return app;
}