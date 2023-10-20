#include <core/app.hpp>
#include <launcher.h>

#include <ecs/entities.hpp>
#include <ecs/scenes.hpp>

#include <model_loader.h>
#include <material_loader.h>
#include <font_loader.h>
#include <texture_loader.h>
#include <skin_loader.h>
#include <skelet_loader.h>
#include <anim_loader.h>
#include <audio_loader.h>

#include <rendering/monitor.h>

#include "test_config.h"

using namespace xpe::core;
using namespace xpe::ecs;
using namespace xpe::render;
using namespace xpe::control;
using namespace xpe::math;
using namespace xpe::res;
using namespace xpe::audio;

class GameApp : public Application
{
public:
    GameApp() {}
    ~GameApp() {}

protected:

    void InitRenderer() override
    {
        Application::InitRenderer();
    }

public:

    void Init() override final {
        LogInfo("GameApp::Init()");

        // read test configs from file
        if (!xpe::res::ReadJsonFile("config/test_config.json", m_TestConfig))
        {
            LogError("test_config.json file not found in config/test_config.json path.");
        }

        // read xml configs from file
        if (!m_XmlConfig.LoadFile("config/xml_config.xml"))
        {
            LogError("xml_config.xml file not found in config/xml_config.xml path.");
        }

        AddWindowClosed(GameApp, 1);
        AddKeyPressed(GameApp, 1);
        AddKeyHold(GameApp, 1);
        AddCursorMove(GameApp, 1);

        m_ModelLoader.Create();
        m_MaterialLoader.Create(m_MaterialStorage);
        m_TextureLoader.Create(m_TextureStorage);
        m_FontLoader.Create(m_FontStorage);
        m_SkeletLoader.Create(m_SkeletStorage);
        m_SkinLoader.Create();
        m_AnimLoader.Create(m_AnimStorage);
        m_AudioLoader.Create(m_AudioStorage);

        InitCamera();
        InitCamera2D();

        // setup text 2D entity
        {
            m_Text2D = new Entity("Text2D", m_MainScene);
            m_Text2D->Transform.Position = { 0, WindowManager::GetHeight(), 0 };
            m_Text2D->Transform.Scale = { 1, 1, 1 };

            auto* textComponent = m_Text2D->AddComponent<Text2DComponent>(
                    "Text2D",
                    m_FontLoader->Load("res/fonts/Roboto-Bold.ttf", 32),
                    "FPS: \n CPU: \n"
            );
            textComponent->Font->NewLineOffset = 1.0f;

            m_Text2D->Transform.Position.y -= textComponent->Font->GlyphSize;
        }

        // setup text 3D entity
        {
            m_Text3D = new Entity("Text3D", m_MainScene);
            m_Text3D->Transform.Position = {0, -10, 0 };
            m_Text3D->Transform.Scale = {10, 10, 1 };

            auto* textComponent = m_Text3D->AddComponent<Text3DComponent>(
                    "Text3D",
                    m_FontLoader->Load("res/fonts/Roboto-Bold.ttf", 44),
                    "Hi,\nWelcome to Example Window\nThis is a testing version of XPE-Engine"
            );
            textComponent->Font->NewLineOffset = 1.0f;
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
            m_MainScene->Skybox->CubeTexture->GenerateMips();
        }

        // setup plane
        {
            m_Plane = new Entity("Plane", m_MainScene);
            m_Plane->Transform.Position = { 0, -10, 0 };
            m_Plane->Transform.Scale = { 100, 0.1, 100 };

            m_Plane->AddComponent<GeometryComponent<Vertex3D>>("G_Plane", GeometryManager::AddGeometry(Cube()));
            auto& planeMaterial = m_Plane->AddComponent<MaterialComponent>("Plane", m_MaterialStorage->Add("PlaneMaterial", Material()))->Material;
            planeMaterial->Albedo = { 1, 1, 1, 1 };
            planeMaterial->Metallness = 0.5;
            planeMaterial->Roughness = 0.5;
            planeMaterial->AO = 0.5;
            planeMaterial->Flush();
            m_Plane->AddComponent<OpaqueComponent>("Opaque");
        }

        // setup direct light
        {
            m_DirectLight = new Entity("DirectLight", m_MainScene);

            DirectLightComponent directLight("L_Direct");
            directLight.Position = { 0, -5, 0 };
            directLight.Color = { 0, 0, 0 };

            m_DirectLight->AddComponent<DirectLightComponent>(directLight);
        }

        // setup point light
        {
            m_PointLight = new Entity("PointLight", m_MainScene);
            m_PointLight->Transform.Position = {20, 20, -20 };
            m_PointLight->Transform.Scale = {0.5, 0.5, 0.5 };

            m_PointLight->AddComponent<GeometryComponent<Vertex3D>>("G_Point", GeometryManager::AddGeometry(Sphere()));
            m_PointLight->AddComponent<MaterialComponent>("PointLight", m_MaterialStorage->Add("MT_PointLight", Material()));
            auto& material = m_PointLight->GetComponent<MaterialComponent>("PointLight")->Material;
            material->Albedo = { 1, 0, 0, 1 };
            material->Flush();
            m_PointLight->AddComponent<DirectLightComponent>("L_Point", glm::vec3(0, 0, 0), glm::vec3(15.3, 15.1, 11.1));
        }

        // setup cubes
        {
            for (s32 i = 0; i < 4; i++)
            {
                m_Planes[i] = new Entity("Plane" + i, m_MainScene);
                m_Planes[i]->Transform.Position = { 10, 2, 10 + (i * 2)};
                m_Planes[i]->Transform.Scale = { 1, 1, 0.1 };
                m_Planes[i]->AddComponent<GeometryComponent<Vertex3D>>("G_Plane" + i, GeometryManager::AddGeometry(Cube()));
                m_Planes[i]->AddComponent<MaterialComponent>("Plane" + i, m_MaterialStorage->Add("MT_Plane" + i, Material()));
                m_Planes[i]->AddComponent<OpaqueComponent>("Opaque");
            }
        }
    }

