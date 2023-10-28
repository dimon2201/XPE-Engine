#include <core/app.hpp>
#include <launcher.h>

#include <ecs/scenes.hpp>

#include <model_loader.h>
#include <material_loader.h>
#include <font_loader.h>
#include <texture_loader.h>
#include <skeleton_loader.h>
#include <anim_loader.h>
#include <audio_loader.h>

#include <rendering/passes/fxaa_pass.hpp>
#include <rendering/passes/ssao_pass.hpp>

#include "test_config.h"

using namespace xpe::core;
using namespace xpe::ecs;
using namespace xpe::render;
using namespace xpe::math;
using namespace xpe::res;
using namespace xpe::audio;
using namespace xpe::physics;

class GameApp : public Application
{
public:
    GameApp() {}
    ~GameApp() {}

protected:

    void InitRenderPasses() override
    {
        Application::InitRenderPasses();
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

        // save xml configs from file
        if (!m_XmlConfig.SaveFile("config/xml_config_saved.xml"))
        {
            LogError("xml_config_saved.xml file not found in config/xml_config_saved.xml path.");
        }

        AddWindowClosed(GameApp, 1);
        AddKeyPressed(GameApp, 1);
        AddKeyHold(GameApp, 1);
        AddCursorMove(GameApp, 1);

        m_ModelLoader.Create();
        m_MaterialLoader.Create();
        m_TextureLoader.Create();
        m_FontLoader.Create();
        m_SkeletLoader.Create();
        m_AnimLoader.Create();
        m_AudioLoader.Create();

        InitCamera();
        InitCamera2D();

        // Create physics scene
        PhysicsManager::AddScene(m_MainScene);

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
            m_Text3D->Transform.Position = { 0, 25, 50 };
            m_Text3D->Transform.Scale = { 0.25, 0.25, 1 };

            auto* textComponent = m_Text3D->AddComponent<Text3DComponent>(
                    "Text3D",
                    m_FontLoader->Load("res/fonts/Roboto-Bold.ttf", 32),
                    "Hi,\nWelcome to Example Window\nThis is a testing version of XPE-Engine"
            );
            textComponent->Font->NewLineOffset = 1.0f;
        }

        // setup skybox global
        {
            TextureCubeFilepath skyboxPath;
            skyboxPath.Name = "Skybox";
            skyboxPath.FrontFilepath = "res/skybox/front.jpg";
            skyboxPath.BackFilepath = "res/skybox/back.jpg";
            skyboxPath.LeftFilepath = "res/skybox/left.jpg";
            skyboxPath.RightFilepath = "res/skybox/right.jpg";
            skyboxPath.TopFilepath = "res/skybox/top.jpg";
            skyboxPath.BottomFilepath = "res/skybox/bottom.jpg";

            m_MainScene->Skybox->Geometry = GeometryManager::AddGeometry(Cube());
            m_MainScene->Skybox->Texture = m_TextureLoader->LoadCube(skyboxPath, eTextureFormat::RGBA8);
            m_MainScene->Skybox->Texture->GenerateMips();
        }

        // setup plane
        {
            m_Plane = new Entity("Plane", m_MainScene);
            m_Plane->Transform.Position = { 0, -10, 0 };
            m_Plane->Transform.Scale = { 1, 1, 1 };

            m_Plane->AddComponent<GeometryComponent>("G_Plane", GeometryManager::AddGeometry(Plane(100)));
            auto& planeMaterial = m_Plane->AddComponent<MaterialComponent>("Plane", MaterialManager::AddMaterial("PlaneMaterial", Material()))->Material;
            planeMaterial->Metallness = 0.0f;
            planeMaterial->Roughness = 0.05f;
            planeMaterial->AO = 0.0f;
            planeMaterial->Flush();

            m_Plane->AddComponent<RigidBodyComponent>(
                PhysicsManager::AddActor(
                    m_Plane,
                    m_MainScene,
                    sActor::eActorType::RIGID_STATIC,
                    sActor::eShapeType::BOX,
                    glm::vec3(0.0f),
                    0.5f, 0.5f, 0.5f,
                    0.01f, 0.0f
                )
            );
        }

