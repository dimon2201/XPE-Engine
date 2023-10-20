#include <core/app.hpp>

#include <ecs/scenes.hpp>

#include <rendering/core/debugger.h>

#include <rendering/monitor.h>
#include <rendering/renderer.h>

#include <rendering/passes/canvas.hpp>
#include <rendering/passes/opaque_pass.h>
#include <rendering/passes/transparent_pass.h>
#include <rendering/passes/skeletal_anim_pass.h>
#include <rendering/passes/text2d_pass.h>
#include <rendering/passes/text3d_pass.h>
#include <rendering/passes/merge_pass.h>
#include <rendering/passes/ssao_pass.hpp>

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

            Viewport* viewport = new Viewport();
            viewport->Width = winDesc.Width;
            viewport->Height = winDesc.Height;

            m_Renderer = new Renderer(viewport, m_UseMSAA, m_MSAASampleCount);
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
            RenderTarget* rendererRT = m_Renderer->GetRenderTarget();

            // Main pass
            {
                Shader* shader = ShaderManager::CreateShader("main");
                ShaderManager::AddVertexStageFromFile(shader, "engine_shaders/passes/main_pass.vs");
                ShaderManager::AddPixelStageFromFile(shader, "engine_shaders/passes/main_opaque_pass.ps");
                ShaderManager::BuildShader(shader);

                vector<RenderPassBinding> bindings = {
                        { "Vertex3D", RenderPassBinding::eType::VERTEX_3D },
                        { "Shader", RenderPassBinding::eType::SHADER, shader },
                        { "CameraBuffer", RenderPassBinding::eType::BUFFER, m_Renderer->CameraBuffer, RenderPassBinding::eStage::VERTEX, RenderPassBinding::SLOT_DEFAULT },
                        { "DirectLightBuffer", RenderPassBinding::eType::BUFFER, m_Renderer->DirectLightBuffer, RenderPassBinding::eStage::PIXEL, RenderPassBinding::SLOT_DEFAULT },
                        { "PointLightBuffer", RenderPassBinding::eType::BUFFER, m_Renderer->PointLightBuffer, RenderPassBinding::eStage::PIXEL, RenderPassBinding::SLOT_DEFAULT },
                        { "SpotLightBuffer", RenderPassBinding::eType::BUFFER, m_Renderer->SpotLightBuffer, RenderPassBinding::eStage::PIXEL, RenderPassBinding::SLOT_DEFAULT },
                        { "MonitorBuffer", RenderPassBinding::eType::BUFFER, Monitor::Get().GetBuffer(), RenderPassBinding::eStage::PIXEL, RenderPassBinding::SLOT_DEFAULT },
                        { "ShadowFilterBuffer", RenderPassBinding::eType::BUFFER, Shadow::Get().GetBuffer(), RenderPassBinding::eStage::PIXEL, RenderPassBinding::SLOT_DEFAULT }
                };

                m_Renderer->AddRenderPass<OpaquePass>(
                    bindings,
                    m_MaterialStorage,
                    rendererRT
                );
            }

            // Skeletal animation pass
            {
                Shader* shader = ShaderManager::CreateShader("skeletal_anim");
                ShaderManager::AddVertexStageFromFile(shader, "engine_shaders/passes/skeletal_anim_pass.vs");
                ShaderManager::AddPixelStageFromFile(shader, "engine_shaders/passes/main_opaque_pass.ps");
                ShaderManager::BuildShader(shader);

                vector<RenderPassBinding> bindings = {
                    { "VertexSkeletal", RenderPassBinding::eType::VERTEX_SKELETAL },
                    { "Shader", RenderPassBinding::eType::SHADER, shader },
                    { "CameraBuffer", RenderPassBinding::eType::BUFFER, m_Renderer->CameraBuffer, RenderPassBinding::eStage::VERTEX, RenderPassBinding::SLOT_DEFAULT },
                    { "DirectLightBuffer", RenderPassBinding::eType::BUFFER, m_Renderer->DirectLightBuffer, RenderPassBinding::eStage::PIXEL, RenderPassBinding::SLOT_DEFAULT },
                    { "PointLightBuffer", RenderPassBinding::eType::BUFFER, m_Renderer->PointLightBuffer, RenderPassBinding::eStage::PIXEL, RenderPassBinding::SLOT_DEFAULT },
                    { "SpotLightBuffer", RenderPassBinding::eType::BUFFER, m_Renderer->SpotLightBuffer, RenderPassBinding::eStage::PIXEL, RenderPassBinding::SLOT_DEFAULT },
                    { "MonitorBuffer", RenderPassBinding::eType::BUFFER, Monitor::Get().GetBuffer(), RenderPassBinding::eStage::PIXEL, RenderPassBinding::SLOT_DEFAULT },
                    { "ShadowFilterBuffer", RenderPassBinding::eType::BUFFER, Shadow::Get().GetBuffer(), RenderPassBinding::eStage::PIXEL, RenderPassBinding::SLOT_DEFAULT }
                };

                m_Renderer->AddRenderPass<SkeletalAnimPass>(
                    bindings,
                    m_MaterialStorage,
                    rendererRT
                );
            }

            // Text 2D pass
            {
                Shader* shader = ShaderManager::CreateShader("text2d");
                ShaderManager::AddVertexStageFromFile(shader, "engine_shaders/passes/text2d_pass.vs");
                ShaderManager::AddPixelStageFromFile(shader, "engine_shaders/passes/text2d_pass.ps");
                ShaderManager::BuildShader(shader);

                vector<RenderPassBinding> bindings = {
                    { "Vertex3D", RenderPassBinding::eType::VERTEX_3D },
                    { "Shader", RenderPassBinding::eType::SHADER, shader },
                    { "CameraBuffer", RenderPassBinding::eType::BUFFER, m_Renderer->CameraBuffer, RenderPassBinding::eStage::VERTEX, RenderPassBinding::SLOT_DEFAULT },
                    { "ViewportBuffer", RenderPassBinding::eType::BUFFER, m_Canvas->GetBuffer(), RenderPassBinding::eStage::VERTEX, RenderPassBinding::SLOT_DEFAULT }
                };

                m_Renderer->AddRenderPass<Text2DPass>(
                    bindings,
                    rendererRT
                );
            }

            // Text 3D pass
            {
                Shader* shader = ShaderManager::CreateShader("text3d");
                ShaderManager::AddVertexStageFromFile(shader, "engine_shaders/passes/text3d_pass.vs");
                ShaderManager::AddPixelStageFromFile(shader, "engine_shaders/passes/text3d_pass.ps");
                ShaderManager::BuildShader(shader);

                vector<RenderPassBinding> bindings = {
                    { "Vertex3D", RenderPassBinding::eType::VERTEX_3D },
                    { "Shader", RenderPassBinding::eType::SHADER, shader },
                    { "CameraBuffer", RenderPassBinding::eType::BUFFER, m_Renderer->CameraBuffer, RenderPassBinding::eStage::VERTEX, RenderPassBinding::SLOT_DEFAULT },
                };

                m_Renderer->AddRenderPass<Text3DPass>(
                    bindings,
                    rendererRT
                );
            }

            // SSAO pass
            {
                Shader* shader = ShaderManager::CreateShader("ssao");
                ShaderManager::AddVertexStageFromFile(shader, "engine_shaders/passes/ssao_pass.vs");
                if (m_UseMSAA == core::K_FALSE) {
                    ShaderManager::AddPixelStageFromFile(shader, "engine_shaders/passes/ssao_pass.ps");
                } else if (m_UseMSAA == core::K_TRUE) {
                    ShaderManager::AddPixelStageFromFile(shader, "engine_shaders/passes/msaa/ssao_pass.ps");
                }
                ShaderManager::BuildShader(shader);

                vector<RenderPassBinding> bindings = {
                    { "Vertex3D", RenderPassBinding::eType::VERTEX_3D },
                    { "Shader", RenderPassBinding::eType::SHADER, shader },
                    { "PositionTexture", RenderPassBinding::eType::TEXTURE, rendererRT->Colors[3], RenderPassBinding::eStage::PIXEL, 0 },
                    { "NormalTexture", RenderPassBinding::eType::TEXTURE, rendererRT->Colors[4], RenderPassBinding::eStage::PIXEL, 1 },
                    { "DepthTexture", RenderPassBinding::eType::TEXTURE, rendererRT->DepthStencil, RenderPassBinding::eStage::PIXEL, 2 }
                };

                m_SSAOPass = m_Renderer->AddRenderPass<SSAOPass>(
                    bindings,
                    canvasViewport,
                    m_UseMSAA,
                    m_MSAASampleCount
                );
            }

            // Merge pass
            {
                Shader* shader = ShaderManager::CreateShader("merge");
                ShaderManager::AddVertexStageFromFile(shader, "engine_shaders/passes/merge_pass.vs");
                if (m_UseMSAA == core::K_FALSE) {
                    ShaderManager::AddPixelStageFromFile(shader, "engine_shaders/passes/merge_pass.ps");
                } else if(m_UseMSAA == core::K_TRUE) {
                    ShaderManager::AddPixelStageFromFile(shader, "engine_shaders/passes/msaa/merge_pass.ps");
                }
                ShaderManager::BuildShader(shader);

                vector<RenderPassBinding> bindings = {
                    { "Shader", RenderPassBinding::eType::SHADER, shader },
                    { "ColorTexture", RenderPassBinding::eType::TEXTURE, rendererRT->Colors[0], RenderPassBinding::eStage::PIXEL, 0 },
                    { "AOTexture", RenderPassBinding::eType::TEXTURE, m_SSAOPass->GetRenderTarget()->Colors[0], RenderPassBinding::eStage::PIXEL, 3}
                };

                m_Renderer->AddRenderPass<MergePass>(
                    bindings,
                    canvasRT
                );
            }
        }

        void Application::Render()
        {
            m_Renderer->GetRenderTarget()->ClearColor(0, glm::vec4(0.0f));
            m_Renderer->GetRenderTarget()->ClearColor(1, glm::vec4(0.0f));
            m_Renderer->GetRenderTarget()->ClearColor(2, glm::vec4(0.0f));
            m_Renderer->GetRenderTarget()->ClearColor(3, glm::vec4(0.0f));
            m_Renderer->GetRenderTarget()->ClearColor(4, glm::vec4(0.0f));
            m_Renderer->GetRenderTarget()->ClearDepth(1.0f);
            m_SSAOPass->GetRenderTarget()->ClearColor(0, glm::vec4(0.0f));
            m_SSAOPass->GetRenderTarget()->ClearDepth(1.0f);

            m_Canvas->Clear(ClearColor);
            m_Renderer->Render(m_MainScene);
            m_Canvas->Present();
        }

    }

}
