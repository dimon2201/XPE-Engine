#include <launcher.hpp>
#include <test_config.hpp>

class cExample : public cApp
{
public:
    cExample() : cApp("Example") {}
    ~cExample() {}

protected:
    void InitShaders() override
    {
        cApp::InitShaders();
    }

public:
    static void HandleButtonPress()
    {

    }

    static void HandleButtonHover()
    {

    }

    void Init() override final
    {
        LogInfo("cExample::Init()");

        // read test configs from file
        if (!xpe::res::LoadJSON("config/test_config.json", m_TestConfig))
        {
            LogError("test_config.json file not found in config/test_config.json path.");
        }

        MEvent::WindowClosed.Add("Example", 1, [this]()
        {
            WindowClosed();
        });
        MEvent::KeyPressed.Add("Example", 1, [this](const eKey key)
        {
            KeyPressed(key);
        });
        MEvent::KeyHold.Add("Example", 1, [this](const eKey key)
        {
            KeyHold(key);
        });
        MEvent::CursorMoved.Add("Example", 1, [this](const double x, const double y)
        {
            CursorMoved(x, y);
        });

        InitCamera();

//        // Widgets
//        {
//            m_Menu = cEntity("Menu", m_Scene);
//            m_Menu.Get<CTransform>().Position = { 0, 0, 0 };
//            m_Menu.Add<CButton>();
//            m_Menu.Get<CButton>().Color = { 0, 0, 1, 1 };
//            m_Menu.Get<CButton>().Pressed = HandleButtonPress;
//            m_Menu.Get<CButton>().Hovered = HandleButtonHover;
//            m_Menu.Add<CSpace2D>();
//
//            m_Label = cEntity("Stats", m_Scene);
//            m_Label.Get<CTransform>().Position = { 0.5, 0.5, 0 };
//            m_Label.Add<CLabel>();
//            m_Label.Get<CLabel>().Text = "Test Label";
//            m_Label.Get<CLabel>().Color = glm::vec4(0, 1, 0, 1);
//            sFont* fontRoboto = MFont::Load("res/fonts/Roboto-Bold.ttf", 32);
//            if (fontRoboto != nullptr)
//            {
//                m_Label.Get<CLabel>().Font = fontRoboto;
//                m_Label.Get<CLabel>().Font->NewLineOffset = 1.0f;
//            }
//            m_Label.Add<CSpace2D>();
//
//            m_Button = cEntity("Button", m_Scene);
//            m_Button.Get<CTransform>().Position = { 0.5, 0.5, 0 };
//            m_Button.Add<CButton>();
//            m_Button.Get<CButton>().Color = { 1, 0, 0, 1 };
//            m_Button.Get<CButton>().ColorHover = { 0.75, 0, 0, 1 };
//            m_Button.Get<CButton>().ColorPressed = { 0.5, 0, 0, 1 };
//            m_Button.Add<CSpace2D>();
//
//            m_Menu.Add<CSpace2D>();
//            m_Menu.Remove<CVisible>();
//        }
//
//        // Text 2D
//        {
////            m_Text2D = new cEntity("Text2D", m_Scene);
////            m_Text2D->SetPosition(glm::vec3(0, 1, 0));
////            m_Text2D->SetScale(glm::vec3(1));
////
////            auto* textComponent = m_Text2D->Add<sCText2D>();
////            textComponent->Text = "FPS: \n CPU: \n";
////            textComponent->Font->NewLineOffset = 1.0f;
//        }
//
//        // Text 3D
//        {
////            m_Text3D = new cEntity("Text3D", m_Scene);
////            m_Text3D->SetPosition(glm::vec3(0, 25, 50));
////            m_Text3D->SetScale(glm::vec3(0.25, 0.25, 1));
////
////            auto* textComponent = m_Text3D->Add<sCText3D>();
////            textComponent->Text = "Hi,\nWelcome to Example Window\nThis is a testing version of XPE-Engine";
////            textComponent->Font = cFontLoader::Load("res/fonts/Roboto-Bold.ttf", 32);
////            textComponent->Font->NewLineOffset = 1.0f;
//        }
//
        // Skybox
        {
            sTextureCubeFilepath skyboxPath;
            skyboxPath.FrontFilepath = "res/skybox/front.jpg";
            skyboxPath.BackFilepath = "res/skybox/back.jpg";
            skyboxPath.LeftFilepath = "res/skybox/left.jpg";
            skyboxPath.RightFilepath = "res/skybox/right.jpg";
            skyboxPath.TopFilepath = "res/skybox/top.jpg";
            skyboxPath.BottomFilepath = "res/skybox/bottom.jpg";

            m_Scene->Skybox = new sSkybox();
            m_Scene->Skybox->Geometry = sCube();
            m_Scene->Skybox->GeometryInfo = MGeometry::Add(m_Scene->Skybox->Geometry);
            m_Scene->Skybox->Texture = MTexture::LoadCubemap("Skybox", skyboxPath, eTextureFormat::RGBA8);
        }

        // Plane
        {
            m_Plane = cEntity("sPlane", m_Scene);
            m_Plane.Get<CTransform>().Position = { 0, 0, 0 };
            m_Plane.Get<CTransform>().Scale = { 5, 1, 5 };
            m_Plane.Add<CMaterialInfo>(MMaterial::Add(CMaterial()));
            m_Plane.Add<CGeometryInfo>(MGeometry::Add(sPlane(10)));
            m_Plane.Get<CGeometryInfo>().Entities = { m_Plane.GetID() };
            m_Plane.Add<COpaque>();

            sPlaneShapeDescriptor planeShapeDesc;
            MPhysics::AddActor(
                m_Plane,
                eActorType::RIGID_STATIC,
                &planeShapeDesc,
                glm::vec3(0.0f),
                0.5f, 0.5f, 0.5f,
                0.05f, 0.0f
            );
        }

        // Sunlight
        {
            m_SunLight = cEntity("SunLight", m_Scene);
            m_SunLight.Get<CTransform>().Position = { -2.0, 4.0, -1.0 };
            m_SunLight.Add<CMaterialInfo>(MMaterial::Add(CMaterial()));
            m_SunLight.Add<CGeometryInfo>(MGeometry::Add(sSphere()));
            m_SunLight.Get<CGeometryInfo>().Entities = { m_SunLight.GetID() };
            m_SunLight.Add<CDirectionalLight>(glm::vec3(-2, 4, -1), glm::vec3(1, 1, 10));
            m_SunLight.Add<COpaque>();
        }

        // Goblins
        {
            m_Goblins = cEntity("Goblins", m_Scene);

            m_Goblin1 = cEntity("Goblin1", m_Scene);
            m_Goblin1.Get<CTransform>().Position = { -4, 0, -4 };
            m_Goblin1.Get<CTransform>().Rotation = { 0, 0, 0 };
            m_Goblin1.Get<CTransform>().Scale = { 5, 5, 5 };

            m_Goblin2 = cEntity("Goblin2", m_Scene);
            m_Goblin2.Get<CTransform>().Position = { -4, 0, 4 };
            m_Goblin2.Get<CTransform>().Rotation = { 0, 0, 0 };
            m_Goblin2.Get<CTransform>().Scale = { 5, 5, 5 };
            m_Goblin2.Remove<CVisible>();

            m_Goblin3 = cEntity("Goblin3", m_Scene);
            m_Goblin3.Get<CTransform>().Position = { 4, 0, -4 };
            m_Goblin3.Get<CTransform>().Rotation = { 0, 0, 0 };
            m_Goblin3.Get<CTransform>().Scale = { 5, 5, 5 };
            m_Goblin3.Remove<CVisible>();

            m_Goblin4 = cEntity("Goblin4", m_Scene);
            m_Goblin4.Get<CTransform>().Position = { 4, 0, 4 };
            m_Goblin4.Get<CTransform>().Rotation = { 0, 0, 0 };
            m_Goblin4.Get<CTransform>().Scale = { 5, 5, 5 };
            m_Goblin4.Remove<CVisible>();

            auto goblinGeometry = MGeometry::Load("res/models/winter-girl/source/dancing_vampire.dae");
            MGeometry::Merge(goblinGeometry);
            auto goblinGeometryInfo = MGeometry::Add(goblinGeometry);
            auto goblinSkeleton = MAnim::LoadSkeleton("res/models/winter-girl/source/dancing_vampire.dae");
            auto goblinSkeletonInfo = MAnim::AddSkeleton(goblinSkeleton);
            auto goblinAnimation = MAnim::LoadAnimation("res/models/winter-girl/source/dancing_vampire.dae");
            m_Goblins.Add<CSkeleton>(goblinSkeleton);
            m_Goblins.Add<CGeometryInfo>(goblinGeometryInfo);
            m_Goblins.Add<CSkeletonInfo>(goblinSkeletonInfo);
            m_Goblins.Get<CGeometryInfo>().Entities =
            {
                    m_Goblin1.GetID(),
                    m_Goblin2.GetID(),
                    m_Goblin3.GetID(),
                    m_Goblin4.GetID()
            };
            m_Goblins.Add<CAnimation>(goblinAnimation);
            m_Goblins.Get<CAnimation>().Play = true;
            m_Goblins.Add<COpaque>();
            m_Goblins.Add<CShadow>();

            sMaterialFilepath materialFilepath;
            materialFilepath.AlbedoFilepath = "res/models/winter-girl/textures/Vampire_diffuse.png";
            materialFilepath.BumpFilepath = "res/models/winter-girl/textures/Vampire_normal.png";
            materialFilepath.RoughnessFilepath = "res/models/winter-girl/textures/Vampire_diffuse.png";
            materialFilepath.MetallicFilepath = "res/models/winter-girl/textures/Vampire_specular.png";

            CMaterial material1;
            material1.Albedo = { 1, 1, 1, 1 };
            material1.Emission = { 0, 0, 10 };
            material1.Metallness = 0.0f;
            material1.Roughness = 0.05f;
            material1.AO = 0.0f;
            material1.EnableAlbedoMap = false;
            material1.EnableNormalMap = false;
            material1.EnableRoughnessMap = false;
            material1.EnableMetalMap = false;
            material1.EnableAOMap = false;
            m_Goblin1.Add<CMaterialInfo>(MMaterial::Add(material1));

            CMaterial material2;
            material2.Albedo = { 1, 1, 1, 1 };
            material2.Emission = { 0, 0, 10 };
            material2.Metallness = 0.0f;
            material2.Roughness = 0.05f;
            material2.AO = 0.0f;
            material2.EnableAlbedoMap = false;
            material2.EnableNormalMap = false;
            material2.EnableRoughnessMap = false;
            material2.EnableMetalMap = false;
            material2.EnableAOMap = false;
            m_Goblin2.Add<CMaterialInfo>(MMaterial::Add(material2));

            CMaterial material3;
            material3.Albedo = { 1, 1, 1, 1 };
            material3.Emission = { 0, 0, 10 };
            material3.Metallness = 0.0f;
            material3.Roughness = 0.05f;
            material3.AO = 0.0f;
            material3.EnableAlbedoMap = false;
            material3.EnableNormalMap = false;
            material3.EnableRoughnessMap = false;
            material3.EnableMetalMap = false;
            material3.EnableAOMap = false;
            m_Goblin3.Add<CMaterialInfo>(MMaterial::Add(material3));

            CMaterial material4;
            material4.Albedo = { 1, 1, 1, 1 };
            material4.Emission = { 0, 0, 10 };
            material4.Metallness = 0.0f;
            material4.Roughness = 0.05f;
            material4.AO = 0.0f;
            material4.EnableAlbedoMap = false;
            material4.EnableNormalMap = false;
            material4.EnableRoughnessMap = false;
            material4.EnableMetalMap = false;
            material4.EnableAOMap = false;
            m_Goblin4.Add<CMaterialInfo>(MMaterial::Add(material4));

            // for testing image resizing on CPU, we can write resized image into file

            MTexture::SaveLayer(
                    "generated/Vampire_diffuse_resized.png",
                    material1.AlbedoMap,
                    eFileFormat::PNG
            );

            MTexture::SaveLayer(
                    "generated/Vampire_normal_resized.png",
                    material1.NormalMap,
                    eFileFormat::PNG
            );

            MTexture::SaveLayer(
                    "generated/Vampire_roughness_resized.png",
                    material1.RoughnessMap,
                    eFileFormat::PNG
            );

            MTexture::SaveLayer(
                    "generated/Vampire_metallic_resized.png",
                    material1.MetalMap,
                    eFileFormat::PNG
            );

            int w, h, c;
            cTextureLayer* layer = MTexture::LoadLayer("res/skybox/back.jpg", eTextureFormat::RGBA8, w, h, c);
            cAtlas atlas;
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
            MTexture::SaveLayer(
                "generated/test_atlas.png",
                &atlas.Layers[0],
                eFileFormat::PNG
            );
        }

        // Cube
        {
            m_Cube = cEntity("Cube", m_Scene);
            m_Cube.Get<CTransform>().Position = { 10.0f, 2.5f, 10.0f };
            m_Cube.Get<CTransform>().Scale = glm::vec3(5.0f);
            m_Cube.Add<CGeometryInfo>(MGeometry::Add(sCube()));
            m_Cube.Get<CGeometryInfo>().Entities = { m_Cube.GetID() };
            m_Cube.Add<CMaterialInfo>(MMaterial::Add(CMaterial()));
            m_Cube.Add<COpaque>();
            m_Cube.Add<CShadow>();
        }

        // Spheres
        for (s32 i = 0; i < 4; i++)
        {
            CMaterial mat;

            if (i == 0) { mat.Albedo = glm::vec4(1.0f, 0.0f, 0.0f, 0.2f); }
            if (i == 1) { mat.Albedo = glm::vec4(0.0f, 1.0f, 0.0f, 0.4f); }
            if (i == 2) { mat.Albedo = glm::vec4(0.0f, 0.0f, 1.0f, 0.6f); }
            if (i == 3) { mat.Albedo = glm::vec4(1.0f, 1.0f, 0.0f, 0.8f); }

            mat.Metallness = 0.0f;
            mat.Roughness = 0.05f;
            mat.AO = 0.0f;

            m_Glasses[i] = cEntity("Glass-" + string(std::to_string(i)), m_Scene);
            m_Glasses[i].Get<CTransform>().Position = glm::vec3(0 + ((float)i * 0.5f), 1.1 + ((float)i * 2.0f), -5);
            m_Glasses[i].Get<CTransform>().Scale = glm::vec3(1.0f);
            m_Glasses[i].Add<CGeometryInfo>(MGeometry::Add(sSphere()));
            m_Glasses[i].Get<CGeometryInfo>().Entities = { m_Glasses[i].GetID() };
            m_Glasses[i].Add<CMaterialInfo>(MMaterial::Add(mat));
            m_Glasses[i].Add<CTransparent>();
            m_Glasses[i].Add<CShadow>();

            sSphereShapeDescriptor sphereShapeDesc(1.0f);
            MPhysics::AddActor(
                m_Glasses[i],
                eActorType::RIGID_DYNAMIC,
                &sphereShapeDesc,
                glm::vec3(0.0f),
                0.5f, 0.5f, 0.5f,
                0.05f, 0.0f
            );
        }
//        // settings for shadows
//        Buffers::ShadowPCF->Item.FilterSize = 0;
//        Buffers::ShadowPCF->Flush();
//
//        {
//            m_AudioBox = cEntity("AudioBox", m_Scene);
//
//            m_AudioBox.Get<CTransform>().Position = glm::vec3(-5.0f, 5.0f, 10.0f);
//            m_AudioBox.Get<CTransform>().Scale = glm::vec3(1.0f);
//            m_AudioBox.Add<CGeometryInfo>(MGeometry::Add(sCube()));
//            m_AudioBox.Get<CGeometryInfo>().Entities = { m_AudioBox.GetID() };
//            m_AudioBox.Add<CMaterialInfo>(MMaterial::Add(CMaterial()));
//            m_AudioBox.Add<COpaque>();
//
//            auto& test1 = m_AudioBox.Add<CStreamAudio>(CStreamAudio());
//            test1.File = MAudio::Load("res/audio/mono_test.wav");
//        }
//
//        Buffers::SSAO->Item.Intensity = 2;
//        Buffers::SSAO->Flush();
    }