        // setup point light
        {
            m_SunLight = new Entity("SunLight", m_MainScene);
            m_SunLight->Transform.Position = { 20, 20, -20 };

            m_SunLight->AddComponent<GeometryComponent>("G_SunLight", GeometryManager::AddGeometry(Sphere()));
            m_SunLight->AddComponent<MaterialComponent>("SunLight", MaterialManager::AddMaterial("MT_SunLight", Material()));
            m_SunLight->AddComponent<DirectLightComponent>("L_SunLight", glm::vec3(0, 0, 0), glm::vec3(50, 30, 10));
        }

        // setup goblins
        {
            m_Goblin1 = new Entity("Goblin1", m_MainScene);
            m_Goblin1->Transform.Position = {-4, -10, -4 };
            m_Goblin1->Transform.Rotation = {0, 0, 0 };
            m_Goblin1->Transform.Scale = {5, 5, 5 };

            m_Goblin2 = new Entity("Goblin2", m_MainScene);
            m_Goblin2->Transform.Position = {-4, -10, 4 };
            m_Goblin2->Transform.Rotation = {0, 0, 0 };
            m_Goblin2->Transform.Scale = {5, 5, 5 };

            m_Goblin3 = new Entity("Goblin3", m_MainScene);
            m_Goblin3->Transform.Position = {4, -10, -4 };
            m_Goblin3->Transform.Rotation = {0, 0, 0 };
            m_Goblin3->Transform.Scale = {5, 5, 5 };

            m_Goblin4 = new Entity("Goblin4", m_MainScene);
            m_Goblin4->Transform.Position = {4, -10, 4 };
            m_Goblin4->Transform.Rotation = {0, 0, 0 };
            m_Goblin4->Transform.Scale = {5, 5, 5 };

            auto* goblinModel = m_Goblin1->AddComponent<SkeletonModelComponent>(
                "GoblinModel",
                m_ModelLoader->Load("res/models/winter-girl/source/dancing_vampire.dae"),
                m_SkeletLoader->Load("res/models/winter-girl/source/dancing_vampire.dae"),
                vector<Entity*> { m_Goblin1, m_Goblin2, m_Goblin3, m_Goblin4 }
            );

            goblinModel->Visible = false;

            auto* goblinAnimation = m_Goblin1->AddComponent<SkeletonAnimationComponent>(
                "GoblinAnimation",
                goblinModel->Skeleton,
                m_AnimLoader->Load("res/models/winter-girl/source/dancing_vampire.dae")
            );

            goblinAnimation->Play = true;

            MaterialFilepath materialFilepath;
            materialFilepath.Name = "niz";
            materialFilepath.AlbedoFilepath = "res/models/winter-girl/textures/Vampire_diffuse.png";
            materialFilepath.BumpFilepath = "res/models/winter-girl/textures/Vampire_normal.png";
//                materialFilepath.RoughnessFilepath = "res/models/winter-girl/textures/Vampire_diffuse.png";
//                materialFilepath.MetallicFilepath = "res/models/winter-girl/textures/Vampire_specular.png";

            auto& material1 = m_Goblin1->AddComponent<MaterialComponent>(m_Goblin1->GetTag(), m_MaterialLoader->Load(materialFilepath))->Material;
            material1->Albedo = { 1, 1, 1, 1 };
            material1->Emission = { 0, 0, 10 };
            material1->Metallness = 0.5;
            material1->Roughness = 0.5;
            material1->AO = 0.5;
            material1->EnableAlbedoMap = false;
            material1->EnableNormalMap = false;
            material1->EnableRoughnessMap = false;
            material1->EnableMetalMap = false;
            material1->EnableAOMap = false;
            material1->Flush();

            auto& material2 = m_Goblin2->AddComponent<MaterialComponent>(m_Goblin2->GetTag(), m_MaterialLoader->Load(materialFilepath))->Material;
            material2->Albedo = { 1, 1, 1, 1 };
            material2->Emission = { 0, 0, 10 };
            material2->Metallness = 0.5;
            material2->Roughness = 0.5;
            material2->AO = 0.5;
            material2->EnableAlbedoMap = false;
            material2->EnableNormalMap = false;
            material2->EnableRoughnessMap = false;
            material2->EnableMetalMap = false;
            material2->EnableAOMap = false;
            material2->Flush();

            auto& material3 = m_Goblin3->AddComponent<MaterialComponent>(m_Goblin3->GetTag(), m_MaterialLoader->Load(materialFilepath))->Material;
            material3->Albedo = { 1, 1, 1, 1 };
            material3->Emission = { 0, 0, 10 };
            material3->Metallness = 0.5;
            material3->Roughness = 0.5;
            material3->AO = 0.5;
            material3->EnableAlbedoMap = false;
            material3->EnableNormalMap = false;
            material3->EnableRoughnessMap = false;
            material3->EnableMetalMap = false;
            material3->EnableAOMap = false;
            material3->Flush();

            auto& material4 = m_Goblin4->AddComponent<MaterialComponent>(m_Goblin4->GetTag(), m_MaterialLoader->Load(materialFilepath))->Material;
            material4->Albedo = { 1, 1, 1, 1 };
            material4->Emission = { 0, 0, 10 };
            material4->Metallness = 0.5;
            material4->Roughness = 0.5;
            material4->AO = 0.5;
            material4->EnableAlbedoMap = false;
            material4->EnableNormalMap = false;
            material4->EnableRoughnessMap = false;
            material4->EnableMetalMap = false;
            material4->EnableAOMap = false;
            material4->Flush();

            // for testing image resizing on CPU, we can write resized image into file

            TextureLoader::SaveLayer(
                    "generated/Vampire_diffuse_resized.png",
                    material1->AlbedoMap,
                    Texture::eFileFormat::PNG
            );

            TextureLoader::SaveLayer(
                    "generated/Vampire_normal_resized.png",
                    material1->NormalMap,
                    Texture::eFileFormat::PNG
            );

            TextureLoader::SaveLayer(
                    "generated/Vampire_roughness_resized.png",
                    material1->RoughnessMap,
                    Texture::eFileFormat::PNG
            );

            TextureLoader::SaveLayer(
                    "generated/Vampire_metallic_resized.png",
                    material1->MetalMap,
                    Texture::eFileFormat::PNG
            );
        }

