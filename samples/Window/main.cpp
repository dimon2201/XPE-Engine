#include <core/app.hpp>
#include <launcher.hpp>

#include <rendering/render_system.hpp>
#include <rendering/skybox_manager.hpp>
#include <rendering/camera_manager.hpp>
#include <rendering/light_manager.hpp>
#include <rendering/canvas.hpp>

#include <audio/audio_manager.hpp>

#include <ecs/components.hpp>

#include <model_loader.hpp>
#include <material_loader.hpp>
#include <font_loader.hpp>
#include <texture_loader.hpp>
#include <skeleton_loader.hpp>
#include <anim_loader.hpp>
#include <audio_loader.hpp>

#include <particle/particle_manager.hpp>
#include <core/texture_manager.hpp>

#include "test_config.hpp"

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

    void InitShaders() override
    {
        cApp::InitShaders();
        Textures::Canvas = RenderTargets::Final->GetColors()[0];
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
            m_Menu = cEntity("Menu", m_Scene);
            m_Menu.SetPosition(0, 0, 0);
            m_Menu.Add<CButton>();
            m_Menu.Get<CButton>().Color = { 0, 0, 1, 1 };
            m_Menu.Get<CButton>().Pressed = HandleButtonPress;
            m_Menu.Get<CButton>().Hovered = HandleButtonHover;
            m_Menu.SetSpace2D();

            m_Label = cEntity("Stats", m_Scene);
            m_Label.SetPosition(0.5, 0.5, 0);
            m_Label.Add<CLabel>(cFontLoader::Load("res/fonts/Roboto-Bold.ttf", 32), "Test Label", glm::vec4(0, 1, 0, 1));
            m_Label.Get<CLabel>().Font->NewLineOffset = 1.0f;
            m_Label.SetSpace2D();

            m_Button = cEntity("Button", m_Scene);
            m_Button.SetPosition(0.5, 0.5, 0);
            m_Button.Add<CButton>();
            m_Button.Get<CButton>().Color = { 1, 0, 0, 1 };
            m_Button.Get<CButton>().ColorHover = { 0.75, 0, 0, 1 };
            m_Button.Get<CButton>().ColorPressed = { 0.5, 0, 0, 1 };
            m_Button.SetSpace2D();

            m_Menu.Children = { &m_Label, &m_Button };
            m_Menu.SetSpace2D();
            m_Menu.SetVisible(false);
            m_Menu.UpdateXmlChildren();

            // save widget xml into file
            if (!m_Menu.SaveFile("config/widget_xml_saved.xml"))
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
            std::tie(skybox.Geometry, skybox.GeometryInfo) = cGeometryManager::AddGeometry(sCube());
            skybox.Texture = cTextureLoader::LoadCube(skyboxPath, eTextureFormat::RGBA8);
            skybox.Texture->GenerateMips();

            cSkyboxManager::Get().Skybox = skybox;
        }

        sAtlas2DTexture diffuseTexture = MTexture::LoadTexture("C:/Users/USER100/CLionProjects/XPE-Engine/out/build/x64-Debug/bin/Debug/res/textures/lowres.png");

        // Capsule
        {
            m_Capsule = cEntity("Cube", m_Scene);
            m_Capsule.SetPosition(glm::vec3(7.0f, 5.0f, 5.0f));
            m_Capsule.SetScale(glm::vec3(1.0f));
            m_Capsule.Add<CGeometryInfo>(cGeometryManager::AddGeometry(sCapsule()).second);
            m_Capsule.Get<CGeometryInfo>().Entities = { m_Capsule.GetID() };
            m_Capsule.SetOpaque(true);
            m_Capsule.SetTransparent(false);

            sCapsuleShapeDescriptor capsuleShapeDesc(0.5f, 0.5f);
            sActor* physicsActor = cPhysicsManager::AddActor(
                m_Capsule,
                CPhysicsActor::eActorType::RIGID_DYNAMIC,
                &capsuleShapeDesc,
                glm::vec3(0.0f),
                0.5f, 0.5f, 0.5f,
                0.05f, 0.0f,
                1.0f
            );

            m_Capsule.Add<CPhysicsActor>(physicsActor);
        }

        // Sunlight
        {
            auto object = cEntity("SunLight", m_Scene);
            object.SetPosition(0.0f, 2.0f, 0.0f);
            object.SetScale(1.3f, 1.3f, 1.3f);
            object.Add<CGeometry>(sSphere());
            object.Add<CGeometryInfo>(cGeometryManager::AddGeometry(sSphere()).second);
            object.Get<CGeometryInfo>().Entities = { object.GetID() };
            object.SetOpaque(true);

            m_SunLight = cEntity("SunLight", m_Scene);
            m_SunLight.SetPosition(1.0, 5.0, 0.0);
            m_SunLight.Add<CDirectionalLight>(0, glm::vec3(-1.0f, -1.0f, 0.0f), glm::vec3(1.0f));
            m_SunLight.Add<CShadowCaster>(
                vector<EntityID>{ object.GetID(), m_Capsule.GetID() },
                cLightManager::AddShadowCaster()
            );

            m_Light2 = cEntity("Light2", m_Scene);
            m_Light2.SetPosition(1.0, 7.0, 5.0);
            m_Light2.Add<CDirectionalLight>(1, glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(1.0f));
            m_Light2.Add<CShadowCaster>(
                vector<EntityID>{ object.GetID(), m_Capsule.GetID() },
                cLightManager::AddShadowCaster()
            );
        }

        // Plane
        {
            m_Plane = cEntity("sPlane", m_Scene);
            m_Plane.SetPosition(0, 0, 0);
            m_Plane.SetScale(5.0, 1.0, 5.0);
            m_Plane.Add<CMaterial2>(MMaterial::AddMaterial(diffuseTexture));
            m_Plane.Add<CGeometryInfo>(cGeometryManager::AddGeometry(sPlane(10)).second);
            m_Plane.Get<CGeometryInfo>().Entities = { m_Plane.GetID() };
            m_Plane.SetOpaque(true);
            m_Plane.SetTransparent(false);

            sPlaneShapeDescriptor planeShapeDesc;
            m_Plane.Add<CPhysicsActor>(
                cPhysicsManager::AddActor(
                    m_Plane,
                    CPhysicsActor::eActorType::RIGID_STATIC,
                    &planeShapeDesc,
                    glm::vec3(0.0f),
                    0.5f, 0.5f, 0.5f,
                    0.05f, 0.0f,
                    10.0f
                )
            );

            m_Plane.Add<CShadowReceiver>(
                vector<CShadowCaster> {
                    m_SunLight.Get<CShadowCaster>(),
                    m_Light2.Get<CShadowCaster>()
                }
            );
        }

        // Spheres
        for (s32 i = 0; i < 0; i++)
        {
            m_Glasses[i] = cEntity("Glass-" + string(std::to_string(i)), m_Scene);
            m_Glasses[i].SetPosition(glm::vec3(0, 1.0f + ((float)i * 3.0f), 95));
            m_Glasses[i].SetScale(glm::vec3(1.0f));
            m_Glasses[i].Add<CGeometryInfo>(cGeometryManager::AddGeometry(sSphere()).second);
            m_Glasses[i].Add<CTransparent>();

            sSphereShapeDescriptor sphereShapeDesc(1.0f);
            m_Glasses[i].Add<CPhysicsActor>(
                cPhysicsManager::AddActor(
                    m_Glasses[i],
                    CPhysicsActor::eActorType::RIGID_DYNAMIC,
                    &sphereShapeDesc,
                    glm::vec3(0.0f),
                    0.5f, 0.5f, 0.5f,
                    0.05f, 0.0f,
                    10.0f
                )
            );
        }

        //m_Ragdoll = cPhysicsManager::AddRagdoll(m_Scene, glm::vec3(0, 20, 25));

        // Goblins
        {
            m_Goblins = cEntity("Goblins", m_Scene);

            m_Goblin1 = cEntity("Goblin1", m_Scene);
            m_Goblin1.SetPosition(-4, 0, -4);
            m_Goblin1.SetRotation(0, 0, 0);
            m_Goblin1.SetScale(5, 5, 5);
            m_Goblin1.SetVisible(true);

            m_Goblin2 = cEntity("Goblin2", m_Scene);
            m_Goblin2.SetPosition(-4, 0, 4);
            m_Goblin2.SetRotation(0, 0, 0);
            m_Goblin2.SetScale(5, 5, 5);
            m_Goblin2.SetVisible(false);

            m_Goblin3 = cEntity("Goblin3", m_Scene);
            m_Goblin3.SetPosition(4, 0, -4);
            m_Goblin3.SetRotation(0, 0, 0);
            m_Goblin3.SetScale(5, 5, 5);
            m_Goblin3.SetVisible(false);

            m_Goblin4 = cEntity("Goblin4", m_Scene);
            m_Goblin4.SetPosition(4, 0, 4);
            m_Goblin4.SetRotation(0, 0, 0);
            m_Goblin4.SetScale(5, 5, 5);
            m_Goblin4.SetVisible(false);

            auto [goblinGeometry, goblinGeometryInfo] = cGeometryManager::AddGeometry(cModelLoader::Load("res/models/winter-girl/source/skeleton.dae").Merge());
            auto [goblinSkeleton, goblinSkeletonInfo] = cSkeletonLoader::Load("res/models/winter-girl/source/skeleton.dae");
            goblinSkeletonInfo.GeometryInfo = goblinGeometryInfo;
            m_Goblins.Add<CSkeleton>(goblinSkeleton);
            m_Goblins.Add<CSkeletonInfo>(goblinSkeletonInfo);
            m_Goblins.Get<CSkeletonInfo>().GeometryInfo.Entities = {
                    m_Goblin1.GetID(),
                    m_Goblin2.GetID(),
                    m_Goblin3.GetID(),
                    m_Goblin4.GetID()
            };
            m_Goblins.Add<CAnimation>();
            m_Goblins.Get<CAnimation>().Animations.emplace_back(cAnimLoader::Load("res/models/winter-girl/source/skeleton.dae"));
            m_Goblins.Get<CAnimation>().Animations[0].Play = true;
            m_Goblins.SetOpaque(true);
            m_Goblins.SetTransparent(false);

            sMaterialFilepath materialFilepath;
            materialFilepath.Name = "niz";
            materialFilepath.AlbedoFilepath = "res/models/winter-girl/textures/Vampire_diffuse.png";
            materialFilepath.BumpFilepath = "res/models/winter-girl/textures/Vampire_normal.png";
            materialFilepath.RoughnessFilepath = "res/models/winter-girl/textures/Vampire_diffuse.png";
            materialFilepath.MetallicFilepath = "res/models/winter-girl/textures/Vampire_specular.png";
        }

        // settings for shadows
        Buffers::ShadowPCF->GetItem().FilterSize = 0;
        Buffers::ShadowPCF->Flush();

        // Texture atlas test
        sAtlas2DTexture texture = MTexture::LoadTexture(
            "C:/Users/USER100/CLionProjects/XPE-Engine/samples/res/textures/texture.png"
        );

        {
            m_AudioBox = cEntity("AudioBox", m_Scene);

            m_AudioBox.SetPosition(glm::vec3(-5.0f, 5.0f, 10.0f));
            m_AudioBox.SetScale(glm::vec3(1.0f));
            m_AudioBox.Add<CGeometry>(sCube());
            m_AudioBox.Add<CGeometryInfo>(cGeometryManager::AddGeometry(sCube()).second);
            m_AudioBox.Get<CGeometryInfo>().Entities = { m_AudioBox.GetID() };
            m_AudioBox.SetOpaque(true);

            auto& test1 = m_AudioBox.Add<CStreamAudio>(CStreamAudio());
            test1.File = cAudioLoader::Load("res/audio/mono_test.wav");
        }

        {
            for (s32 i = 0; i < 4; i++)
            {
                m_ParticleEmitters[i] = cEntity("ParticleEmitter", m_Scene);

                m_ParticleEmitters[i].SetPosition(glm::vec3(15.0f + (2.5f * (float)i), 1.0f, 15.0f));
                m_ParticleEmitters[i].SetScale(glm::vec3(1.0f));
                m_ParticleEmitters[i].Add<CParticleEmitter>(MParticle::AddEmitter(8));
                MParticle::UpdateEmitter(m_ParticleEmitters[i], m_Scene);
            }
        }

        Buffers::SSAO->GetItem().Intensity = 2;
        Buffers::SSAO->Flush();

        MMaterial::Update(m_Scene);
    }

    void Update() override final
    {
        LockFPSFromConfig();
        UpdateCamera();
        UpdateListener();
        AnimateLight();
        DisplayStats();
        cLightManager::UpdateLights(m_Scene);
    }

    void Free()
    {
        LogInfo("cExample::Free()");

        cFontLoader::Free();
        cTextureLoader::Free();
        cAudioLoader::Free();

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
            Textures::Canvas = RenderTargets::Shadow->GetColors()[0];
        }

        if (key == eKey::T)
        {
            Textures::Canvas = RenderTargets::Final->GetColors()[0];
        }

        if (key == eKey::V)
        {
            cAudioManager::VoiceRecord();
        }

        if (key == eKey::J)
        {
            m_Scene->GetComponent<CTransform>(m_SunLight.GetID()).Position.x -= 0.1f;

            //cPhysicsManager::SetForce(m_Ragdoll->Bodyparts[0], glm::vec3(256.0f, 0.0f, 0.0f));
            //cPhysicsManager::SetForce(m_Ragdoll->Bodyparts[1], glm::vec3(256.0f, 0.0f, 256.0f));
            //cPhysicsManager::SetForce(m_Ragdoll->Bodyparts[2], glm::vec3(256.0f, 256.0f, 0.0f));
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
        m_PerspectiveCamera = new cPerspectiveCamera(cWindowManager::GetWidth(), cWindowManager::GetHeight());
        m_PerspectiveCamera->Component.Near = 0.001f;
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

    void UpdateListener() {
        auto& camera = *m_PerspectiveCamera;

        cAudioManager::SetListenerPosition(camera.Component.Position);
        cAudioManager::SetListenerOrientation(camera.Pitch, camera.Yaw, camera.Roll, camera.Component.Up);
    }

    void MoveLight(const eKey key)
    {
        auto& pos = m_SunLight.GetPosition();

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
            m_Menu.Move(0, 0.1, 0);
        }

        if (key == eKey::Down)
        {
            m_Menu.Move(0, -0.1, 0);
        }

        if (key == eKey::Left)
        {
            m_Menu.Move(-0.1, 0, 0);
        }

        if (key == eKey::Right)
        {
            m_Menu.Move(0.1, 0, 0);
        }
    }

    void PlayAnimations(const eKey key)
    {
        if (key == eKey::P)
        {
            m_Goblins.Get<CAnimation>().Animations[0].Play = !m_Goblins.Get<CAnimation>().Animations[0].Play;
        }
    }

    void AnimateLight()
    {
        if (m_TestConfig.AnimateLight)
        {
            auto& pos = m_SunLight.GetPosition();

            // translation light up and down every N ticks
            static int tick = 1;

            pos.x = 100 * sin(tick / 3000.0f);
            pos.z = 100 * sin(tick / 3000.0f);

            tick++;
        }
    }

    void DisplayStats() {
        auto& text = m_Label.Get<CLabel>().Text;
        text = "\nFPS: " + std::to_string(CPUTime.Fps()) +
                 "\nCPU: " + std::to_string(CPUTime.Millis()) +
                 "\nGamma: " + std::to_string(cCameraManager::GetGamma()) +
                 "\nExposure: " + std::to_string(cCameraManager::GetExposure());
    }

private:
    cPerspectiveCamera* m_PerspectiveCamera;

    cEntity m_SunLight;
    cEntity m_Light2;
    cEntity m_Plane;
    cEntity m_Goblins;
    cEntity m_Goblin1;
    cEntity m_Goblin2;
    cEntity m_Goblin3;
    cEntity m_Goblin4;
    cEntity m_Capsule;
    cEntity m_AudioBox;
    cEntity m_Glasses[4];
    cEntity m_ParticleEmitters[4];
    sRagdoll* m_Ragdoll;

    sTestConfig m_TestConfig;
    sXmlConfig  m_XmlConfig;

    cEntity m_Menu;
    cEntity m_Button;
    cEntity m_Label;
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