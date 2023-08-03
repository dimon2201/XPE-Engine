#include <core/core.hpp>

#include "test_config.h"
#include "launcher.h"

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

        m_Font = TTFManager::Load("resources/fonts/Roboto-Italic.ttf", 32);
        Font* f = TTFManager::Resize("resources/fonts/Roboto-Italic.ttf", 80);
        m_Font = *f;
        TextureManager::WriteTextureFile("C:/Users/USER100/Documents/GitHub/XPE-Engine/font.png", m_Font.Atlas, Texture::eFileFormat::PNG);

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
            TextRenderer::Get().Draw(&m_Font, &transform, "Hello!\nNew line");

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