        // setup cube
        {
            m_Cube = new Entity("Cube", m_MainScene);
            m_Cube->Transform.Position = { 10, -7.6, 10 };
            m_Cube->Transform.Scale = { 5, 5, 5 };
            m_Cube->AddComponent<GeometryComponent>("G_Cube", GeometryManager::AddGeometry(Cube()));
            m_Cube->AddComponent<MaterialComponent>("Cube", MaterialManager::AddMaterial("MT_Cube", Material()));
        }

        // setup glasses
        for (s32 i = 0; i < 4; i++)
        {
            auto mat = Material();
            if (i == 0) { mat.Albedo = glm::vec4(1.0f, 0.0f, 0.0f, 0.2f); }
            if (i == 1) { mat.Albedo = glm::vec4(0.0f, 1.0f, 0.0f, 0.4f); }
            if (i == 2) { mat.Albedo = glm::vec4(0.0f, 0.0f, 1.0f, 0.6f); }
            if (i == 3) { mat.Albedo = glm::vec4(1.0f, 1.0f, 0.0f, 0.8f); }

            mat.Metallness = 0.0f;
            mat.Roughness = 0.05f;
            mat.AO = 0.0f;

            m_Glasses[i] = new Entity("Glass" + i, m_MainScene);
            m_Glasses[i]->Transform.Position = { 10, 2, 10 + (i * 2)};
            m_Glasses[i]->Transform.Scale = { 1, 1, 1 };
            m_Glasses[i]->AddComponent<GeometryComponent>("G_Glass" + i, GeometryManager::AddGeometry(Sphere()));
            m_Glasses[i]->AddComponent<MaterialComponent>("Glass" + i, MaterialManager::Add("MT_Glass" + i, mat));
            m_Glasses[i]->GetComponent<GeometryComponent>("G_Glass" + i)->Transparent = true;
        }

