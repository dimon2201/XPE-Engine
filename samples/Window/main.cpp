#include <core/app.hpp>
#include <launcher.h>

#include <rendering/render_system.h>
#include <rendering/skybox_manager.h>
#include <rendering/camera_manager.h>

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

class cExample : public cApp
{
public:
    cExample() {}
    ~cExample() {}

protected:

    void InitRenderPasses() override
    {
        cApp::InitRenderPasses();
        m_CanvasTexture = m_RenderSystem->GetFinalRT()->Colors[0];
    }

public:

    static void HandleButtonPress() {

    }

    static void HandleButtonHover() {

    }

    void Init() override final {
        LogInfo("cExample::Init()");

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

        // save xml configs into file
        if (!m_XmlConfig.SaveFile("config/xml_config_saved.xml"))
        {
            LogError("xml_config_saved.xml file not found in config/xml_config_saved.xml path.");
        }

        AddWindowClosed(cExample, 1);
        AddKeyPressed(cExample, 1);
        AddKeyHold(cExample, 1);
        AddCursorMove(cExample, 1);

        cFontLoader::Init();
        cTextureLoader::Init();
        cAudioLoader::Init();

        InitCamera();

        // Create physics scene and add into current scene
        m_Scene->PhysicsScene = cPhysicsManager::AddScene(m_Scene->GetTag());

        // Widgets
        {
            m_Menu = new cEntity("Menu", m_Scene);
            m_Menu->SetPosition(glm::vec3(0.0, 0.0, 0));
            m_Menu->Add<sCButton>();
            m_Menu->Get<sCButton>()->Color = { 0, 0, 1, 1 };
            m_Menu->Get<sCButton>()->Pressed = HandleButtonPress;
            m_Menu->Get<sCButton>()->Hovered = HandleButtonHover;

            m_Label = new cEntity("Stats", m_Scene);
            m_Label->SetPosition(glm::vec3(0.5, 0.5, 0));
            m_Label->Add<sCLabel>(cFontLoader::Load("res/fonts/Roboto-Bold.ttf", 32), "Test Label", glm::vec4(0, 1, 0, 1));
            m_Label->Get<sCLabel>()->Font->NewLineOffset = 1.0f;

            m_Button = new cEntity("Button", m_Scene);
            m_Button->SetPosition(glm::vec3(0.5, 0.5, 0));
            m_Button->Add<sCButton>();
            m_Button->Get<sCButton>()->Color = { 1, 0, 0, 1 };
            m_Button->Get<sCButton>()->ColorHover = { 0.75, 0, 0, 1 };
            m_Button->Get<sCButton>()->ColorPressed = { 0.5, 0, 0, 1 };

            m_Menu->Children = { m_Label, m_Button };
            m_Menu->SetSpace(eSpace::SPACE_2D);
            m_Menu->SetVisible(false);
            m_Menu->UpdateXmlChildren();

            // save widget xml into file
            if (!m_Menu->SaveFile("config/widget_xml_saved.xml"))
            {
                LogError("widget_xml_saved.xml file not found in config/widget_xml_saved.xml path.");
            }
        }

        // Text 2D
        {
//            m_Text2D = new cEntity("Text2D", m_Scene);
//            m_Text2D->SetPosition(glm::vec3(0, 1, 0));
//            m_Text2D->SetScale(glm::vec3(1));
//
//            auto* textComponent = m_Text2D->Add<sCText2D>();
//            textComponent->Text = "FPS: \n CPU: \n";
//            textComponent->Font->NewLineOffset = 1.0f;
        }

        // Text 3D
        {
//            m_Text3D = new cEntity("Text3D", m_Scene);
//            m_Text3D->SetPosition(glm::vec3(0, 25, 50));
//            m_Text3D->SetScale(glm::vec3(0.25, 0.25, 1));
//
//            auto* textComponent = m_Text3D->Add<sCText3D>();
//            textComponent->Text = "Hi,\nWelcome to Example Window\nThis is a testing version of XPE-Engine";
//            textComponent->Font = cFontLoader::Load("res/fonts/Roboto-Bold.ttf", 32);
//            textComponent->Font->NewLineOffset = 1.0f;
        }

        // sSkybox
        {
            sTextureCubeFilepath skyboxPath;
            skyboxPath.Name = "sSkybox";
            skyboxPath.FrontFilepath = "res/skybox/front.jpg";
            skyboxPath.BackFilepath = "res/skybox/back.jpg";
            skyboxPath.LeftFilepath = "res/skybox/left.jpg";
            skyboxPath.RightFilepath = "res/skybox/right.jpg";
            skyboxPath.TopFilepath = "res/skybox/top.jpg";
            skyboxPath.BottomFilepath = "res/skybox/bottom.jpg";

            sSkybox skybox;
            skybox.Geometry = cGeometryManager::AddGeometry(sCube());
            skybox.Texture = cTextureLoader::LoadCube(skyboxPath, eTextureFormat::RGBA8);
            skybox.Texture->GenerateMips();

            cSkyboxManager::Get().Skybox = skybox;
        }

        // Plane
        {
            m_Plane = new cEntity("sPlane", m_Scene);
            m_Plane->SetPosition(glm::vec3(0.0f));
            m_Plane->SetScale(glm::vec3(5.0f, 1.0f, 5.0f));
            m_Plane->Add<sCGeometry>(cGeometryManager::AddGeometry(sPlane(10)))->CastShadow = true;
            m_Plane->Add<sCMaterial>(cMaterialManager::AddMaterial());

            sPlaneShapeDescriptor planeShapeDesc;
            m_Plane->Add<sCPhysicsActor>(
                    cPhysicsManager::AddActor(
                        m_Plane,
                        sCPhysicsActor::eActorType::RIGID_STATIC,
                        &planeShapeDesc,
                        glm::vec3(0.0f),
                        0.5f, 0.5f, 0.5f,
                        0.05f, 0.0f
                )
            );
        }

        // Sunlight
        {
            m_SunLight = new cEntity("SunLight", m_Scene);
            m_SunLight->SetPosition(glm::vec3(-2.0f, 4.0f, -1.0f));
            m_SunLight->Add<sCGeometry>(cGeometryManager::AddGeometry(sSphere()))->CastShadow = false;
            m_SunLight->Add<sCMaterial>(cMaterialManager::AddMaterial());
            m_SunLight->Add<sCDirectionalLight>(glm::vec3(-2, 4, -1), glm::vec3(1, 1, 10));
        }

        // Goblins
        {
            m_Goblins = new cEntity("Goblins", m_Scene);

            m_Goblin1 = new cEntity("Goblin1", m_Scene);
            m_Goblin1->GetPosition() = { -4, 0, -4 };
            m_Goblin1->GetRotation() = { 0, 0, 0 };
            m_Goblin1->GetScale() = { 5, 5, 5 };
            m_Goblin1->SetVisible(false);

            m_Goblin2 = new cEntity("Goblin2", m_Scene);
            m_Goblin2->GetPosition() = { -4, 0, 4 };
            m_Goblin2->GetRotation() = { 0, 0, 0 };
            m_Goblin2->GetScale() = { 5, 5, 5 };
            m_Goblin2->SetVisible(false);

            m_Goblin3 = new cEntity("Goblin3", m_Scene);
            m_Goblin3->GetPosition() = { 4, 0, -4 };
            m_Goblin3->GetRotation() = { 0, 0, 0 };
            m_Goblin3->GetScale() = { 5, 5, 5 };
            m_Goblin3->SetVisible(false);

            m_Goblin4 = new cEntity("Goblin4", m_Scene);
            m_Goblin4->GetPosition() = { 4, 0, 4 };
            m_Goblin4->GetRotation() = { 0, 0, 0 };
            m_Goblin4->GetScale() = { 5, 5, 5 };

            auto* goblins = m_Goblins->Add<sCSkeletonModel>(
                cGeometryManager::AddGeometry(
                        cModelLoader::Load("res/models/winter-girl/source/dancing_vampire.dae").Bake()
                )
            );
            goblins->Entities = { m_Goblin1, m_Goblin2, m_Goblin3, m_Goblin4 };
            goblins->Skeleton = cSkeletonLoader::Load("res/models/winter-girl/source/dancing_vampire.dae");
            goblins->Animations.emplace_back(cAnimLoader::Load("res/models/winter-girl/source/dancing_vampire.dae"));
            goblins->Animations[0].Play = true;

            sMaterialFilepath materialFilepath;
            materialFilepath.Name = "niz";
            materialFilepath.AlbedoFilepath = "res/models/winter-girl/textures/Vampire_diffuse.png";
            materialFilepath.BumpFilepath = "res/models/winter-girl/textures/Vampire_normal.png";
            materialFilepath.RoughnessFilepath = "res/models/winter-girl/textures/Vampire_diffuse.png";
            materialFilepath.MetallicFilepath = "res/models/winter-girl/textures/Vampire_specular.png";

            auto* material1 = m_Goblin1->Add<sCMaterial>(cMaterialManager::AddMaterial());
            material1->Albedo = { 1, 1, 1, 1 };
            material1->Emission = { 0, 0, 10 };
            material1->Metallness = 0.0f;
            material1->Roughness = 0.05f;
            material1->AO = 0.0f;
            material1->EnableAlbedoMap = false;
            material1->EnableNormalMap = false;
            material1->EnableRoughnessMap = false;
            material1->EnableMetalMap = false;
            material1->EnableAOMap = false;
            cMaterialManager::Flush(*material1);

            auto* material2 = m_Goblin2->Add<sCMaterial>(cMaterialManager::AddMaterial());
            material2->Albedo = { 1, 1, 1, 1 };
            material2->Emission = { 0, 0, 10 };
            material2->Metallness = 0.0f;
            material2->Roughness = 0.05f;
            material2->AO = 0.0f;
            material2->EnableAlbedoMap = false;
            material2->EnableNormalMap = false;
            material2->EnableRoughnessMap = false;
            material2->EnableMetalMap = false;
            material2->EnableAOMap = false;
            cMaterialManager::Flush(*material2);

            auto* material3 = m_Goblin3->Add<sCMaterial>(cMaterialManager::AddMaterial());
            material3->Albedo = { 1, 1, 1, 1 };
            material3->Emission = { 0, 0, 10 };
            material3->Metallness = 0.0f;
            material3->Roughness = 0.05f;
            material3->AO = 0.0f;
            material3->EnableAlbedoMap = false;
            material3->EnableNormalMap = false;
            material3->EnableRoughnessMap = false;
            material3->EnableMetalMap = false;
            material3->EnableAOMap = false;
            cMaterialManager::Flush(*material3);

            auto* material4 = m_Goblin4->Add<sCMaterial>(cMaterialManager::AddMaterial());
            material4->Albedo = { 1, 1, 1, 1 };
            material4->Emission = { 0, 0, 10 };
            material4->Metallness = 0.0f;
            material4->Roughness = 0.05f;
            material4->AO = 0.0f;
            material4->EnableAlbedoMap = false;
            material4->EnableNormalMap = false;
            material4->EnableRoughnessMap = false;
            material4->EnableMetalMap = false;
            material4->EnableAOMap = false;
            cMaterialManager::Flush(*material4);

            // for testing image resizing on CPU, we can write resized image into file

            cTextureLoader::SaveLayer(
                    "generated/Vampire_diffuse_resized.png",
                    material1->AlbedoMap,
                    eFileFormat::PNG
            );

            cTextureLoader::SaveLayer(
                    "generated/Vampire_normal_resized.png",
                    material1->NormalMap,
                    eFileFormat::PNG
            );

            cTextureLoader::SaveLayer(
                    "generated/Vampire_roughness_resized.png",
                    material1->RoughnessMap,
                    eFileFormat::PNG
            );

            cTextureLoader::SaveLayer(
                    "generated/Vampire_metallic_resized.png",
                    material1->MetalMap,
                    eFileFormat::PNG
            );

            int w, h, c;
            sTextureLayer* layer = cTextureLoader::LoadLayer("res/skybox/back.jpg", eTextureFormat::RGBA8, w, h, c);
            sAtlas atlas;
            atlas.Width = 1024;
            atlas.Height = 1024;
            atlas.Channels = c;
            atlas.Format = eTextureFormat::RGBA8;
            atlas.AddLayer();
            atlas.AddCell(0, glm::vec2(0, 0), glm::vec2(32, 32), layer);
            atlas.AddCell(0, glm::vec2(32, 0), glm::vec2(64, 64), layer);
            atlas.AddCell(0, glm::vec2(32 + 64, 0), glm::vec2(128, 128), layer);
            atlas.AddCell(0, glm::vec2(32 + 64 + 128, 0), glm::vec2(256, 256), layer);
            atlas.Init();
            cTextureLoader::SaveLayer(
                "generated/test_atlas.png",
                &atlas.Layers[0],
                eFileFormat::PNG
            );
        }

        // Cube
        {
            m_Cube = new cEntity("Cube", m_Scene);
            m_Cube->SetPosition(glm::vec3(10.0f, 2.5f, 10.0f));
            m_Cube->SetScale(glm::vec3(5.0f));
            m_Cube->Add<sCGeometry>(cGeometryManager::AddGeometry(sCube()));
            m_Cube->Add<sCMaterial>(cMaterialManager::AddMaterial());
        }

        // Spheres
        for (s32 i = 0; i < 4; i++)
        {
            auto mat = sMaterial();
            if (i == 0) { mat.Albedo = glm::vec4(1.0f, 0.0f, 0.0f, 0.2f); }
            if (i == 1) { mat.Albedo = glm::vec4(0.0f, 1.0f, 0.0f, 0.4f); }
            if (i == 2) { mat.Albedo = glm::vec4(0.0f, 0.0f, 1.0f, 0.6f); }
            if (i == 3) { mat.Albedo = glm::vec4(1.0f, 1.0f, 0.0f, 0.8f); }

            mat.Metallness = 0.0f;
            mat.Roughness = 0.05f;
            mat.AO = 0.0f;

            m_Glasses[i] = new cEntity("Glass-" + string(std::to_string(i)), m_Scene);
            m_Glasses[i]->SetPosition(glm::vec3(0 + ((float)i * 0.5f), 1.1 + ((float)i * 2.0f), -5));
            m_Glasses[i]->SetScale(glm::vec3(1.0f));
            m_Glasses[i]->Add<sCGeometry>(cGeometryManager::AddGeometry(sSphere()));
            m_Glasses[i]->Add<sCMaterial>(cMaterialManager::AddMaterial(mat));
            m_Glasses[i]->Get<sCGeometry>()->Transparent = true;

            sSphereShapeDescriptor sphereShapeDesc(1.0f);
            m_Glasses[i]->Add<sCPhysicsActor>(
                    cPhysicsManager::AddActor(
                            m_Glasses[i],
                            sCPhysicsActor::eActorType::RIGID_DYNAMIC,
                            &sphereShapeDesc,
                            glm::vec3(0.0f),
                            0.5f, 0.5f, 0.5f,
                            0.05f, 0.0f
                    )
            );
        }

        // settings for shadows
        cShadowManager::GetData().FilterSize = 0;
        cShadowManager::Flush();

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

        m_SsaoPass->GetData().Intensity = 2;
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
        LogInfo("cExample::Free()");

        cFontLoader::Free();
        cTextureLoader::Free();
        cAudioLoader::Free();

        delete m_SunLight;
        delete m_Plane;
        delete m_Goblins;
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

        delete m_Menu;
        delete m_Button;
        delete m_Label;

        RemoveWindowClose();
        RemoveKeyPressed();
        RemoveKeyHold();
        RemoveCursorMove();
    }

