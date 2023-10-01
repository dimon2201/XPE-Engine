#include <core/app.hpp>
#include <core/windowing.hpp>

#include <ecs/scenes.hpp>
#include <ecs/entities.hpp>

#include <rendering/core/debugger.h>

#include <rendering/monitor.h>
#include <rendering/renderer.h>

#include <rendering/passes/canvas.hpp>
#include <rendering/passes/instancing_pass.h>
#include <rendering/passes/skeletal_anim_pass.h>
#include <rendering/passes/text2d_pass.h>
#include <rendering/passes/text3d_pass.h>
#include <rendering/passes/skybox_pass.h>
#include <rendering/passes/merge_pass.h>
#include <rendering/passes/ssao_pass.hpp>
#include <rendering/passes/fxaa_pass.hpp>

#include <rendering/shadow/shadow.h>

#include <rendering/storages/texture_storage.h>
#include <rendering/storages/font_storage.h>

#include <anim/animator.h>
#include <anim/storages/anim_storage.h>

#include <audio/audio_system.h>
#include <audio/storages/audio_storage.h>

#include <thread>

namespace xpe {

    using namespace render;
    using namespace ecs;
    using namespace anim;
    using namespace audio;

    namespace core {

        void Application::Run()
        {
            LoggerDescriptor logDesc;
            logDesc.Name = Config.LogTitle.c_str();
            logDesc.Backtrace = Config.LogBacktrace;
            InitLogger(logDesc);

            WindowDescriptor winDesc;
            winDesc.Title = Config.WinTitle.c_str();
            winDesc.Width = Config.WinWidth;
            winDesc.Height = Config.WinHeight;
            winDesc.X = Config.WinX;
            winDesc.Y = Config.WinY;
            winDesc.VSync = Config.VSync;
            winDesc.Gamma = Config.Gamma;
            winDesc.Exposure = Config.Exposure;

            DeltaTime.SetFps(Config.FPS);
            CPUTime = DeltaTime;

            WindowManager::Init();
            WindowManager::InitWindow(winDesc);

            Input::Init();

            debugger::DebugErrors = Config.DebugErrors;
            debugger::DebugWarnings = Config.DebugWarnings;
            debugger::DebugInfo = Config.DebugInfo;

            m_Renderer = new Renderer();
            m_FontStorage = new FontStorage();
            m_GeometryStorage = new GeometryStorage();
            m_MaterialStorage = new MaterialStorage();
            m_TextureStorage = new TextureStorage();

            m_SkeletStorage = new SkeletStorage();
            m_SkinStorage = new SkinStorage();
            m_AnimStorage = new AnimStorage();
            m_Animator = new Animator(m_SkeletStorage);

            InitRenderer();

            Monitor::Get().Exposure = winDesc.Exposure;
            Monitor::Get().Gamma = winDesc.Gamma;

            m_AudioSystem = new AudioSystem();
            m_AudioStorage = new AudioStorage();

            m_MainScene = new MainScene();
            m_MainScene->PerspectiveCamera->Buffer = m_Renderer->CameraBuffer;
            m_MainScene->OrthoCamera->Buffer = m_Renderer->CameraBuffer;

            Init();
            m_Game = CreateGame();
            InitGame();

            std::atomic<bool> AudioThreadFlag(true);
            std::thread AudioThread([this, &AudioThreadFlag]() {
                while (AudioThreadFlag) {
                    m_AudioSystem->Update(m_MainScene);
                }
            });

            while (!WindowManager::ShouldClose())
            {

                // measure cpu ticks in seconds and log CPU time
#ifdef DEBUG
                static float cpuTickSec = 0;
                cpuTickSec += CPUTime.Seconds();
                if (cpuTickSec >= Config.LogTimeDelaySeconds) {
                    cpuTickSec = 0;
                    LogCpuTime(CPUTime);
                    LogMemoryPools();
                    LogStackMemory();
                }
#endif

                Timer cpuTimer(&CPUTime);
                Timer deltaTimer(&DeltaTime);

                CurrentTime = cpuTimer.GetStartTime();

                m_Animator->Animate(m_MainScene, DeltaTime);

                Update();

                m_Game->Update();

                Render();

                m_AudioSystem->UpdateListener(m_MainScene);

                WindowManager::PollEvents();
                WindowManager::Swap();

                // measure delta ticks in seconds and log delta
#ifdef DEBUG
                static float deltaTickSec = 0;
                deltaTickSec += CPUTime.Seconds();
                if (deltaTickSec >= Config.LogTimeDelaySeconds) {
                    deltaTickSec = 0;
                    LogTime(DeltaTime);
                }
#endif

            }

            AudioThreadFlag = false;
            AudioThread.join();

            m_Game->Free();
            delete m_Game;
            Free();

            delete m_MainScene;

            delete m_FontStorage;
            delete m_GeometryStorage;
            delete m_MaterialStorage;
            delete m_TextureStorage;

            delete m_AnimStorage;
            delete m_SkinStorage;
            delete m_SkeletStorage;
            delete m_Animator;

            delete m_AudioSystem;
            delete m_AudioStorage;

            delete m_Canvas;
            delete m_Renderer;

            Input::Free();

            WindowManager::FreeWindow();
            WindowManager::Free();

            FreeLogger();
        }

