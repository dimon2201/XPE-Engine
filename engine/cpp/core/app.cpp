#include <core/app.hpp>

#include <ecs/scenes.hpp>

#include <rendering/core/debugger.h>

#include <rendering/monitor.h>
#include <rendering/renderer.h>

#include <rendering/passes/canvas.hpp>
#include <rendering/passes/main_pass.h>
#include <rendering/passes/skeletal_anim_pass.h>
#include <rendering/passes/text2d_pass.h>
#include <rendering/passes/text3d_pass.h>
#include <rendering/passes/merge_pass.h>
#include <rendering/passes/ssao_pass.hpp>
#include <rendering/passes/collision_pass.h>

#include <rendering/shadow/shadow.h>

#include <rendering/storages/texture_storage.h>
#include <rendering/storages/font_storage.h>

#include <anim/animator.h>
#include <anim/storages/anim_storage.h>

#include <audio/audio_system.h>
#include <audio/storages/audio_storage.h>

#include <physics/physics.h>

#include <dispatcher/main_dispatcher.h>

namespace xpe {

    using namespace render;
    using namespace ecs;
    using namespace anim;
    using namespace audio;
    using namespace physics;

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

            MainDispatcher* mainDispatcher = new MainDispatcher(
                    Hardware::CPU.Cores,
                    100,
                    "Worker",
                    Thread::ePriority::NORMAL
            );

            TaskManager::Init(mainDispatcher);
            PhysicsManager::Init(mainDispatcher);

            WindowManager::Init();
            WindowManager::InitWindow(winDesc);

            Input::Init();

            debugger::DebugErrors = Config.DebugErrors;
            debugger::DebugWarnings = Config.DebugWarnings;
            debugger::DebugInfo = Config.DebugInfo;

            m_Renderer = new Renderer();
            m_FontStorage = new FontStorage();
            m_MaterialStorage = new MaterialStorage();
            m_TextureStorage = new TextureStorage();

            m_SkeletStorage = new SkeletStorage();
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

            while (m_IsOpen)
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

                Update();
                m_Game->Update();

                // submit audio task with current scene state
                TaskManager::SubmitTask({[this]() {
                    m_AudioSystem->Update(m_MainScene);
                    m_AudioSystem->UpdateListener(m_MainScene);
                }});

                // submit animation task with current scene state
                TaskManager::SubmitTask({[this]() {
                    m_Animator->Animate(m_MainScene, DeltaTime);
                }});
                
                // todo: fix crush when use SubmitTask to update physics
                //TaskManager::SubmitTask({[this]() {
                //    PhysicsManager::Update(m_MainScene, DeltaTime); // mb to fix the truble I need to use mutex
                //}});

                PhysicsManager::Update(m_MainScene, DeltaTime);

                Render();

                WindowManager::PollEvents();
                WindowManager::Swap();

                m_IsOpen = !WindowManager::ShouldClose();

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

            m_Game->Free();
            delete m_Game;
            Free();

            delete m_MainScene;

            delete m_FontStorage;
            delete m_MaterialStorage;
            delete m_TextureStorage;

            delete m_AnimStorage;
            delete m_SkeletStorage;
            delete m_Animator;

            delete m_AudioSystem;
            delete m_AudioStorage;

            delete m_Canvas;
            delete m_Renderer;

            PhysicsManager::Free();

            Input::Free();

            WindowManager::FreeWindow();
            WindowManager::Free();

            TaskManager::Free();

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
            m_Game->TextureStorage = m_TextureStorage;

            m_Game->SkeletStorage = m_SkeletStorage;
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
            // Canvas pass
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

            // Main pass
            {
                Shader* shader = ShaderManager::CreateShader("main");
                ShaderManager::AddVertexStageFromFile(shader, "engine_shaders/passes/main_pass.vs");
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

                m_Renderer->AddRenderPass<MainPass>(
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

            // Collision pass
            {
                Shader* shader = ShaderManager::CreateShader("collison");
                ShaderManager::AddVertexStageFromFile(shader, "engine_shaders/passes/main_pass.vs");
                ShaderManager::AddPixelStageFromFile(shader, "engine_shaders/passes/color_pass.ps");
                ShaderManager::BuildShader(shader);

                vector<RenderPassBinding> bindings = {
                    { "Shader", RenderPassBinding::eType::SHADER, RenderPassBinding::eStage::VERTEX, 0, shader },
                    { "CameraBuffer", RenderPassBinding::eType::BUFFER, RenderPassBinding::eStage::VERTEX, RenderPassBinding::SLOT_DEFAULT, m_Renderer->CameraBuffer },
                };

                m_Renderer->AddRenderPass<CollisionPass>(
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
                    MainRT
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
                };

                m_Renderer->AddRenderPass<Text3DPass>(
                    bindings,
                    MainRT
                );
            }

            // SSAO pass
            {
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
                    bindings,
                    SsaoRT
                );
            }

            // SSAO Merge pass
            {
                Shader* shader = ShaderManager::CreateShader("merge");
                ShaderManager::AddVertexStageFromFile(shader, "engine_shaders/passes/merge_pass.vs");
                ShaderManager::AddPixelStageFromFile(shader, "engine_shaders/passes/merge_pass.ps");
                ShaderManager::BuildShader(shader);

                vector<RenderPassBinding> bindings = {
                    { "Shader", RenderPassBinding::eType::SHADER, RenderPassBinding::eStage::VERTEX, 0, shader },
                    { "ColorTexture", RenderPassBinding::eType::TEXTURE, RenderPassBinding::eStage::PIXEL, 0, MainRT->Colors[0] },
                    { "AOTexture", RenderPassBinding::eType::TEXTURE, RenderPassBinding::eStage::PIXEL, 2, SsaoRT->Colors[0] }
                };

                m_Renderer->AddRenderPass<MergePass>(
                    bindings,
                    canvasRT
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

            m_Canvas->Clear(ClearColor);
            m_Renderer->Render(m_MainScene);
            m_Canvas->Present();
        }

    }

}