    void WindowClosed()
    {
        LogWarning("cExample::WindowClosed()");
    }

    void KeyPressed(const eKey key)
    {
        if (key == eKey::Esc)
        {
            cWindowManager::Close();
        }

        MoveLight(key);
        PlayAnimations(key);

        if (key == eKey::R)
        {
            m_CanvasTexture = m_RenderSystem->GetShadowRT()->Colors[0];
        }

        if (key == eKey::T)
        {
            m_CanvasTexture = m_RenderSystem->GetFinalRT()->Colors[0];
        }

        MoveWidget(key);
    }

    void KeyHold(const eKey key)
    {
        MoveLight(key);
        PlayAnimations(key);
        MoveWidget(key);
    }

    void CursorMoved(const double x, const double y)
    {
        auto& camera = *m_PerspectiveCamera;
        if (cInputManager::MousePressed(eMouse::ButtonRight)) {
            camera.EnableLook = false;
            cInputManager::CaptureCursor(x, y);
            auto& cursorDelta = cInputManager::GetMouseCursor().Delta;
            camera.Pan(cursorDelta);
        } else {
            camera.EnableLook = true;
        }
    }

private:

    void InitCamera() {
        m_PerspectiveCamera = cCameraManager::AddPerspectiveCamera(cWindowManager::GetWidth(), cWindowManager::GetHeight());
        m_PerspectiveCamera->Component.Far = m_TestConfig.CameraFar;
        m_PerspectiveCamera->Component.Position = { 5, 5, 20 };
        m_PerspectiveCamera->Component.AspectRatio = Config.AspectRatio;
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
        if (cInputManager::MousePressed(eMouse::ButtonLeft)) {
            camera.LookMode = cCamera::eLookMode::EDITOR;
        } else {
            camera.LookMode = cCamera::eLookMode::GAME;
        }
        camera.Move();
    }

