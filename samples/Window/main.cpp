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

#include <rendering/storages/material_storage.h>

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
        m_SkeletLoader.Create(m_SkeletStorage);
        m_SkinLoader.Create(m_SkinStorage);
        m_AnimLoader.Create(m_AnimStorage);

        InitCamera();
        InitCamera2D();

        // setup text 2D entity
        {
            m_Text2D = { "Text2D", m_MainScene };

            Text2DComponent text("Text2D");
            text.FontResFilepath = "res/fonts/Roboto-Bold.ttf";
            text.Font = m_FontLoader->Load(text.FontResFilepath.c_str(), 32);
            text.Text = "FPS: \n CPU: \n";
            text.Font->NewLineOffset = 1.0f;
            text.Transform.Position = { 0, WindowManager::GetHeight() - text.Font->GlyphSize, 0 };
            text.Transform.Scale = { 1, 1, 1 };

            m_Text2D.AddComponent<Text2DComponent>(text);
        }

        // setup text 3D entity
        {
            m_Text3D = { "Text3D", m_MainScene };

            Text3DComponent text("Text3D");
            text.FontResFilepath = "res/fonts/Roboto-Bold.ttf";
            text.Font = m_FontLoader->Load(text.FontResFilepath.c_str(), 44);
            text.Text = "Hi,\nWelcome to Example Window\nThis is a testing version of XPE-Engine";
            text.Font->NewLineOffset = 1.0f;
            text.Transform.Position = { 0, -10, 0 };
            text.Transform.Scale = {10, 10, 1 };

            m_Text3D.AddComponent<Text3DComponent>(text);
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

//            m_MainScene->Skybox->CubeTexture = m_TextureLoader->LoadCube(skyboxPath, eTextureFormat::RGBA8);
//            m_MainScene->Skybox->CubeTexture->GenerateMips();
        }

        // setup plane
        {
            m_Plane = { "Plane", m_MainScene };

            GeometryIndexed3DComponent plane("G_Plane");
            plane.Geometry = m_GeometryStorage->AddGeometryIndexed3D("G_Plane", Cube());
            plane.Instance.Transform.Position = { 0, -10, 0 };
            plane.Instance.Transform.Scale = { 100, 0.1, 100 };
            plane.Instance.Material = m_MaterialStorage->Add("MT_Plane", Material());
            plane.Instance.Material->MetallicFactor = 0;
            plane.Instance.Material->RoughnessFactor = 1;
            plane.Instance.Material->Flush();

            m_Plane.AddComponent<GeometryIndexed3DComponent>(plane);
        }

        // setup direct light
        {
            m_DirectLight = { "DirectLight", m_MainScene };

            DirectLightComponent directLight("L_Direct");
            directLight.Position = { 0, -5, 0 };
            directLight.Color = { 0, 0, 0 };

            m_DirectLight.AddComponent<DirectLightComponent>(directLight);
        }

        // setup point light
        {
            m_PointLight = { "PointLight", m_MainScene };

            GeometryIndexed3DComponent pointLightShape("G_Point");
            pointLightShape.Geometry = m_GeometryStorage->AddGeometryIndexed3D("G_Point", Cube());
            pointLightShape.Instance.Transform.Position = { 0, 0, 0 };
            pointLightShape.Instance.Transform.Scale = { 1, 1, 1 };
            pointLightShape.Instance.Material = m_MaterialStorage->Add("MT_Point", Material());
            pointLightShape.Instance.Material->BaseColor = { 1, 1, 0, 1 };
            pointLightShape.Instance.Material->Flush();
            m_PointLight.AddComponent<GeometryIndexed3DComponent>(pointLightShape);

            PointLightComponent pointLight("L_Point");
            pointLight.Position = { 0, 0, 0 };
            pointLight.Color = { 10, 10, 10 };
            m_PointLight.AddComponent<PointLightComponent>(pointLight);
        }

        // setup 3D model
        {
            m_WinterGirl = { "WinterGirl", m_MainScene };

            SkinModelListComponent winterGirlModel("M_WinterGirl");
            winterGirlModel.Model = m_SkinLoader->Load("res/models/winter-girl/source/dancing_vampire.dae");
            winterGirlModel.Skelet = m_SkeletLoader->Load("res/models/winter-girl/source/dancing_vampire.dae");

            SkeletalAnimationComponent winterGirlAnimation("A_WinterGirl");
            winterGirlAnimation.Skelet = winterGirlModel.Skelet;
            winterGirlAnimation.Animation = m_AnimLoader->Load("res/models/winter-girl/source/dancing_vampire.dae");
            winterGirlAnimation.Play = true;

            {
                Transform transform;
                transform.Position = { -4, -10, -4 };
                transform.Rotation = { 0, 0, 0 };
                transform.Scale = { 5, 5, 5 };
                winterGirlModel.Transforms.emplace_back(transform);
            }

            {
                Transform transform;
                transform.Position = { -4, -10, 4 };
                transform.Rotation = { 0, 0, 0 };
                transform.Scale = { 5, 5, 5 };
                winterGirlModel.Transforms.emplace_back(transform);
            }

            {
                Transform transform;
                transform.Position = { 4, -10, -4 };
                transform.Rotation = { 0, 0, 0 };
                transform.Scale = { 5, 5, 5 };
                winterGirlModel.Transforms.emplace_back(transform);
            }

            {
                Transform transform;
                transform.Position = { 4, -10, 4 };
                transform.Rotation = { 0, 0, 0 };
                transform.Scale = { 5, 5, 5 };
                winterGirlModel.Transforms.emplace_back(transform);
            }

            {
                MaterialFilepath materialFilepath;
                materialFilepath.Name = "niz";
                materialFilepath.AlbedoFilepath = "res/models/winter-girl/textures/Vampire_diffuse.png";
//                materialFilepath.BumpFilepath = "res/models/winter-girl/textures/Vampire_normal.png";
                materialFilepath.MetallicFilepath = "res/models/winter-girl/textures/Vampire_specular.png";
                winterGirlModel.Model->Skins[0].Material = m_MaterialLoader->Load(materialFilepath);
                winterGirlModel.Model->Skins[0].Material->EmissionColor = { 1, 1, 1 };
                winterGirlModel.Model->Skins[0].Material->Flush();
            }

            {
                MaterialFilepath materialFilepath;
                materialFilepath.Name = "04";
                materialFilepath.AlbedoFilepath = "res/models/winter-girl/textures/LOW_04_BaseColor.png";
                materialFilepath.BumpFilepath = "res/models/winter-girl/textures/LOW_04_Normal.png";
                materialFilepath.MetallicFilepath = "res/models/winter-girl/textures/LOW_04_Metallic.png";
                materialFilepath.RoughnessFilepath = "res/models/winter-girl/textures/LOW_04_Roughness.png";
//                winterGirlModel.Model->Skins[1].Material = m_MaterialLoader->Load(materialFilepath);
            }

            {
                MaterialFilepath materialFilepath;
                materialFilepath.Name = "R1";
                materialFilepath.AlbedoFilepath = "res/models/winter-girl/textures/LOW_R1_BaseColor.png";
                materialFilepath.BumpFilepath = "res/models/winter-girl/textures/LOW_R1_Normal.png";
                materialFilepath.MetallicFilepath = "res/models/winter-girl/textures/LOW_R1_Metallic.png";
                materialFilepath.RoughnessFilepath = "res/models/winter-girl/textures/LOW_R1_Roughness.png";
//                winterGirlModel.Model->Skins[2].Material = m_MaterialLoader->Load(materialFilepath);
            }

            {
                MaterialFilepath materialFilepath;
                materialFilepath.Name = "verx";
                materialFilepath.AlbedoFilepath = "res/models/winter-girl/textures/LOW_verx_BaseColor.png";
                materialFilepath.BumpFilepath = "res/models/winter-girl/textures/LOW_verx_Normal.png";
                materialFilepath.MetallicFilepath = "res/models/winter-girl/textures/LOW_verx_Metallic.png";
                materialFilepath.RoughnessFilepath = "res/models/winter-girl/textures/LOW_verx_Roughness.png";
//                winterGirlModel.Model->Skins[3].Material = m_MaterialLoader->Load(materialFilepath);
            }

            {
                MaterialFilepath materialFilepath;
                materialFilepath.Name = "pngegg";
                materialFilepath.AlbedoFilepath = "res/models/winter-girl/textures/pngegg.png";
//                winterGirlModel.Model->Skins[4].Material = m_MaterialLoader->Load(materialFilepath);
            }

            m_WinterGirl.AddComponent<SkinModelListComponent>(winterGirlModel);
            m_WinterGirl.AddComponent<SkeletalAnimationComponent>(winterGirlAnimation);
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
        auto* pointLight = m_PointLight.GetComponent<PointLightComponent>("L_Point");
        auto& pos = pointLight->Position;
        auto* pointLightShape = m_PointLight.GetComponent<GeometryIndexed3DComponent>("G_Point");
        pointLightShape->Instance.Transform.Position = pos;

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

        if (key == eKey::P)
        {
            auto* winterGirlAnim = m_WinterGirl.GetComponent<SkeletalAnimationComponent>("A_WinterGirl");
            winterGirlAnim->Play = !winterGirlAnim->Play;
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

        auto& text2D = m_Text2D.GetComponent<Text2DComponent>("Text2D")->Text;
        text2D = "XPE-Engine \n FPS: " + std::to_string(CPUTime.Fps()) + "\n CPU: " + std::to_string(CPUTime.Millis());
    }

private:

    Ref<ModelLoader> m_ModelLoader;
    Ref<MaterialLoader> m_MaterialLoader;
    Ref<TextureLoader> m_TextureLoader;
    Ref<FontLoader> m_FontLoader;
    Ref<SkeletLoader> m_SkeletLoader;
    Ref<SkinLoader> m_SkinLoader;
    Ref<AnimLoader> m_AnimLoader;

    Entity m_DirectLight;
    Entity m_PointLight;
    Entity m_Text2D;
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