    void Update() override final
    {
        UpdateCamera();
        UpdateListener();
//        DisplayStats();
    }

    void Free()
    {
        LogInfo("cExample::Free()");
        MEvent::WindowClosed.Remove("Example");
        MEvent::KeyPressed.Remove("Example");
        MEvent::KeyHold.Remove("Example");
        MEvent::CursorMoved.Remove("Example");
    }

    void WindowClosed()
    {
        LogWarning("cExample::WindowClosed()");
    }

    void KeyPressed(const eKey key)
    {
        if (key == eKey::Esc)
        {
            MWindow::Close();
        }

        MoveLight(key);
        PlayAnimations(key);

        if (key == eKey::R)
        {
            Textures::Canvas = RenderTargets::DirectionalShadow->Colors[0];
            Viewports::Canvas = Viewports::DirectionalShadow;
        }

        if (key == eKey::T)
        {
            Textures::Canvas = RenderTargets::Final->Colors[0];
        }

        if (key == eKey::V)
        {
            MAudio::VoiceRecord();
        }
    }

    void KeyHold(const eKey key)
    {
        MoveLight(key);
    }

    void CursorMoved(const double x, const double y)
    {
        if (MCamera::Camera != nullptr)
        {
            if (MInput::MousePressed(eMouse::ButtonRight))
            {
                MCamera::Camera->EnableLook = false;
                MInput::CaptureCursor(x, y);
                MCamera::Pan(MInput::GetMouseCursor().Delta);
            }
            else
            {
                MCamera::Camera->EnableLook = true;
            }
        }
    }

private:

    void InitCamera()
    {
        MCamera::Camera->Projection.Far = m_TestConfig.CameraFar;
        MCamera::Camera->Position = { 5, 5, 20 };
        MCamera::Camera->Projection.AspectRatio = m_Config.AspectRatio;
        MCamera::Camera->MoveSpeed = m_TestConfig.CameraMoveSpeed;
        MCamera::Camera->ZoomAcceleration = m_TestConfig.CameraZoomAcceleration;
        MCamera::Camera->PanAcceleration = m_TestConfig.CameraPanAcceleration;
        MCamera::Camera->HorizontalSensitivity = m_TestConfig.CameraHorizontalSens;
        MCamera::Camera->VerticalSensitivity = m_TestConfig.CameraVerticalSens;
        MCamera::Flush();
    }

    void UpdateCamera()
    {
        if (MCamera::Camera != nullptr)
        {
            if (MInput::MousePressed(eMouse::ButtonLeft))
            {
                MCamera::Camera->Mode = eCameraMode::EDITOR;
            }
            else
            {
                MCamera::Camera->Mode = eCameraMode::GAME;
            }
            MCamera::Move();
        }
    }

    void UpdateListener()
    {
        MAudio::SetListenerPosition(MCamera::Camera->Position);
        MAudio::SetListenerOrientation(MCamera::Camera->Rotation, MCamera::Camera->Up);
    }

    void MoveLight(const eKey key)
    {
        auto& pos = m_SunLight.Get<CTransform>().Position;

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

        m_SunLight.Get<CDirectionalLight>().View.Position = pos;
    }

    void PlayAnimations(const eKey key)
    {
        if (key == eKey::P)
        {
            m_Goblins.Get<CAnimation>().Play = !m_Goblins.Get<CAnimation>().Play;
        }
    }

private:
    cEntity m_SunLight;
    cEntity m_Plane;
    cEntity m_Goblins;
    cEntity m_Goblin1;
    cEntity m_Goblin2;
    cEntity m_Goblin3;
    cEntity m_Goblin4;
    cEntity m_Cube;
    cEntity m_AudioBox;
    cEntity m_Glasses[4];

    sTestConfig m_TestConfig;

    cEntity m_Menu;
    cEntity m_Button;
    cEntity m_Label;
};

cApp* CreateApplication()
{
    return new cExample();
}