    void MoveLight(const eKey key)
    {
        auto& pos = m_SunLight->GetPosition();

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

        if (key == eKey::Z)
        {
            pos.z += 1;
        }

        if (key == eKey::X)
        {
            pos.z -= 1;
        }
    }

    void MoveWidget(const eKey key)
    {
        if (key == eKey::Up)
        {
            m_Menu->Move({0, 0.1, 0});
        }

        if (key == eKey::Down)
        {
            m_Menu->Move({0, -0.1, 0});
        }

        if (key == eKey::Left)
        {
            m_Menu->Move({-0.1, 0, 0});
        }

        if (key == eKey::Right)
        {
            m_Menu->Move({0.1, 0, 0});
        }
    }

    void PlayAnimations(const eKey key)
    {
        if (key == eKey::P)
        {
            m_Goblins->Get<sCSkeletonModel>()->Animations[0].Play = !m_Goblins->Get<sCSkeletonModel>()->Animations[0].Play;
        }
    }

    void AnimateLight()
    {
        if (m_TestConfig.AnimateLight)
        {
            auto& pos = m_SunLight->GetPosition();

            // translation light up and down every N ticks
            static int tick = 1;

            pos.x = 100 * sin(tick / 3000.0f);
            pos.z = 100 * sin(tick / 3000.0f);

            tick++;
        }
    }

