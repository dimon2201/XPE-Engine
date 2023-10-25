#include <core/app.hpp>
#include <core/dispatchers.h>

#include <ecs/scenes.hpp>

#include <rendering/core/debugger.h>

#include <rendering/render_system.h>

#include <rendering/passes/canvas.hpp>
#include <rendering/passes/main_pass.h>
#include <rendering/passes/skeletal_anim_pass.h>
#include <rendering/passes/text2d_pass.h>
#include <rendering/passes/text3d_pass.h>
#include <rendering/passes/merge_pass.h>
#include <rendering/passes/ssao_pass.hpp>

#include <anim/anim_system.h>

#include <audio/audio_system.h>

#include <physics/physics_system.h>

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
                    HardwareManager::CPU.Cores,
                    100,
                    "Worker",
                    Thread::ePriority::NORMAL
            );

            TaskManager::Init(mainDispatcher);
            PhysicsSystem::Init(mainDispatcher);

            WindowManager::Init();
            WindowManager::InitWindow(winDesc);

            InputManager::Init();

            render::context::EnableInfoLog = Config.EnableGPUInfoLog;
            render::context::EnableWarnLog = Config.EnableGPUWarnLog;
            render::context::EnableErrorLog = Config.EnableGPUErrorLog;

            m_RenderSystem = new RenderSystem();
            m_AnimSystem = new AnimSystem();
            m_AudioSystem = new AudioSystem();

            InitRenderPasses();

            WindowManager::SetMonitorBuffer(m_RenderSystem->GetMonitorBuffer());
            WindowManager::SetExposure(winDesc.Exposure);
            WindowManager::SetGamma(winDesc.Gamma);

            m_MainScene = new MainScene();
            m_MainScene->PerspectiveCamera->Buffer = m_RenderSystem->GetCameraBuffer();
            m_MainScene->OrthoCamera->Buffer = m_RenderSystem->GetCameraBuffer();

            Init();

            m_RenderSystem->Prepare();

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

                TimeManager cpuTimer(&CPUTime);
                TimeManager deltaTimer(&DeltaTime);

                CurrentTime = cpuTimer.GetStartTime();

                Update();

                // submit audio task with current scene state
                TaskManager::SubmitTask({[this]() {
                    m_AudioSystem->Update(m_MainScene, DeltaTime);
                    m_AudioSystem->UpdateListener(m_MainScene);
                }});

                // submit animation task with current scene state
                TaskManager::SubmitTask({[this]() {
                    m_AnimSystem->Update(m_MainScene, DeltaTime);
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

            Free();

            delete m_MainScene;

            delete m_AudioSystem;
            delete m_AnimSystem;
            delete m_Canvas;
            delete m_RenderSystem;

            PhysicsSystem::Free();

            InputManager::Free();

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

        void Application::InitRenderPasses()
        {
            // Canvas
            Shader* shader = ShaderManager::CreateShader("canvas");
            ShaderManager::AddVertexStageFromFile(shader, "engine_shaders/passes/canvas.vs");
            ShaderManager::AddPixelStageFromFile(shader, "engine_shaders/passes/canvas.ps");
            ShaderManager::BuildShader(shader);
            m_Canvas = new Canvas(WindowManager::GetWidth(), WindowManager::GetHeight(), shader);

            RenderTarget* canvasRT = m_Canvas->GetRenderTarget();
            Viewport* canvasViewport = m_Canvas->GetViewport(0);

            m_RenderSystem->InitRenderTargets(canvasViewport, Config.MsaaSampleCount);

            RenderTarget* opaqueRT = m_RenderSystem->GetOpaqueRT();
            RenderTarget* transparentRT = m_RenderSystem->GetTransparentRT();

            // Main opaque pass
            {
                Shader* shader = ShaderManager::CreateShader("main_opaque");
                ShaderManager::AddVertexStageFromFile(shader, "engine_shaders/passes/main_pass.vs");
                ShaderManager::AddPixelStageFromFile(shader, "engine_shaders/passes/pbr_pass_opaque.ps");
                ShaderManager::BuildShader(shader);

                vector<RenderPassBinding> bindings = {
                    { "Shader", RenderPassBinding::eType::SHADER, shader },
                    { "RenderTarget", RenderPassBinding::eType::RENDER_TARGET, opaqueRT },
                    { "CameraBuffer", RenderPassBinding::eType::BUFFER, m_RenderSystem->GetCameraBuffer(), RenderPassBinding::eStage::VERTEX, RenderPassBinding::SLOT_DEFAULT },
                    { "DirectLightBuffer", RenderPassBinding::eType::BUFFER, m_RenderSystem->GetDirectLightBuffer(), RenderPassBinding::eStage::PIXEL,  RenderPassBinding::SLOT_DEFAULT },
                    { "PointLightBuffer", RenderPassBinding::eType::BUFFER, m_RenderSystem->GetPointLightBuffer(), RenderPassBinding::eStage::PIXEL,  RenderPassBinding::SLOT_DEFAULT },
                    { "SpotLightBuffer", RenderPassBinding::eType::BUFFER, m_RenderSystem->GetSpotLightBuffer(), RenderPassBinding::eStage::PIXEL,  RenderPassBinding::SLOT_DEFAULT },
                    { "MonitorBuffer", RenderPassBinding::eType::BUFFER, m_RenderSystem->GetMonitorBuffer(), RenderPassBinding::eStage::PIXEL,  RenderPassBinding::SLOT_DEFAULT },
                    { "ShadowFilterBuffer", RenderPassBinding::eType::BUFFER, m_RenderSystem->GetShadowFilterBuffer(), RenderPassBinding::eStage::PIXEL,  RenderPassBinding::SLOT_DEFAULT }
                };

                m_RenderSystem->AddRenderPass<MainPass>(RenderPass::eType::OPAQUE, bindings);
            }

            // Main transparent pass
            {
                Shader* shader = ShaderManager::CreateShader("main_transparent");
                ShaderManager::AddVertexStageFromFile(shader, "engine_shaders/passes/main_pass.vs");
                ShaderManager::AddPixelStageFromFile(shader, "engine_shaders/passes/pbr_pass_transparent.ps");
                ShaderManager::BuildShader(shader);

                vector<RenderPassBinding> bindings = {
                        { "Shader", RenderPassBinding::eType::SHADER, shader },
                        { "RenderTarget", RenderPassBinding::eType::RENDER_TARGET, transparentRT },
                        { "CameraBuffer", RenderPassBinding::eType::BUFFER, m_RenderSystem->GetCameraBuffer(), RenderPassBinding::eStage::VERTEX, RenderPassBinding::SLOT_DEFAULT },
                        { "DirectLightBuffer", RenderPassBinding::eType::BUFFER, m_RenderSystem->GetDirectLightBuffer(), RenderPassBinding::eStage::PIXEL,  RenderPassBinding::SLOT_DEFAULT },
                        { "PointLightBuffer", RenderPassBinding::eType::BUFFER, m_RenderSystem->GetPointLightBuffer(), RenderPassBinding::eStage::PIXEL,  RenderPassBinding::SLOT_DEFAULT },
                        { "SpotLightBuffer", RenderPassBinding::eType::BUFFER, m_RenderSystem->GetSpotLightBuffer(), RenderPassBinding::eStage::PIXEL,  RenderPassBinding::SLOT_DEFAULT },
                        { "MonitorBuffer", RenderPassBinding::eType::BUFFER, m_RenderSystem->GetMonitorBuffer(), RenderPassBinding::eStage::PIXEL,  RenderPassBinding::SLOT_DEFAULT },
                        { "ShadowFilterBuffer", RenderPassBinding::eType::BUFFER, m_RenderSystem->GetShadowFilterBuffer(), RenderPassBinding::eStage::PIXEL,  RenderPassBinding::SLOT_DEFAULT }
                };

                m_RenderSystem->AddRenderPass<MainPass>(RenderPass::eType::TRANSPARENT, bindings);
            }

            // Skeletal animation opaque pass
            {
                Shader* shader = ShaderManager::CreateShader("skeletal_anim_opaque");
                ShaderManager::AddVertexStageFromFile(shader, "engine_shaders/passes/skeletal_anim_pass.vs");
                ShaderManager::AddPixelStageFromFile(shader, "engine_shaders/passes/pbr_pass_opaque.ps");
                ShaderManager::BuildShader(shader);

                vector<RenderPassBinding> bindings = {
                    { "Shader", RenderPassBinding::eType::SHADER, shader },
                    { "RenderTarget", RenderPassBinding::eType::RENDER_TARGET, opaqueRT },
                    { "CameraBuffer", RenderPassBinding::eType::BUFFER, m_RenderSystem->GetCameraBuffer(), RenderPassBinding::eStage::VERTEX, RenderPassBinding::SLOT_DEFAULT },
                    { "DirectLightBuffer", RenderPassBinding::eType::BUFFER, m_RenderSystem->GetDirectLightBuffer(), RenderPassBinding::eStage::PIXEL,  RenderPassBinding::SLOT_DEFAULT },
                    { "PointLightBuffer", RenderPassBinding::eType::BUFFER, m_RenderSystem->GetPointLightBuffer(), RenderPassBinding::eStage::PIXEL,  RenderPassBinding::SLOT_DEFAULT },
                    { "SpotLightBuffer", RenderPassBinding::eType::BUFFER, m_RenderSystem->GetSpotLightBuffer(), RenderPassBinding::eStage::PIXEL,  RenderPassBinding::SLOT_DEFAULT },
                    { "MonitorBuffer", RenderPassBinding::eType::BUFFER, m_RenderSystem->GetMonitorBuffer(), RenderPassBinding::eStage::PIXEL,  RenderPassBinding::SLOT_DEFAULT },
                    { "ShadowFilterBuffer", RenderPassBinding::eType::BUFFER, m_RenderSystem->GetShadowFilterBuffer(), RenderPassBinding::eStage::PIXEL,  RenderPassBinding::SLOT_DEFAULT }
                };

                m_RenderSystem->AddRenderPass<SkeletalAnimPass>(RenderPass::eType::OPAQUE, bindings);
            }

            // Skeletal animation transparent pass
            {
                Shader* shader = ShaderManager::CreateShader("skeletal_anim_transparent");
                ShaderManager::AddVertexStageFromFile(shader, "engine_shaders/passes/skeletal_anim_pass.vs");
                ShaderManager::AddPixelStageFromFile(shader, "engine_shaders/passes/pbr_pass_transparent.ps");
                ShaderManager::BuildShader(shader);

                vector<RenderPassBinding> bindings = {
                        { "Shader", RenderPassBinding::eType::SHADER, shader },
                        { "RenderTarget", RenderPassBinding::eType::RENDER_TARGET, transparentRT },
                        { "CameraBuffer", RenderPassBinding::eType::BUFFER, m_RenderSystem->GetCameraBuffer(), RenderPassBinding::eStage::VERTEX, RenderPassBinding::SLOT_DEFAULT },
                        { "DirectLightBuffer", RenderPassBinding::eType::BUFFER, m_RenderSystem->GetDirectLightBuffer(), RenderPassBinding::eStage::PIXEL,  RenderPassBinding::SLOT_DEFAULT },
                        { "PointLightBuffer", RenderPassBinding::eType::BUFFER, m_RenderSystem->GetPointLightBuffer(), RenderPassBinding::eStage::PIXEL,  RenderPassBinding::SLOT_DEFAULT },
                        { "m_SpotLightBuffer", RenderPassBinding::eType::BUFFER, m_RenderSystem->GetSpotLightBuffer(), RenderPassBinding::eStage::PIXEL,  RenderPassBinding::SLOT_DEFAULT },
                        { "MonitorBuffer", RenderPassBinding::eType::BUFFER, m_RenderSystem->GetMonitorBuffer(), RenderPassBinding::eStage::PIXEL,  RenderPassBinding::SLOT_DEFAULT },
                        { "ShadowFilterBuffer", RenderPassBinding::eType::BUFFER, m_RenderSystem->GetShadowFilterBuffer(), RenderPassBinding::eStage::PIXEL,  RenderPassBinding::SLOT_DEFAULT }
                };

                m_RenderSystem->AddRenderPass<SkeletalAnimPass>(RenderPass::eType::TRANSPARENT, bindings);
            }

            // Text 2D pass
            {
                Shader* shader = ShaderManager::CreateShader("text2d");
                ShaderManager::AddVertexStageFromFile(shader, "engine_shaders/passes/text2d_pass.vs");
                ShaderManager::AddPixelStageFromFile(shader, "engine_shaders/passes/text2d_pass.ps");
                ShaderManager::BuildShader(shader);

                vector<RenderPassBinding> bindings = {
                    { "Shader", RenderPassBinding::eType::SHADER, shader },
                    { "RenderTarget", RenderPassBinding::eType::RENDER_TARGET, opaqueRT },
                    { "CameraBuffer", RenderPassBinding::eType::BUFFER, m_RenderSystem->GetCameraBuffer(), RenderPassBinding::eStage::VERTEX, RenderPassBinding::SLOT_DEFAULT },
                    { "ViewportBuffer", RenderPassBinding::eType::BUFFER, m_Canvas->GetBuffer(), RenderPassBinding::eStage::VERTEX, RenderPassBinding::SLOT_DEFAULT }
                };

                m_RenderSystem->AddRenderPass<Text2DPass>(RenderPass::eType::OPAQUE, bindings);
            }

            // Text 3D pass
            {
                Shader* shader = ShaderManager::CreateShader("text3d");
                ShaderManager::AddVertexStageFromFile(shader, "engine_shaders/passes/text3d_pass.vs");
                ShaderManager::AddPixelStageFromFile(shader, "engine_shaders/passes/text3d_pass.ps");
                ShaderManager::BuildShader(shader);

                vector<RenderPassBinding> bindings = {
                    { "Shader", RenderPassBinding::eType::SHADER, shader },
                    { "RenderTarget", RenderPassBinding::eType::RENDER_TARGET, opaqueRT },
                    { "CameraBuffer", RenderPassBinding::eType::BUFFER, m_RenderSystem->GetCameraBuffer(), RenderPassBinding::eStage::VERTEX, RenderPassBinding::SLOT_DEFAULT },
                };

                m_RenderSystem->AddRenderPass<Text3DPass>(RenderPass::eType::OPAQUE, bindings);
            }

            // SSAO pass
            {
                Shader* shader = ShaderManager::CreateShader("ssao");
                ShaderManager::AddVertexStageFromFile(shader, "engine_shaders/passes/ssao_pass.vs");
                ShaderManager::AddPixelStageFromFile(shader, "engine_shaders/passes/ssao_pass.ps");
                ShaderManager::BuildShader(shader);

                vector<RenderPassBinding> bindings = {
                    { "Shader", RenderPassBinding::eType::SHADER, shader },
                    { "PositionTexture", RenderPassBinding::eType::TEXTURE, opaqueRT->Colors[1], RenderPassBinding::eStage::PIXEL, 0 },
                    { "NormalTexture", RenderPassBinding::eType::TEXTURE, opaqueRT->Colors[2], RenderPassBinding::eStage::PIXEL, 1 },
                    { "DepthTexture", RenderPassBinding::eType::TEXTURE, opaqueRT->DepthStencil, RenderPassBinding::eStage::PIXEL, 2 }
                };

                m_SsaoPass = m_RenderSystem->AddRenderPass<SSAOPass>(bindings, canvasViewport);
            }

            RenderTarget* ssaoRT = m_SsaoPass->GetRenderTarget();

            // SSAO Merge pass
            {
                Shader* shader = ShaderManager::CreateShader("merge");
                ShaderManager::AddVertexStageFromFile(shader, "engine_shaders/passes/merge_pass.vs");
                ShaderManager::AddPixelStageFromFile(shader, "engine_shaders/passes/merge_pass.ps");
                ShaderManager::BuildShader(shader);

                vector<RenderPassBinding> bindings = {
                    { "Shader", RenderPassBinding::eType::SHADER, shader },
                    { "RenderTarget", RenderPassBinding::eType::RENDER_TARGET, canvasRT },
                    { "ColorTexture", RenderPassBinding::eType::TEXTURE, opaqueRT->Colors[0], RenderPassBinding::eStage::PIXEL, 0 },
                    { "AOTexture", RenderPassBinding::eType::TEXTURE, ssaoRT->Colors[0], RenderPassBinding::eStage::PIXEL, 2 }
                };

                m_RenderSystem->AddRenderPass<MergePass>(bindings);
            }
        }

        void Application::Render()
        {
            m_SsaoPass->GetRenderTarget()->ClearColor(0, { 1, 1, 1, 1 });
            m_SsaoPass->GetRenderTarget()->ClearDepth(1);
            m_Canvas->Clear(ClearColor);
            m_RenderSystem->Update(m_MainScene, DeltaTime);
            m_Canvas->Present();
        }

    }

}
