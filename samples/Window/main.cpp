#include <core/app.hpp>
#include <launcher.h>

#include <rendering/render_system.h>
#include <rendering/skybox_manager.h>
#include <rendering/camera.h>

#include <ecs/components.hpp>

#include <model_loader.h>
#include <material_loader.h>
#include <font_loader.h>
#include <texture_loader.h>
#include <skeleton_loader.h>
#include <anim_loader.h>
#include <audio_loader.h>

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

        FontLoader::Init();
        TextureLoader::Init();
        AudioLoader::Init();

        InitCamera();

        // Create physics scene
        PhysicsManager::AddScene(m_Scene);

        // Text 2D
        {
            m_Text2D = new Entity("Text2D", m_Scene);
            m_Text2D->Transform.Position = { 0, WindowManager::GetHeight(), 0 };
            m_Text2D->Transform.Scale = { 1, 1, 1 };

            auto* textComponent = m_Text2D->Add<Text2DComponent>();
            textComponent->Text = "FPS: \n CPU: \n";
            textComponent->Font = FontLoader::Load("res/fonts/Roboto-Bold.ttf", 32);
            textComponent->Font->NewLineOffset = 1.0f;

            m_Text2D->Transform.Position.y -= textComponent->Font->GlyphSize;
        }

        // Text 3D
        {
            m_Text3D = new Entity("Text3D", m_Scene);
            m_Text3D->Transform.Position = { 0, 25, 50 };
            m_Text3D->Transform.Scale = { 0.25, 0.25, 1 };

            auto* textComponent = m_Text3D->Add<Text3DComponent>();
            textComponent->Text = "Hi,\nWelcome to Example Window\nThis is a testing version of XPE-Engine";
            textComponent->Font = FontLoader::Load("res/fonts/Roboto-Bold.ttf", 32);
            textComponent->Font->NewLineOffset = 1.0f;
        }

        // Skybox
        {
            TextureCubeFilepath skyboxPath;
            skyboxPath.Name = "Skybox";
            skyboxPath.FrontFilepath = "res/skybox/front.jpg";
            skyboxPath.BackFilepath = "res/skybox/back.jpg";
            skyboxPath.LeftFilepath = "res/skybox/left.jpg";
            skyboxPath.RightFilepath = "res/skybox/right.jpg";
            skyboxPath.TopFilepath = "res/skybox/top.jpg";
            skyboxPath.BottomFilepath = "res/skybox/bottom.jpg";

            Skybox skybox;
            skybox.Geometry = GeometryManager::AddGeometry(Cube());
            skybox.Texture = TextureLoader::LoadCube(skyboxPath, eTextureFormat::RGBA8);
            skybox.Texture->GenerateMips();

            SkyboxManager::Get().Skybox = skybox;
        }

        // Plane
        {
            m_Plane = new Entity("Plane", m_Scene);
            m_Plane->Transform.Position = { 0, 0, 0 };
            m_Plane->Transform.Scale = { 2, 1, 2 };

            m_Plane->Add<GeometryComponent>(GeometryManager::AddGeometry(Plane(10)));
            auto* planeMaterial = m_Plane->Add<MaterialComponent>(MaterialManager::AddMaterial());
            planeMaterial->Metallness = 0.0f;
            planeMaterial->Roughness = 0.05f;
            planeMaterial->AO = 0.0f;
            MaterialManager::Flush(*planeMaterial);

            sPlaneShapeDescriptor planeShapeDesc;
            m_Plane->Add<RigidBodyComponent>(
                PhysicsManager::AddActor(
                        m_Plane,
                        m_Scene,
                        sActor::eActorType::RIGID_STATIC,
                        &planeShapeDesc,
                        glm::vec3(0.0f),
                        0.5f, 0.5f, 0.5f,
                        0.05f, 0.0f
                )
            );
        }

        // Sunlight
        {
            m_SunLight = new Entity("SunLight", m_Scene);
            m_SunLight->Transform.Position = { 20, 20, -20 };

            m_SunLight->Add<GeometryComponent>(GeometryManager::AddGeometry(Sphere()));
            m_SunLight->Add<MaterialComponent>(MaterialManager::AddMaterial());
            m_SunLight->Add<DirectLightComponent>(glm::vec3(0, 0, 0), glm::vec3(1, 1, 10));
        }

        // Goblins
        {
            m_Goblin1 = new Entity("Goblin1", m_Scene);
            m_Goblin1->Transform.Position = {-4, -10, -4 };
            m_Goblin1->Transform.Rotation = {0, 0, 0 };
            m_Goblin1->Transform.Scale = {5, 5, 5 };

            m_Goblin2 = new Entity("Goblin2", m_Scene);
            m_Goblin2->Transform.Position = {-4, -10, 4 };
            m_Goblin2->Transform.Rotation = {0, 0, 0 };
            m_Goblin2->Transform.Scale = {5, 5, 5 };

            m_Goblin3 = new Entity("Goblin3", m_Scene);
            m_Goblin3->Transform.Position = {4, -10, -4 };
            m_Goblin3->Transform.Rotation = {0, 0, 0 };
            m_Goblin3->Transform.Scale = {5, 5, 5 };

            m_Goblin4 = new Entity("Goblin4", m_Scene);
            m_Goblin4->Transform.Position = {4, -10, 4 };
            m_Goblin4->Transform.Rotation = {0, 0, 0 };
            m_Goblin4->Transform.Scale = {5, 5, 5 };

            auto* goblinModel = m_Goblin1->Add<ModelComponent>(
                    ModelLoader::Load("res/models/winter-girl/source/dancing_vampire.dae"));
            goblinModel->Visible = false;
            goblinModel->HasSkeleton = true;
            goblinModel->Entities = { m_Goblin1, m_Goblin2, m_Goblin3, m_Goblin4 };

            m_Goblin1->Add<SkeletonComponent>(SkeletonLoader::Load("res/models/winter-girl/source/dancing_vampire.dae"));
            m_Goblin2->Add<SkeletonComponent>(SkeletonLoader::Load("res/models/winter-girl/source/dancing_vampire.dae"));
            m_Goblin3->Add<SkeletonComponent>(SkeletonLoader::Load("res/models/winter-girl/source/dancing_vampire.dae"));
            m_Goblin4->Add<SkeletonComponent>(SkeletonLoader::Load("res/models/winter-girl/source/dancing_vampire.dae"));

            m_Goblin1->Add<AnimationComponent>(AnimLoader::Load("res/models/winter-girl/source/dancing_vampire.dae"));
            m_Goblin2->Add<AnimationComponent>(AnimLoader::Load("res/models/winter-girl/source/dancing_vampire.dae"));
            m_Goblin3->Add<AnimationComponent>(AnimLoader::Load("res/models/winter-girl/source/dancing_vampire.dae"));
            m_Goblin4->Add<AnimationComponent>(AnimLoader::Load("res/models/winter-girl/source/dancing_vampire.dae"));

            m_Goblin1->Get<AnimationComponent>()->Play = false;
            m_Goblin2->Get<AnimationComponent>()->Play = false;
            m_Goblin3->Get<AnimationComponent>()->Play = false;
            m_Goblin4->Get<AnimationComponent>()->Play = false;

            MaterialFilepath materialFilepath;
            materialFilepath.Name = "niz";
            materialFilepath.AlbedoFilepath = "res/models/winter-girl/textures/Vampire_diffuse.png";
            materialFilepath.BumpFilepath = "res/models/winter-girl/textures/Vampire_normal.png";
//                materialFilepath.RoughnessFilepath = "res/models/winter-girl/textures/Vampire_diffuse.png";
//                materialFilepath.MetallicFilepath = "res/models/winter-girl/textures/Vampire_specular.png";

            auto* material1 = m_Goblin1->Add<MaterialComponent>(MaterialLoader::Load(materialFilepath));
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
            MaterialManager::Flush(*material1);

            auto* material2 = m_Goblin2->Add<MaterialComponent>(MaterialLoader::Load(materialFilepath));
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
            MaterialManager::Flush(*material2);

            auto* material3 = m_Goblin3->Add<MaterialComponent>(MaterialLoader::Load(materialFilepath));
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
            MaterialManager::Flush(*material3);

            auto* material4 = m_Goblin4->Add<MaterialComponent>(MaterialLoader::Load(materialFilepath));
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
            MaterialManager::Flush(*material4);

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

        // Cube
        {
            m_Cube = new Entity("Cube", m_Scene);
            m_Cube->Transform.Position = { 10, 2.5, 10 };
            m_Cube->Transform.Scale = { 5, 5, 5 };
            m_Cube->Add<GeometryComponent>(GeometryManager::AddGeometry(Cube()));
            m_Cube->Add<MaterialComponent>(MaterialManager::AddMaterial());
        }

        // Spheres
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

            m_Glasses[i] = new Entity("Glass-" + string(std::to_string(i)), m_Scene);
            m_Glasses[i]->Transform.Position = { 1 + ((float)i * 0.5f), 1.1 + ((float)i * 2.0f), 0 };
            m_Glasses[i]->Transform.Scale = { 1, 1, 1 };
            m_Glasses[i]->Add<GeometryComponent>(GeometryManager::AddGeometry(Sphere()));
            m_Glasses[i]->Add<MaterialComponent>(MaterialManager::AddMaterial(mat));
            m_Glasses[i]->Get<GeometryComponent>()->Transparent = true;

            sSphereShapeDescriptor sphereShapeDesc(1.0f);
            m_Glasses[i]->Add<RigidBodyComponent>(
                    PhysicsManager::AddActor(
                            m_Glasses[i],
                            m_Scene,
                            sActor::eActorType::RIGID_DYNAMIC,
                            &sphereShapeDesc,
                            glm::vec3(0.0f),
                            0.5f, 0.5f, 0.5f,
                            0.05f, 0.0f
                    )
            );
        }