    void DisplayStats() {
        auto& text = m_Label->Get<sCLabel>()->Text;
        text = "\nFPS: " + std::to_string(CPUTime.Fps()) +
                 "\nCPU: " + std::to_string(CPUTime.Millis()) +
                 "\nGamma: " + std::to_string(cCameraManager::GetGamma()) +
                 "\nExposure: " + std::to_string(cCameraManager::GetExposure());
    }

private:
    cPerspectiveCamera* m_PerspectiveCamera;

    cEntity* m_SunLight;
    cEntity* m_Plane;
    cEntity* m_Goblins;
    cEntity* m_Goblin1;
    cEntity* m_Goblin2;
    cEntity* m_Goblin3;
    cEntity* m_Goblin4;
    cEntity* m_Cube;
    cEntity* m_BackgroundAudio;
    cEntity* m_AudioObject;
    cEntity* m_Glasses[4];

    sTestConfig m_TestConfig;
    sXmlConfig  m_XmlConfig;

    cEntity* m_Menu;
    cEntity* m_Button;
    cEntity* m_Label;
};

cApp* CreateApplication() {
    cApp* app = new cExample();

    // read app configs
    if (!xpe::res::ReadJsonFile("config/app_config.json", app->Config))
    {
        FMT_ASSERT(false, "Failed to read app config from config/app_config.json file. Please provide config file!");
        return 0;
    }

    return app;
}