        m_Listener = new Entity("Listener", m_MainScene);

        auto& camera = *m_MainScene->PerspectiveCamera; // Get camera to set listener's position, up and look

        ListenerComponent component("Listener");

        component.Position = &camera.Component.Position;
        component.Up = camera.Component.Up;
        component.Look = &camera.Component.Front;

        m_Listener->AddComponent<ListenerComponent>(component);

        //loading stream audio files
        {
            m_AudioObject = new Entity("AudioObject", m_MainScene);

            //load test stream audio
            {
                m_AudioObject->AddComponent<AudioFileComponent>("Test", m_AudioLoader->Load("res/audio/test.wav"));
            }

            //load spell stream audio
            {
                m_AudioObject->AddComponent<AudioFileComponent>("Magicfail", m_AudioLoader->Load("res/audio/magicfail.ogg"));
            }

            //load magicfail stream audio
            {
                m_AudioObject->AddComponent<AudioFileComponent>("Spell", m_AudioLoader->Load("res/audio/spell.ogg"));
            }

        }
        //temporarily commented
        //// create voice component
        //{
        //    VoiceComponent component("Test");
        //    m_MainScene->AddComponent<VoiceComponent>(m_MainScene->Audio->GetTag(), component);
        //}

        //setup background audio
        {
            m_BackgroundAudio = new Entity("BackgroundAudio", m_MainScene);

            //test sources
            SourceAudioComponent* source;
            SourceAudioComponent* source1;
            SourceAudioComponent* source2;

            //test files //You can use one file for many sources in one time
            auto* file = m_AudioObject->GetComponent<AudioFileComponent>("Spell");
            auto* file1 = m_AudioObject->GetComponent<AudioFileComponent>("Test");
            auto* file2 = m_AudioObject->GetComponent<AudioFileComponent>("Magicfail");

            //creating source
            {
                SourceAudioComponent component("Sound_Test");

                component.Position = { -15.0f, 2.0f, 0.0f };
                component.Looping = true;
                component.Gain = 0.5f;
                component.RefDistance = 10.0f;
                component.MaxDistance = 100.0f;
                source = m_BackgroundAudio->AddComponent<SourceAudioComponent>(component);
            }

            //creating stream audio
            {
                AudioComponent component("Sound_Test");

                component.Source = source;
                component.File = file->File;

                m_BackgroundAudio->AddComponent<AudioComponent>(component);
            }

            //-------------------------------------
            //creating source
            {
                SourceAudioComponent component("Test");

                component.Position = { -5.0f, 2.0f, 0.0f };
                component.Looping = true; //(todo) Need do some logic to looping the stream audio
                component.Gain = 0.2f;
                component.RefDistance = 10.0f;
                component.MaxDistance = 100.0f;
                source1 = m_BackgroundAudio->AddComponent<SourceAudioComponent>(component);
            }

            //creating stream audio
            {
                StreamAudioComponent component("Test");

                component.Source = source1;
                component.File = file1->File;
                component.BufferSamples = 8192 * 2; //for test
                component.NumBuffers = 5; //for test

                m_BackgroundAudio->AddComponent<StreamAudioComponent>(component);
            }

            //-------------------------------------
            //creating source
            {
                SourceAudioComponent component("Sound_Test1");

                component.Position = { 11.0f, 2.0f, 0.0f };
                component.Looping = true;
                component.Gain = 0.5f;
                component.RefDistance = 10.0f;
                component.MaxDistance = 100.0f;
                source2 = m_BackgroundAudio->AddComponent<SourceAudioComponent>(component);
            }

            //creating stream audio
            {
                AudioComponent component("Sound_Test1");

                component.Source = source2;
                component.File = file2->File;

                m_BackgroundAudio->AddComponent<AudioComponent>(component);
            }
        }