//        //loading stream audio files
//        {
//            m_AudioObject = new Entity("AudioObject", m_Scene);
//
//            //load test stream audio
//            {
//                m_AudioObject->Add<AudioFileComponent>("Test", m_AudioLoader->Load("res/audio/test.wav"));
//            }
//
//            //load spell stream audio
//            {
//                m_AudioObject->Add<AudioFileComponent>("Magicfail", m_AudioLoader->Load("res/audio/magicfail.ogg"));
//            }
//
//            //load magicfail stream audio
//            {
//                m_AudioObject->Add<AudioFileComponent>("Spell", m_AudioLoader->Load("res/audio/spell.ogg"));
//            }
//
//        }
//        //temporarily commented
//        //// create voice component
//        //{
//        //    VoiceComponent component("Test");
//        //    m_Scene->Add<VoiceComponent>(m_Scene->Audio->GetTag(), component);
//        //}
//
//        //setup background audio
//        {
//            m_BackgroundAudio = new Entity("BackgroundAudio", m_Scene);
//
//            //test sources
//            SourceAudioComponent* source;
//            SourceAudioComponent* source1;
//            SourceAudioComponent* source2;
//
//            //test files //You can use one file for many sources in one time
//            auto* file = m_AudioObject->Get<AudioFileComponent>("Spell");
//            auto* file1 = m_AudioObject->Get<AudioFileComponent>("Test");
//            auto* file2 = m_AudioObject->Get<AudioFileComponent>("Magicfail");
//
//            //creating source
//            {
//                SourceAudioComponent component("Sound_Test");
//
//                component.Position = { -15.0f, 2.0f, 0.0f };
//                component.Looping = true;
//                component.Gain = 0.5f;
//                component.RefDistance = 10.0f;
//                component.MaxDistance = 100.0f;
//                source = m_BackgroundAudio->Add<SourceAudioComponent>(component);
//            }
//
//            //creating stream audio
//            {
//                AudioComponent component("Sound_Test");
//
//                component.Source = source;
//                component.File = file->File;
//
//                m_BackgroundAudio->Add<AudioComponent>(component);
//            }
//
//            //-------------------------------------
//            //creating source
//            {
//                SourceAudioComponent component("Test");
//
//                component.Position = { -5.0f, 2.0f, 0.0f };
//                component.Looping = true; //(todo) Need do some logic to looping the stream audio
//                component.Gain = 0.2f;
//                component.RefDistance = 10.0f;
//                component.MaxDistance = 100.0f;
//                source1 = m_BackgroundAudio->Add<SourceAudioComponent>(component);
//            }
//
//            //creating stream audio
//            {
//                StreamAudioComponent component("Test");
//
//                component.Source = source1;
//                component.File = file1->File;
//                component.BufferSamples = 8192 * 2; //for test
//                component.NumBuffers = 5; //for test
//
//                m_BackgroundAudio->Add<StreamAudioComponent>(component);
//            }
//
//            //-------------------------------------
//            //creating source
//            {
//                SourceAudioComponent component("Sound_Test1");
//
//                component.Position = { 11.0f, 2.0f, 0.0f };
//                component.Looping = true;
//                component.Gain = 0.5f;
//                component.RefDistance = 10.0f;
//                component.MaxDistance = 100.0f;
//                source2 = m_BackgroundAudio->Add<SourceAudioComponent>(component);
//            }
//
//            //creating stream audio
//            {
//                AudioComponent component("Sound_Test1");
//
//                component.Source = source2;
//                component.File = file2->File;
//
//                m_BackgroundAudio->Add<AudioComponent>(component);
//            }
//        }

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

        FontLoader::Free();
        TextureLoader::Free();
        AudioLoader::Free();

        delete m_SunLight;
        delete m_Text2D;
        delete m_Text3D;
        delete m_Plane;
        delete m_Goblin1;
        delete m_Goblin2;
        delete m_Goblin3;
        delete m_Goblin4;
        delete m_Cube;