    void Update() override final
    {
        LockFPSFromConfig();
        UpdateCamera();
        AnimateLight();
        DisplayStats();
    }

    void Free()
    {
        LogInfo("GameApp::Free()");
        delete m_DirectLight;
        delete m_PointLight;
        delete m_Text2D;
        delete m_Text3D;
        delete m_Plane;
        delete m_Planes[0];
        delete m_Planes[1];
        delete m_Planes[2];
        delete m_Planes[3];
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
        auto& pos = m_PointLight->Transform.Position;

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

    void AnimateLight()
    {
        if (m_TestConfig.AnimateLight)
        {
            auto& pos = m_PointLight->Transform.Position;

            // translation light up and down every N ticks
            static int tick = 1;

            pos.x = 100 * sin(tick / 3000.0f);
            pos.z = 100 * sin(tick / 3000.0f);

            tick++;
        }
    }

    void DisplayStats() {
        auto& text2D = m_Text2D->GetComponent<Text2DComponent>("Text2D")->Text;
        text2D = "\nFPS: " + std::to_string(CPUTime.Fps()) +
                 "\nCPU: " + std::to_string(CPUTime.Millis()) +
                 "\nGamma: " + std::to_string(Monitor::Get().Gamma) +
                 "\nExposure: " + std::to_string(Monitor::Get().Exposure);

        auto& text3D = m_Text3D->GetComponent<Text3DComponent>("Text3D")->Text;
        text3D = "\nFPS: " + std::to_string(CPUTime.Fps()) +
                 "\nCPU: " + std::to_string(CPUTime.Millis()) +
                 "\nGamma: " + std::to_string(Monitor::Get().Gamma) +
                 "\nExposure: " + std::to_string(Monitor::Get().Exposure);
    }

private:
    Ref<ModelLoader> m_ModelLoader;
    Ref<MaterialLoader> m_MaterialLoader;
    Ref<TextureLoader> m_TextureLoader;
    Ref<FontLoader> m_FontLoader;
    Ref<SkeletLoader> m_SkeletLoader;
    Ref<SkinLoader> m_SkinLoader;
    Ref<AnimLoader> m_AnimLoader;
    Ref<AudioLoader> m_AudioLoader;

    Entity* m_DirectLight;
    Entity* m_PointLight;
    Entity* m_Text2D;
    Entity* m_Text3D;
    Entity* m_Plane;
    Entity* m_Planes[4];

    TestConfig m_TestConfig = string("TestConfig");
    XmlConfig  m_XmlConfig = string("XmlConfig");
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