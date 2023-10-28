#include <core/app.hpp>
#include <core/dispatchers.h>

#include <ecs/scenes.hpp>

#include <rendering/core/debugger.h>

#include <rendering/render_system.h>

#include <rendering/passes/canvas.hpp>
#include <rendering/passes/skybox_pass.h>
#include <rendering/passes/main_pass.h>
#include <rendering/passes/skeleton_pass.h>
#include <rendering/passes/text2d_pass.h>
#include <rendering/passes/text3d_pass.h>
#include <rendering/passes/composite_transparent_pass.h>
#include <rendering/passes/composite_ao_pass.h>
#include <rendering/passes/final_pass.h>

#include <anim/anim_system.h>

#include <audio/audio_system.h>

#include <physics/physics_manager.h>

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
            EventManager::Init();
            WindowManager::Init();
            WindowManager::InitWindow(winDesc);
            InputManager::Init();
            PhysicsManager::Init(mainDispatcher);

            render::context::EnableInfoLog = Config.EnableGPUInfoLog;
            render::context::EnableWarnLog = Config.EnableGPUWarnLog;
            render::context::EnableErrorLog = Config.EnableGPUErrorLog;

            m_Viewport.Width = WindowManager::GetWidth();
            m_Viewport.Height = WindowManager::GetHeight();

            m_RenderSystem = new RenderSystem(m_Viewport, Config.MsaaSampleCount);
            m_AnimSystem = new AnimSystem();
            m_AudioSystem = new AudioSystem();

            InitRenderPasses();

            WindowManager::SetMonitorBuffer(m_RenderSystem->GetMonitorBuffer());
            WindowManager::SetExposure(winDesc.Exposure);
            WindowManager::SetGamma(winDesc.Gamma);

            m_SsaoPass->Enable = Config.EnableSSAO;
            m_FxaaPass->Enable = Config.MsaaSampleCount == 1;

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

                ClearRenderPasses();
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

            PhysicsManager::Free();
            InputManager::Free();
            WindowManager::FreeWindow();
            WindowManager::Free();
            EventManager::Free();
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
            RenderTarget* finalRT = m_RenderSystem->GetFinalRT();
            RenderTarget* sceneRT = m_RenderSystem->GetSceneRT();
            RenderTarget* shadowRT = m_RenderSystem->GetShadowRT();
            RenderTarget* opaqueRT = m_RenderSystem->GetOpaqueRT();
            RenderTarget* transparentRT = m_RenderSystem->GetTransparentRT();
            RenderTarget* uiRT = m_RenderSystem->GetUiRT();

            // Canvas
            {
                Shader* shader = ShaderManager::CreateShader("canvas");
                ShaderManager::AddVertexStageFromFile(shader, "engine_shaders/passes/screen.vs");
                ShaderManager::AddPixelStageFromFile(shader, "engine_shaders/passes/canvas.ps");
                ShaderManager::BuildShader(shader);
                m_Canvas = new Canvas(shader, finalRT, m_RenderSystem->GetViewportBuffer());
            }

            // Skybox pass
            {
                Shader* shader = ShaderManager::CreateShader("skybox_pass");
                ShaderManager::AddVertexStageFromFile(shader, "engine_shaders/passes/skybox_pass.vs");
                ShaderManager::AddPixelStageFromFile(shader, "engine_shaders/passes/skybox_pass.ps");
                ShaderManager::BuildShader(shader);

                vector<RenderPassBinding> bindings = {
                        { "Shader", RenderPassBinding::eType::SHADER, shader },
                        { "RenderTarget", RenderPassBinding::eType::RENDER_TARGET, opaqueRT },
                        { "CameraBuffer", RenderPassBinding::eType::BUFFER, m_RenderSystem->GetCameraBuffer(), RenderPassBinding::eStage::VERTEX, RenderPassBinding::SLOT_DEFAULT },
                };

                m_RenderSystem->AddRenderPass<SkyboxPass>(bindings);
            }

            // Main opaque pass
            {
                Shader* shader = ShaderManager::CreateShader("main_opaque_pass");
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
                    { "ShadowFilterBuffer", RenderPassBinding::eType::BUFFER, m_RenderSystem->GetShadowFilterBuffer(), RenderPassBinding::eStage::PIXEL,  RenderPassBinding::SLOT_DEFAULT },
                    { "ShadowSampler", RenderPassBinding::eType::SAMPLER, m_RenderSystem->GetShadowSampler(), RenderPassBinding::eStage::PIXEL,  RenderPassBinding::SLOT_DEFAULT },
                    { "ShadowMap", RenderPassBinding::eType::TEXTURE, m_RenderSystem->GetShadowMap(), RenderPassBinding::eStage::PIXEL,  RenderPassBinding::SLOT_DEFAULT },
                };

                m_RenderSystem->AddRenderPass<MainPass>(RenderPass::eType::OPAQUE, bindings);
            }

            // Main transparent pass
            {
                Shader* shader = ShaderManager::CreateShader("main_transparent_pass");
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
                        { "ShadowFilterBuffer", RenderPassBinding::eType::BUFFER, m_RenderSystem->GetShadowFilterBuffer(), RenderPassBinding::eStage::PIXEL,  RenderPassBinding::SLOT_DEFAULT },
                        { "ShadowSampler", RenderPassBinding::eType::SAMPLER, m_RenderSystem->GetShadowSampler(), RenderPassBinding::eStage::PIXEL,  RenderPassBinding::SLOT_DEFAULT },
                        { "ShadowMap", RenderPassBinding::eType::TEXTURE, m_RenderSystem->GetShadowMap(), RenderPassBinding::eStage::PIXEL,  RenderPassBinding::SLOT_DEFAULT },
                };

                m_RenderSystem->AddRenderPass<MainPass>(RenderPass::eType::TRANSPARENT, bindings);
            }

            // Main shadow pass
            {
                Shader* shader = ShaderManager::CreateShader("main_shadow_pass");
                ShaderManager::AddVertexStageFromFile(shader, "engine_shaders/passes/main_pass_shadow.vs");
                ShaderManager::AddPixelStageFromFile(shader, "engine_shaders/passes/shadow_pass.ps");
                ShaderManager::BuildShader(shader);

                vector<RenderPassBinding> bindings = {
                        { "Shader", RenderPassBinding::eType::SHADER, shader },
                        { "RenderTarget", RenderPassBinding::eType::RENDER_TARGET, shadowRT },
                };

                m_RenderSystem->AddRenderPass<MainPass>(RenderPass::eType::SHADOW, bindings);
            }

            // Skeleton opaque pass
            {
                Shader* shader = ShaderManager::CreateShader("skeleton_opaque_pass");
                ShaderManager::AddVertexStageFromFile(shader, "engine_shaders/passes/skeleton_pass.vs");
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
                    { "ShadowFilterBuffer", RenderPassBinding::eType::BUFFER, m_RenderSystem->GetShadowFilterBuffer(), RenderPassBinding::eStage::PIXEL,  RenderPassBinding::SLOT_DEFAULT },
                    { "ShadowSampler", RenderPassBinding::eType::SAMPLER, m_RenderSystem->GetShadowSampler(), RenderPassBinding::eStage::PIXEL,  RenderPassBinding::SLOT_DEFAULT },
                    { "ShadowMap", RenderPassBinding::eType::TEXTURE, m_RenderSystem->GetShadowMap(), RenderPassBinding::eStage::PIXEL,  RenderPassBinding::SLOT_DEFAULT },
                };

                m_RenderSystem->AddRenderPass<SkeletonPass>(RenderPass::eType::OPAQUE, bindings);
            }

            // Skeleton transparent pass
            {
                Shader* shader = ShaderManager::CreateShader("skeleton_transparent_pass");
                ShaderManager::AddVertexStageFromFile(shader, "engine_shaders/passes/skeleton_pass.vs");
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
                        { "ShadowFilterBuffer", RenderPassBinding::eType::BUFFER, m_RenderSystem->GetShadowFilterBuffer(), RenderPassBinding::eStage::PIXEL,  RenderPassBinding::SLOT_DEFAULT },
                        { "ShadowSampler", RenderPassBinding::eType::SAMPLER, m_RenderSystem->GetShadowSampler(), RenderPassBinding::eStage::PIXEL,  RenderPassBinding::SLOT_DEFAULT },
                        { "ShadowMap", RenderPassBinding::eType::TEXTURE, m_RenderSystem->GetShadowMap(), RenderPassBinding::eStage::PIXEL,  RenderPassBinding::SLOT_DEFAULT },
                };

                m_RenderSystem->AddRenderPass<SkeletonPass>(RenderPass::eType::TRANSPARENT, bindings);
            }

            // Skeleton shadow pass
            {
                Shader* shader = ShaderManager::CreateShader("skeleton_shadow_pass");
                ShaderManager::AddVertexStageFromFile(shader, "engine_shaders/passes/skeleton_pass_shadow.vs");
                ShaderManager::AddPixelStageFromFile(shader, "engine_shaders/passes/shadow_pass.ps");
                ShaderManager::BuildShader(shader);

                vector<RenderPassBinding> bindings = {
                        { "Shader", RenderPassBinding::eType::SHADER, shader },
                        { "RenderTarget", RenderPassBinding::eType::RENDER_TARGET, shadowRT },
                };

                m_RenderSystem->AddRenderPass<SkeletonPass>(RenderPass::eType::SHADOW, bindings);
            }

            // Text 2D pass
            {
                Shader* shader = ShaderManager::CreateShader("text2d_pass");
                ShaderManager::AddVertexStageFromFile(shader, "engine_shaders/passes/text2d_pass.vs");
                ShaderManager::AddPixelStageFromFile(shader, "engine_shaders/passes/text2d_pass.ps");
                ShaderManager::BuildShader(shader);

                vector<RenderPassBinding> bindings = {
                    { "Shader", RenderPassBinding::eType::SHADER, shader },
                    { "RenderTarget", RenderPassBinding::eType::RENDER_TARGET, uiRT },
                    { "CameraBuffer", RenderPassBinding::eType::BUFFER, m_RenderSystem->GetCameraBuffer(), RenderPassBinding::eStage::VERTEX, RenderPassBinding::SLOT_DEFAULT },
                    { "ViewportBuffer", RenderPassBinding::eType::BUFFER, m_RenderSystem->GetViewportBuffer(), RenderPassBinding::eStage::VERTEX, RenderPassBinding::SLOT_DEFAULT }
                };

                m_RenderSystem->AddRenderPass<Text2DPass>(bindings);
            }

            // Text 3D pass
            {
                Shader* shader = ShaderManager::CreateShader("text3d_pass");
                ShaderManager::AddVertexStageFromFile(shader, "engine_shaders/passes/text3d_pass.vs");
                ShaderManager::AddPixelStageFromFile(shader, "engine_shaders/passes/text3d_pass.ps");
                ShaderManager::BuildShader(shader);

                vector<RenderPassBinding> bindings = {
                    { "Shader", RenderPassBinding::eType::SHADER, shader },
                    { "RenderTarget", RenderPassBinding::eType::RENDER_TARGET, opaqueRT },
                    { "CameraBuffer", RenderPassBinding::eType::BUFFER, m_RenderSystem->GetCameraBuffer(), RenderPassBinding::eStage::VERTEX, RenderPassBinding::SLOT_DEFAULT },
                };

                m_RenderSystem->AddRenderPass<Text3DPass>(bindings);
            }

            // FXAA pass
            {
                Shader* shader = ShaderManager::CreateShader("fxaa_pass");
                ShaderManager::AddVertexStageFromFile(shader, "engine_shaders/passes/screen.vs");
                ShaderManager::AddPixelStageFromFile(shader, "engine_shaders/passes/fxaa_pass.ps");
                ShaderManager::BuildShader(shader);

                vector<RenderPassBinding> bindings = {
                        { "Shader", RenderPassBinding::eType::SHADER, shader },
                        { "ColorTexture", RenderPassBinding::eType::TEXTURE, opaqueRT->Colors[0], RenderPassBinding::eStage::PIXEL, 0 },
                };

                m_FxaaPass = m_RenderSystem->AddRenderPass<FXAAPass>(bindings, &m_Viewport);
            }

            // SSAO pass
            {
                Shader* shader = ShaderManager::CreateShader("ssao_pass");
                ShaderManager::AddVertexStageFromFile(shader, "engine_shaders/passes/screen.vs");
                ShaderManager::AddPixelStageFromFile(shader, Config.MsaaSampleCount > 1 ? "engine_shaders/passes/msaa/ssao_pass.ps" : "engine_shaders/passes/ssao_pass.ps");
                ShaderManager::BuildShader(shader);

                vector<RenderPassBinding> bindings = {
                    { "Shader", RenderPassBinding::eType::SHADER, shader },
                    { "PositionTexture", RenderPassBinding::eType::TEXTURE, opaqueRT->Colors[1], RenderPassBinding::eStage::PIXEL, 1 },
                    { "NormalTexture", RenderPassBinding::eType::TEXTURE, opaqueRT->Colors[2], RenderPassBinding::eStage::PIXEL, 2 },
                    { "DepthTexture", RenderPassBinding::eType::TEXTURE, opaqueRT->DepthStencil, RenderPassBinding::eStage::PIXEL, 3 }
                };

                m_SsaoPass = m_RenderSystem->AddRenderPass<SSAOPass>(bindings, &m_Viewport, Config.MsaaSampleCount);
            }

            // Composite transparent pass
            {
                Shader* shader = ShaderManager::CreateShader("composite_pass_transparent");
                ShaderManager::AddVertexStageFromFile(shader, "engine_shaders/passes/screen.vs");
                ShaderManager::AddPixelStageFromFile(shader, Config.MsaaSampleCount > 1 ? "engine_shaders/passes/msaa/composite_pass_transparent.ps" : "engine_shaders/passes/composite_pass_transparent.ps");
                ShaderManager::BuildShader(shader);

                vector<RenderPassBinding> bindings = {
                        { "Shader", RenderPassBinding::eType::SHADER, shader },
                        { "RenderTarget", RenderPassBinding::eType::RENDER_TARGET, opaqueRT },
                        { "AccumTexture", RenderPassBinding::eType::TEXTURE, transparentRT->Colors[0], RenderPassBinding::eStage::PIXEL, 0 },
                        { "RevealTexture", RenderPassBinding::eType::TEXTURE, transparentRT->Colors[1], RenderPassBinding::eStage::PIXEL, 1 }
                };

                m_RenderSystem->AddRenderPass<CompositeTransparentPass>(bindings);
            }

            // Composite AO pass
            {
                Shader* shader = ShaderManager::CreateShader("composite_pass_ao");
                ShaderManager::AddVertexStageFromFile(shader, "engine_shaders/passes/screen.vs");
                ShaderManager::AddPixelStageFromFile(shader, Config.MsaaSampleCount > 1 ? "engine_shaders/passes/msaa/composite_pass_ao.ps" : "engine_shaders/passes/composite_pass_ao.ps");
                ShaderManager::BuildShader(shader);

                vector<RenderPassBinding> bindings = {
                    { "Shader", RenderPassBinding::eType::SHADER, shader },
                    { "RenderTarget", RenderPassBinding::eType::RENDER_TARGET, sceneRT },
                    { "ColorTexture", RenderPassBinding::eType::TEXTURE, Config.MsaaSampleCount > 1 ? opaqueRT->Colors[0] : m_FxaaPass->GetRenderTarget()->Colors[0], RenderPassBinding::eStage::PIXEL, 0 },
                    { "AOTexture", RenderPassBinding::eType::TEXTURE, m_SsaoPass->GetRenderTarget()->Colors[0], RenderPassBinding::eStage::PIXEL, 1 }
                };

                m_RenderSystem->AddRenderPass<CompositeAOPass>(bindings);
            }

            // Final pass
            {
                Shader* shader = ShaderManager::CreateShader("final_pass");
                ShaderManager::AddVertexStageFromFile(shader, "engine_shaders/passes/screen.vs");
                ShaderManager::AddPixelStageFromFile(shader, "engine_shaders/passes/final_pass.ps");
                ShaderManager::BuildShader(shader);

                vector<RenderPassBinding> bindings = {
                        { "Shader", RenderPassBinding::eType::SHADER, shader },
                        { "RenderTarget", RenderPassBinding::eType::RENDER_TARGET, finalRT },
                        { "SceneTexture", RenderPassBinding::eType::TEXTURE, sceneRT->Colors[0], RenderPassBinding::eStage::PIXEL, 0 },
                        { "UiTexture", RenderPassBinding::eType::TEXTURE, uiRT->Colors[0], RenderPassBinding::eStage::PIXEL, 1 }
                };

                m_RenderSystem->AddRenderPass<FinalPass>(bindings);
            }
        }

        void Application::ClearRenderPasses()
        {
            if (m_FxaaPass->Enable)
            {
                m_FxaaPass->GetRenderTarget()->ClearColor(0, { 1, 1, 1, 1 });
                m_FxaaPass->GetRenderTarget()->ClearDepth(1);
            }

            if (m_SsaoPass->Enable)
            {
                m_SsaoPass->GetRenderTarget()->ClearColor(0, { 1, 1, 1, 1 });
                m_SsaoPass->GetRenderTarget()->ClearDepth(1);
            }
        }

        void Application::Render()
        {
            m_RenderSystem->Update(m_MainScene, DeltaTime);
            m_Canvas->SetRenderTarget(m_RenderSystem->GetFinalRT());
            m_Canvas->Draw();
        }

    }

}