//        delete m_BackgroundAudio;
//        delete m_AudioObject;
        delete m_Glasses[0];
        delete m_Glasses[1];
        delete m_Glasses[2];
        delete m_Glasses[3];

        RemoveWindowClose();
        RemoveKeyPressed();
        RemoveKeyHold();
        RemoveCursorMove();
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
        auto& camera = *m_PerspectiveCamera;
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
        m_PerspectiveCamera = new PerspectiveCamera(WindowManager::GetWidth(), WindowManager::GetHeight(), m_RenderSystem->GetCameraBuffer());
        m_PerspectiveCamera->Component.Far = m_TestConfig.CameraFar;
        // todo(cheerwizard): BUG - after moving camera, the camera resets position
        m_PerspectiveCamera->Component.Position = { 5, 5, 20 };
        m_PerspectiveCamera->MoveSpeed = m_TestConfig.CameraMoveSpeed;
        m_PerspectiveCamera->ZoomAcceleration = m_TestConfig.CameraZoomAcceleration;
        m_PerspectiveCamera->PanAcceleration = m_TestConfig.CameraPanAcceleration;
        m_PerspectiveCamera->HorizontalSensitivity = m_TestConfig.CameraHorizontalSens;
        m_PerspectiveCamera->VerticalSensitivity = m_TestConfig.CameraVerticalSens;
        m_PerspectiveCamera->Flush();
    }

    void UpdateCamera()
    {
        auto& camera = *m_PerspectiveCamera;
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
            m_Goblin1->Get<AnimationComponent>()->Play = !m_Goblin1->Get<AnimationComponent>()->Play;
            m_Goblin2->Get<AnimationComponent>()->Play = !m_Goblin2->Get<AnimationComponent>()->Play;
            m_Goblin3->Get<AnimationComponent>()->Play = !m_Goblin3->Get<AnimationComponent>()->Play;
            m_Goblin4->Get<AnimationComponent>()->Play = !m_Goblin4->Get<AnimationComponent>()->Play;
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
        auto& text2D = m_Text2D->Get<Text2DComponent>()->Text;
        text2D = "\nFPS: " + std::to_string(CPUTime.Fps()) +
                 "\nCPU: " + std::to_string(CPUTime.Millis()) +
                 "\nGamma: " + std::to_string(WindowManager::GetGamma()) +
                 "\nExposure: " + std::to_string(WindowManager::GetExposure());

        auto& text3D = m_Text3D->Get<Text3DComponent>()->Text;
        text3D = "\nFPS: " + std::to_string(CPUTime.Fps()) +
                 "\nCPU: " + std::to_string(CPUTime.Millis()) +
                 "\nGamma: " + std::to_string(WindowManager::GetGamma()) +
                 "\nExposure: " + std::to_string(WindowManager::GetExposure());
    }

private:
    PerspectiveCamera* m_PerspectiveCamera;

    Entity* m_SunLight;
    Entity* m_Text2D;
    Entity* m_Text3D;
    Entity* m_Plane;
    Entity* m_Goblin1;
    Entity* m_Goblin2;
    Entity* m_Goblin3;
    Entity* m_Goblin4;
    Entity* m_Cube;
    Entity* m_BackgroundAudio;
    Entity* m_AudioObject;
    Entity* m_Glasses[4];

    TestConfig m_TestConfig;
    XmlConfig  m_XmlConfig;
};

Application* CreateApplication() {
    Application* app = new GameApp();

    // read app configs
    if (!xpe::res::ReadJsonFile("config/app_config.json", app->Config))
    {
        FMT_ASSERT(false, "Failed to read app config from config/app_config.json file. Please provide config file!");
        return 0;
    }

    return app;
}