        m_SsaoPass->GetData().Intensity = 4;
        m_SsaoPass->Flush();
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
        delete m_SunLight;
        delete m_Text2D;
        delete m_Text3D;
        delete m_Plane;
        delete m_Goblin1;
        delete m_Goblin2;
        delete m_Goblin3;
        delete m_Goblin4;
        delete m_Cube;
        delete m_Listener;
        delete m_BackgroundAudio;
        delete m_AudioObject;
        delete m_Glasses[0];
        delete m_Glasses[1];
        delete m_Glasses[2];
        delete m_Glasses[3];
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
        PlayAnimations(key);
    }

    void KeyHold(const eKey key)
    {
        MoveLight(key);
        PlayAnimations(key);
    }

    void CursorMoved(const double x, const double y)
    {
        auto& camera = *m_MainScene->PerspectiveCamera;
        if (InputManager::MousePressed(eMouse::ButtonRight)) {
            camera.EnableLook = false;
            InputManager::CaptureCursor(x, y);
            auto& cursorDelta = InputManager::GetMouseCursor().Delta;
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
        if (InputManager::MousePressed(eMouse::ButtonLeft)) {
            camera.LookMode = Camera::eLookMode::EDITOR;
        } else {
            camera.LookMode = Camera::eLookMode::GAME;
        }
        camera.Move();
    }

    void MoveLight(const eKey key)
    {
        auto& pos = m_SunLight->Transform.Position;

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

    void PlayAnimations(const eKey key)
    {
        if (key == eKey::P)
        {
            auto* goblinAnimation = m_Goblin1->GetComponent<SkeletonAnimationComponent>("GoblinAnimation");
            goblinAnimation->Play = !goblinAnimation->Play;
        }
    }

    void AnimateLight()
    {
        if (m_TestConfig.AnimateLight)
        {
            auto& pos = m_SunLight->Transform.Position;

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
                 "\nGamma: " + std::to_string(WindowManager::GetGamma()) +
                 "\nExposure: " + std::to_string(WindowManager::GetExposure());

        auto& text3D = m_Text3D->GetComponent<Text3DComponent>("Text3D")->Text;
        text3D = "\nFPS: " + std::to_string(CPUTime.Fps()) +
                 "\nCPU: " + std::to_string(CPUTime.Millis()) +
                 "\nGamma: " + std::to_string(WindowManager::GetGamma()) +
                 "\nExposure: " + std::to_string(WindowManager::GetExposure());
    }

private:
    Ref<ModelLoader> m_ModelLoader;
    Ref<MaterialLoader> m_MaterialLoader;
    Ref<TextureLoader> m_TextureLoader;
    Ref<FontLoader> m_FontLoader;
    Ref<SkeletonLoader> m_SkeletLoader;
    Ref<AnimLoader> m_AnimLoader;
    Ref<AudioLoader> m_AudioLoader;

    Entity* m_SunLight;
    Entity* m_Text2D;
    Entity* m_Text3D;
    Entity* m_Plane;
    Entity* m_Goblin1;
    Entity* m_Goblin2;
    Entity* m_Goblin3;
    Entity* m_Goblin4;
    Entity* m_Cube;
    Entity* m_Listener;
    Entity* m_BackgroundAudio;
    Entity* m_AudioObject;
    Entity* m_Glasses[4];

    TestConfig m_TestConfig = string("TestConfig");
    XmlConfig  m_XmlConfig = string("XmlConfig");
};

Application* CreateApplication() {
    Application* app = new GameApp();

    // read app configs
    app->Config = string("AppConfig");
    if (!xpe::res::ReadJsonFile("config/app_config.json", app->Config))
    {
        FMT_ASSERT(false, "Failed to read app config from config/config.json file. Please provide config file!");
        return 0;
    }

    return app;
}