        void Application::LockFPSFromConfig()
        {
            if (Config.LockOnFps && Config.FPS < DeltaTime.Fps())
            {
                DeltaTime.SetFps(Config.FPS);
            }
        }

        void Application::InitGame()
        {
            m_Game->CPUTime = &CPUTime;
            m_Game->DeltaTime = &DeltaTime;
            m_Game->CurrentTime = &CurrentTime;

            m_Game->Config = &Config;

            m_Game->MainScene = m_MainScene;

            m_Game->Renderer = m_Renderer;
            m_Game->FontStorage = m_FontStorage;
            m_Game->MaterialStorage = m_MaterialStorage;
            m_Game->GeometryStorage = m_GeometryStorage;
            m_Game->TextureStorage = m_TextureStorage;

            m_Game->SkeletStorage = m_SkeletStorage;
            m_Game->SkinStorage = m_SkinStorage;
            m_Game->AnimStorage = m_AnimStorage;
            m_Game->Animator = m_Animator;

            m_Game->AudioSystem = m_AudioSystem;
            m_Game->AudioStorage = m_AudioStorage;

            m_Game->Init();
        }

        Game* Application::CreateGame()
        {
            return new Game();
        }

        void Application::InitRenderer()
        {
            // Canvas
            Shader* shader = ShaderManager::CreateShader("canvas");
            ShaderManager::AddVertexStageFromFile(shader, "engine_shaders/passes/canvas.vs");
            ShaderManager::AddPixelStageFromFile(shader, "engine_shaders/passes/canvas.ps");
            ShaderManager::BuildShader(shader);
            m_Canvas = new Canvas(WindowManager::GetWidth(), WindowManager::GetHeight(), shader);

            RenderTarget* canvasRT = m_Canvas->GetRenderTarget();
            Viewport* canvasViewport = m_Canvas->GetViewport(0);

            // Main render target
            Texture* mainColor = new Texture();
            mainColor->Width = m_Canvas->GetDimension().x;
            mainColor->Height = m_Canvas->GetDimension().y;
            mainColor->Format = eTextureFormat::RGBA8;
            mainColor->InitializeData = false;
            mainColor->EnableRenderTarget = true;
            mainColor->Init();

            Texture* mainPosition = new Texture();
            mainPosition->Width = m_Canvas->GetDimension().x;
            mainPosition->Height = m_Canvas->GetDimension().y;
            mainPosition->Format = eTextureFormat::RGBA32;
            mainPosition->InitializeData = false;
            mainPosition->EnableRenderTarget = true;
            mainPosition->Init();

            Texture* mainNormal = new Texture();
            mainNormal->Width = m_Canvas->GetDimension().x;
            mainNormal->Height = m_Canvas->GetDimension().y;
            mainNormal->Format = eTextureFormat::RGBA16;
            mainNormal->InitializeData = false;
            mainNormal->EnableRenderTarget = true;
            mainNormal->Init();

            Texture* mainDepth = new Texture();
            mainDepth->Type = Texture::eType::TEXTURE_2D_DEPTH_STENCIL;
            mainDepth->Width = m_Canvas->GetDimension().x;
            mainDepth->Height = m_Canvas->GetDimension().y;
            mainDepth->Format = eTextureFormat::R32_TYPELESS;
            mainDepth->InitializeData = false;
            mainDepth->EnableRenderTarget = true;
            mainDepth->Init();

            MainRT = new RenderTarget({ mainColor, mainPosition, mainNormal }, mainDepth, *canvasViewport);

            // SSAO render target
            Texture* ssaoColor = new Texture();
            ssaoColor->Width = m_Canvas->GetDimension().x;
            ssaoColor->Height = m_Canvas->GetDimension().y;
            ssaoColor->Format = eTextureFormat::RGBA8;
            ssaoColor->InitializeData = false;
            ssaoColor->EnableRenderTarget = true;
            ssaoColor->Init();

            Texture* ssaoDepth = new Texture();
            ssaoDepth->Type = Texture::eType::TEXTURE_2D_DEPTH_STENCIL;
            ssaoDepth->Width = m_Canvas->GetDimension().x;
            ssaoDepth->Height = m_Canvas->GetDimension().y;
            ssaoDepth->Format = eTextureFormat::R32_TYPELESS;
            ssaoDepth->InitializeData = false;
            ssaoDepth->EnableRenderTarget = true;
            ssaoDepth->Init();

            SsaoRT = new RenderTarget({ ssaoColor }, ssaoDepth, *canvasViewport);

            // Instancing pass
            {
                Shader* shader = ShaderManager::CreateShader("instancing");
                ShaderManager::AddVertexStageFromFile(shader, "engine_shaders/passes/instancing_pass.vs");
                ShaderManager::AddPixelStageFromFile(shader, "engine_shaders/passes/pbr_pass.ps");
                ShaderManager::BuildShader(shader);

                vector<RenderPassBinding> bindings = {
                        { "Shader", RenderPassBinding::eType::SHADER, RenderPassBinding::eStage::VERTEX, 0, shader },
                        { "CameraBuffer", RenderPassBinding::eType::BUFFER, RenderPassBinding::eStage::VERTEX, RenderPassBinding::SLOT_DEFAULT, m_Renderer->CameraBuffer },
                        { "DirectLightBuffer", RenderPassBinding::eType::BUFFER, RenderPassBinding::eStage::PIXEL, RenderPassBinding::SLOT_DEFAULT, m_Renderer->DirectLightBuffer },
                        { "PointLightBuffer", RenderPassBinding::eType::BUFFER, RenderPassBinding::eStage::PIXEL, RenderPassBinding::SLOT_DEFAULT, m_Renderer->PointLightBuffer },
                        { "SpotLightBuffer", RenderPassBinding::eType::BUFFER, RenderPassBinding::eStage::PIXEL, RenderPassBinding::SLOT_DEFAULT, m_Renderer->SpotLightBuffer },
                        { "MonitorBuffer", RenderPassBinding::eType::BUFFER, RenderPassBinding::eStage::PIXEL, RenderPassBinding::SLOT_DEFAULT, Monitor::Get().GetBuffer() },
                        { "ShadowFilterBuffer", RenderPassBinding::eType::BUFFER, RenderPassBinding::eStage::PIXEL, RenderPassBinding::SLOT_DEFAULT, Shadow::Get().GetBuffer() }
                };

                m_Renderer->AddRenderPass<InstancingPass>(
                    bindings,
                    MainRT,
                    m_MaterialStorage
                );
            }

            // Skeletal animation pass
            {
                Shader* shader = ShaderManager::CreateShader("skeletal_anim");
                ShaderManager::AddVertexStageFromFile(shader, "engine_shaders/passes/skeletal_anim_pass.vs");
                ShaderManager::AddPixelStageFromFile(shader, "engine_shaders/passes/pbr_pass.ps");
                ShaderManager::BuildShader(shader);

                vector<RenderPassBinding> bindings = {
                    { "Shader", RenderPassBinding::eType::SHADER, RenderPassBinding::eStage::VERTEX, 0, shader },
                    { "CameraBuffer", RenderPassBinding::eType::BUFFER, RenderPassBinding::eStage::VERTEX, RenderPassBinding::SLOT_DEFAULT, m_Renderer->CameraBuffer },
                    { "DirectLightBuffer", RenderPassBinding::eType::BUFFER, RenderPassBinding::eStage::PIXEL, RenderPassBinding::SLOT_DEFAULT, m_Renderer->DirectLightBuffer },
                    { "PointLightBuffer", RenderPassBinding::eType::BUFFER, RenderPassBinding::eStage::PIXEL, RenderPassBinding::SLOT_DEFAULT, m_Renderer->PointLightBuffer },
                    { "SpotLightBuffer", RenderPassBinding::eType::BUFFER, RenderPassBinding::eStage::PIXEL, RenderPassBinding::SLOT_DEFAULT, m_Renderer->SpotLightBuffer },
                    { "MonitorBuffer", RenderPassBinding::eType::BUFFER, RenderPassBinding::eStage::PIXEL, RenderPassBinding::SLOT_DEFAULT, Monitor::Get().GetBuffer() },
                    { "ShadowFilterBuffer", RenderPassBinding::eType::BUFFER, RenderPassBinding::eStage::PIXEL, RenderPassBinding::SLOT_DEFAULT, Shadow::Get().GetBuffer() }
                };

                m_Renderer->AddRenderPass<SkeletalAnimPass>(
                    bindings,
                    MainRT,
                    m_MaterialStorage
                );
            }

            // Text 2D pass
            {
                Shader* shader = ShaderManager::CreateShader("text2d");
                ShaderManager::AddVertexStageFromFile(shader, "engine_shaders/passes/text2d_pass.vs");
                ShaderManager::AddPixelStageFromFile(shader, "engine_shaders/passes/text2d_pass.ps");
                ShaderManager::BuildShader(shader);

                vector<RenderPassBinding> bindings = {
                    { "Shader", RenderPassBinding::eType::SHADER, RenderPassBinding::eStage::VERTEX, 0, shader },
                    { "CameraBuffer", RenderPassBinding::eType::BUFFER, RenderPassBinding::eStage::VERTEX, RenderPassBinding::SLOT_DEFAULT, m_Renderer->CameraBuffer },
                    { "ViewportBuffer", RenderPassBinding::eType::BUFFER, RenderPassBinding::eStage::VERTEX, RenderPassBinding::SLOT_DEFAULT, m_Canvas->GetBuffer() }
                };

                m_Renderer->AddRenderPass<Text2DPass>(
                    bindings,
                    MainRT,
                    m_GeometryStorage
                );
            }

            // Text 3D pass
            {
                Shader* shader = ShaderManager::CreateShader("text3d");
                ShaderManager::AddVertexStageFromFile(shader, "engine_shaders/passes/text3d_pass.vs");
                ShaderManager::AddPixelStageFromFile(shader, "engine_shaders/passes/text3d_pass.ps");
                ShaderManager::BuildShader(shader);

                vector<RenderPassBinding> bindings = {
                    { "Shader", RenderPassBinding::eType::SHADER, RenderPassBinding::eStage::VERTEX, 0, shader },
                    { "CameraBuffer", RenderPassBinding::eType::BUFFER, RenderPassBinding::eStage::VERTEX, RenderPassBinding::SLOT_DEFAULT, m_Renderer->CameraBuffer },
                    { "ViewportBuffer", RenderPassBinding::eType::BUFFER, RenderPassBinding::eStage::VERTEX, RenderPassBinding::SLOT_DEFAULT, m_Canvas->GetBuffer() }
                };

                m_Renderer->AddRenderPass<Text3DPass>(
                    bindings,
                    MainRT,
                    m_GeometryStorage
                );
            }

            // SSAO pass
            {
                m_GeometryStorage->AddGeometryIndexed2D("SSAOQuad", Quad2D());

                Shader* shader = ShaderManager::CreateShader("ssao");
                ShaderManager::AddVertexStageFromFile(shader, "engine_shaders/passes/ssao_pass.vs");
                ShaderManager::AddPixelStageFromFile(shader, "engine_shaders/passes/ssao_pass.ps");
                ShaderManager::BuildShader(shader);

                vector<RenderPassBinding> bindings = {
                    { "Shader", RenderPassBinding::eType::SHADER, RenderPassBinding::eStage::VERTEX, 0, shader },
                    { "PositionTexture", RenderPassBinding::eType::TEXTURE, RenderPassBinding::eStage::PIXEL, 0, MainRT->Colors[1] },
                    { "NormalTexture", RenderPassBinding::eType::TEXTURE, RenderPassBinding::eStage::PIXEL, 1, MainRT->Colors[2] },
                    { "DepthTexture", RenderPassBinding::eType::TEXTURE, RenderPassBinding::eStage::PIXEL, 2, MainRT->DepthStencil }
                };

                m_Renderer->AddRenderPass<SSAOPass>(
                    m_GeometryStorage,
                    bindings,
                    SsaoRT
                );
            }

            // FXAA pass
            fxaaPass = nullptr;
            {
                Shader* shader = ShaderManager::CreateShader("fxaa");
                ShaderManager::AddVertexStageFromFile(shader, "engine_shaders/passes/fxaa_pass.vs");
                ShaderManager::AddPixelStageFromFile(shader, "engine_shaders/passes/fxaa_pass.ps");
                ShaderManager::BuildShader(shader);

                vector<RenderPassBinding> bindings = {
                        { "Shader", RenderPassBinding::eType::SHADER, RenderPassBinding::eStage::VERTEX, 0, shader },
                        { "ColorTexture", RenderPassBinding::eType::TEXTURE, RenderPassBinding::eStage::PIXEL, 0, MainRT->Colors[0] }
                };

                fxaaPass = m_Renderer->AddRenderPass<FXAAPass>(
                    bindings,
                    m_Canvas->GetDimension(),
                    canvasViewport
                );
            }

            // Canvas pass
            {
                Shader* shader = ShaderManager::CreateShader("merge");
                ShaderManager::AddVertexStageFromFile(shader, "engine_shaders/passes/merge_pass.vs");
                ShaderManager::AddPixelStageFromFile(shader, "engine_shaders/passes/merge_pass.ps");
                ShaderManager::BuildShader(shader);

                vector<RenderPassBinding> bindings = {
                    { "Shader", RenderPassBinding::eType::SHADER, RenderPassBinding::eStage::VERTEX, 0, shader },
                    { "ColorTexture", RenderPassBinding::eType::TEXTURE, RenderPassBinding::eStage::PIXEL, 0, fxaaPass->GetRenderTarget()->Colors[0] },
                    { "AOTexture", RenderPassBinding::eType::TEXTURE, RenderPassBinding::eStage::PIXEL, 1, SsaoRT->Colors[0] }
                };

                m_Renderer->AddRenderPass<MergePass>(
                    bindings,
                    canvasRT,
                    m_GeometryStorage
                );
            }
        }

        void Application::Render()
        {
            MainRT->ClearColor(0, { 1, 1, 1, 1 });
            MainRT->ClearColor(1, { 0, 0, 0, 0 });
            MainRT->ClearColor(2, { 0, 0, 0, 0 });
            MainRT->ClearDepth(1);

            SsaoRT->ClearColor(0, { 1, 1, 1, 1 });
            SsaoRT->ClearDepth(1);

            fxaaPass->GetRenderTarget()->ClearColor(0, { 0, 0, 0, 0 });
            fxaaPass->GetRenderTarget()->ClearDepth(1);

            m_Canvas->Clear(ClearColor);
            m_Renderer->Render(m_MainScene);
            m_Canvas->Present();
        }

    }

}
