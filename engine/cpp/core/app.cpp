#include <core/app.hpp>

#include <rendering/core/debugger.h>

#include <rendering/render_system.h>

#include <rendering/passes/canvas.hpp>
#include <rendering/passes/skybox_pass.h>
#include <rendering/passes/geometry_pass.h>
#include <rendering/passes/widget_pass.h>
#include <rendering/passes/composite_transparent_pass.h>
#include <rendering/passes/final_pass.h>

#include <anim/anim_system.h>

#include <audio/audio_system.h>

#include <physics/physics_manager.h>
#include <physics/physics_system.h>

namespace xpe {

    using namespace render;
    using namespace ecs;
    using namespace anim;
    using namespace audio;
    using namespace physics;

    namespace core {

        void cApp::Run()
        {
            sLoggerDescriptor logDesc;
            logDesc.Name = Config.LogTitle.c_str();
            logDesc.Backtrace = Config.LogBacktrace;
            InitLogger(logDesc);

            sWindowDescriptor winDesc;
            winDesc.Title = Config.WinTitle.c_str();
            winDesc.Width = Config.WinWidth;
            winDesc.Height = Config.WinHeight;
            winDesc.X = Config.WinX;
            winDesc.Y = Config.WinY;
            winDesc.VSync = Config.VSync;
            winDesc.Fullscreen = Config.Fullscreen;
            winDesc.Gamma = Config.Gamma;
            winDesc.Exposure = Config.Exposure;

            DeltaTime.SetFps(Config.FPS);
            CPUTime = DeltaTime;

            cTaskManager::Init();
            cEventManager::Init();
            cWindowManager::Init();
            cWindowManager::InitWindow(winDesc);
            cInputManager::Init();
            cPhysicsManager::Init(cTaskManager::GetSimulationDispatcher());

            render::context::EnableInfoLog = Config.EnableGPUInfoLog;
            render::context::EnableWarnLog = Config.EnableGPUWarnLog;
            render::context::EnableErrorLog = Config.EnableGPUErrorLog;

            m_Viewport.Width = cWindowManager::GetWidth();
            m_Viewport.Height = cWindowManager::GetHeight();

            m_RenderSystem = new cRenderSystem(m_Viewport, Config.MsaaSampleCount);
            m_AnimSystem = new cAnimSystem();
            m_AudioSystem = new cAudioSystem();
            m_PhysicsSystem = new cPhysicsSystem();

            InitRenderPasses();

            cCameraManager::SetExposure(winDesc.Exposure);
            cCameraManager::SetGamma(winDesc.Gamma);

            m_SsaoPass->Enable = Config.EnableSSAO;

            m_Scene = new cScene();

            Init();

            m_RenderSystem->Prepare();

            while (m_Opened)
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

                cTimeManager cpuTimer(&CPUTime);
                cTimeManager deltaTimer(&DeltaTime);

                CurrentTime = cpuTimer.GetStartTime();

                Update();

                // submit audio task with current scene state
                cTaskManager::SubmitTask({eTaskType::AUDIO, [this]() {
                    m_AudioSystem->Update(m_Scene, DeltaTime);
                    m_AudioSystem->UpdateListener(m_Scene);
                }});

                // submit animation task with current scene state
                cTaskManager::SubmitTask({eTaskType::ANIMATION, [this]() {
                    m_AnimSystem->Update(m_Scene, DeltaTime);
                }});

                // calls physics simulation that will be automatically dispatched into TaskManager
                // we don't need to submit physics system into task
                m_PhysicsSystem->Update(m_Scene, DeltaTime);

                ClearRenderPasses();
                Render();

                cWindowManager::PollEvents();
                cWindowManager::Swap();

                m_Opened = !cWindowManager::ShouldClose();

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

            delete m_Scene;

            delete m_AudioSystem;
            delete m_AnimSystem;
            delete m_Canvas;
            delete m_RenderSystem;

            cPhysicsManager::Free();
            cInputManager::Free();
            cWindowManager::FreeWindow();
            cWindowManager::Free();
            cEventManager::Free();
            cTaskManager::Free();

            FreeLogger();
        }

        void cApp::LockFPSFromConfig()
        {
            if (Config.LockOnFps && Config.FPS < DeltaTime.Fps())
            {
                DeltaTime.SetFps(Config.FPS);
            }
        }

        void cApp::InitRenderPasses()
        {
            sRenderTarget* finalRT = m_RenderSystem->GetFinalRT();
            sRenderTarget* sceneRT = m_RenderSystem->GetSceneRT();
            sRenderTarget* shadowRT = m_RenderSystem->GetShadowRT();
            sRenderTarget* opaqueRT = m_RenderSystem->GetOpaqueRT();
            sRenderTarget* transparentRT = m_RenderSystem->GetTransparentRT();
            sRenderTarget* uiRT = m_RenderSystem->GetUiRT();
            m_CanvasTexture = finalRT->Colors[0];

            // cCanvas
            {
                sShader* shader = cShaderManager::CreateShader("canvas");
                cShaderManager::AddVertexStageFromFile(shader, "engine_shaders/passes/screen.vs");
                cShaderManager::AddPixelStageFromFile(shader, "engine_shaders/passes/canvas.ps");
                cShaderManager::BuildShader(shader);
                m_Canvas = new cCanvas(shader, m_CanvasTexture, cCameraManager::GetViewport());
            }

            // sSkybox pass
            {
                sShader* shader = cShaderManager::CreateShader("skybox_pass");
                cShaderManager::AddVertexStageFromFile(shader, "engine_shaders/passes/skybox_pass.vs");
                cShaderManager::AddPixelStageFromFile(shader, "engine_shaders/passes/skybox_pass.ps");
                cShaderManager::BuildShader(shader);

                vector<sRenderPassBinding> bindings = {
                        { "Shader",       sRenderPassBinding::eType::SHADER,        shader },
                        { "RenderTarget", sRenderPassBinding::eType::RENDER_TARGET, opaqueRT },
                        { "CameraBuffer", sRenderPassBinding::eType::BUFFER,   cCameraManager::GetBuffer(), sShaderStage::eType::VERTEX, K_SLOT_DEFAULT },
                };

                m_RenderSystem->AddRenderPass<cSkyboxPass>(bindings);
            }

            // Geometry opaque pass
            {
                sShader* shader = cShaderManager::CreateShader("geometry_opaque_pass");
                cShaderManager::AddVertexStageFromFile(shader, "engine_shaders/passes/geometry_pass.vs");
                cShaderManager::AddPixelStageFromFile(shader, "engine_shaders/passes/pbr_pass_opaque.ps");
                cShaderManager::BuildShader(shader);

                vector<sRenderPassBinding> bindings = {
                    { "Shader",             sRenderPassBinding::eType::SHADER,        shader },
                    { "RenderTarget",       sRenderPassBinding::eType::RENDER_TARGET, opaqueRT },
                    { "CameraBuffer",       sRenderPassBinding::eType::BUFFER,   cCameraManager::GetBuffer(),            sShaderStage::eType::VERTEX, K_SLOT_DEFAULT },
                    { "DirectLightBuffer",  sRenderPassBinding::eType::BUFFER,   m_RenderSystem->GetDirectLightBuffer(), sShaderStage::eType::PIXEL,  K_SLOT_DEFAULT },
                    { "DirectLightMatrixBuffer", sRenderPassBinding::eType::BUFFER,  m_RenderSystem->GetDirectLightMatrixBuffer(), sShaderStage::eType::VERTEX,  K_SLOT_DEFAULT },
                    { "PointLightBuffer",   sRenderPassBinding::eType::BUFFER,        m_RenderSystem->GetPointLightBuffer(),  sShaderStage::eType::PIXEL,  K_SLOT_DEFAULT },
                    { "SpotLightBuffer",    sRenderPassBinding::eType::BUFFER,        m_RenderSystem->GetSpotLightBuffer(),   sShaderStage::eType::PIXEL,  K_SLOT_DEFAULT },
                    { "ShadowAtlas",        sRenderPassBinding::eType::TEXTURE,       m_RenderSystem->GetShadowRT()->Colors[0], sShaderStage::eType::PIXEL,K_SLOT_DEFAULT },
                    { "ShadowSampler",      sRenderPassBinding::eType::SAMPLER,       cShadowManager::GetSampler(),           sShaderStage::eType::PIXEL,  K_SLOT_DEFAULT },
                    { "ShadowPcfBuffer",    sRenderPassBinding::eType::BUFFER,        cShadowManager::GetBuffer(),            sShaderStage::eType::PIXEL,  K_SLOT_DEFAULT },
                };

                m_RenderSystem->AddRenderPass<cGeometryPass>(cRenderPass::eType::OPAQUE, bindings);
            }

            // Geometry transparent pass
            {
                sShader* shader = cShaderManager::CreateShader("geometry_transparent_pass");
                cShaderManager::AddVertexStageFromFile(shader, "engine_shaders/passes/geometry_pass.vs");
                cShaderManager::AddPixelStageFromFile(shader, "engine_shaders/passes/pbr_pass_transparent.ps");
                cShaderManager::BuildShader(shader);

                vector<sRenderPassBinding> bindings = {
                        { "Shader",             sRenderPassBinding::eType::SHADER,        shader },
                        { "RenderTarget",       sRenderPassBinding::eType::RENDER_TARGET, transparentRT },
                        { "CameraBuffer",       sRenderPassBinding::eType::BUFFER,   cCameraManager::GetBuffer(),            sShaderStage::eType::VERTEX, K_SLOT_DEFAULT },
                        { "DirectLightBuffer",  sRenderPassBinding::eType::BUFFER,   m_RenderSystem->GetDirectLightBuffer(), sShaderStage::eType::PIXEL,  K_SLOT_DEFAULT },
                        { "DirectLightMatrixBuffer",  sRenderPassBinding::eType::BUFFER, m_RenderSystem->GetDirectLightMatrixBuffer(), sShaderStage::eType::VERTEX,  K_SLOT_DEFAULT },
                        { "PointLightBuffer",   sRenderPassBinding::eType::BUFFER,   m_RenderSystem->GetPointLightBuffer(),  sShaderStage::eType::PIXEL,  K_SLOT_DEFAULT },
                        { "SpotLightBuffer",    sRenderPassBinding::eType::BUFFER,   m_RenderSystem->GetSpotLightBuffer(),   sShaderStage::eType::PIXEL,  K_SLOT_DEFAULT },
                        { "ShadowAtlas",        sRenderPassBinding::eType::TEXTURE,       m_RenderSystem->GetShadowRT()->Colors[0], sShaderStage::eType::PIXEL,  K_SLOT_DEFAULT },
                        { "ShadowSampler",      sRenderPassBinding::eType::SAMPLER,       cShadowManager::GetSampler(),           sShaderStage::eType::PIXEL,  K_SLOT_DEFAULT },
                        { "ShadowPcfBuffer",    sRenderPassBinding::eType::BUFFER,   cShadowManager::GetBuffer(),            sShaderStage::eType::PIXEL,  K_SLOT_DEFAULT },
                };

                m_RenderSystem->AddRenderPass<cGeometryPass>(cRenderPass::eType::TRANSPARENT, bindings);
            }

            // Geometry shadow pass
            {
                sShader* shader = cShaderManager::CreateShader("geometry_shadow_pass");
                cShaderManager::AddVertexStageFromFile(shader, "engine_shaders/passes/shadow_pass.vs");
                cShaderManager::AddPixelStageFromFile(shader, "engine_shaders/passes/shadow_pass.ps");
                cShaderManager::BuildShader(shader);

                vector<sRenderPassBinding> bindings = {
                        { "Shader",                   sRenderPassBinding::eType::SHADER,        shader },
                        { "RenderTarget",             sRenderPassBinding::eType::RENDER_TARGET, shadowRT },
                        { "DirectLightMatrixBuffer",  sRenderPassBinding::eType::BUFFER,   m_RenderSystem->GetDirectLightMatrixBuffer(), sShaderStage::eType::VERTEX,  K_SLOT_DEFAULT }
                };

                m_RenderSystem->AddRenderPass<cGeometryPass>(cRenderPass::eType::SHADOW, bindings);
            }

            // Widget pass
            {
                sShader* shader = cShaderManager::CreateShader("widget_pass");
                cShaderManager::AddVertexStageFromFile(shader, "engine_shaders/passes/widget_pass.vs");
                cShaderManager::AddPixelStageFromFile(shader, "engine_shaders/passes/widget_pass.ps");
                cShaderManager::BuildShader(shader);

                vector<sRenderPassBinding> bindings = {
                        { "Shader",         sRenderPassBinding::eType::SHADER,        shader },
                        { "RenderTarget",   sRenderPassBinding::eType::RENDER_TARGET, uiRT }
                };

                m_RenderSystem->AddRenderPass<cWidgetPass>(bindings);
            }

            // Text pass
            {
                sShader* shader = cShaderManager::CreateShader("text_pass");
                cShaderManager::AddVertexStageFromFile(shader, "engine_shaders/passes/text_pass.vs");
                cShaderManager::AddPixelStageFromFile(shader, "engine_shaders/passes/text_pass.ps");
                cShaderManager::BuildShader(shader);

                vector<sRenderPassBinding> bindings = {
                        { "Shader",         sRenderPassBinding::eType::SHADER,        shader },
                        { "RenderTarget",   sRenderPassBinding::eType::RENDER_TARGET, uiRT }
                };

                m_RenderSystem->AddRenderPass<cTextPass>(bindings);
            }

            // SSAO pass
            {
                sShader* shader = cShaderManager::CreateShader("ssao_pass");
                cShaderManager::AddVertexStageFromFile(shader, "engine_shaders/passes/screen.vs");
                cShaderManager::AddPixelStageFromFile(shader, Config.MsaaSampleCount > 1 ? "engine_shaders/passes/msaa/ssao_pass.ps" : "engine_shaders/passes/ssao_pass.ps");
                cShaderManager::BuildShader(shader);

                vector<sRenderPassBinding> bindings = {
                    { "Shader",          sRenderPassBinding::eType::SHADER,  shader },
                    { "PositionTexture", sRenderPassBinding::eType::TEXTURE, opaqueRT->Colors[1],    sShaderStage::eType::PIXEL, 1 },
                    { "NormalTexture",   sRenderPassBinding::eType::TEXTURE, opaqueRT->Colors[2],    sShaderStage::eType::PIXEL, 2 },
                    { "DepthTexture",    sRenderPassBinding::eType::TEXTURE, opaqueRT->DepthStencil, sShaderStage::eType::PIXEL, 3 }
                };

                m_SsaoPass = m_RenderSystem->AddRenderPass<cSsaoPass>(bindings, &m_Viewport, Config.MsaaSampleCount);
            }

            // Composite transparent pass
            {
                sShader* shader = cShaderManager::CreateShader("composite_pass_transparent");
                cShaderManager::AddVertexStageFromFile(shader, "engine_shaders/passes/screen.vs");
                cShaderManager::AddPixelStageFromFile(shader, Config.MsaaSampleCount > 1 ? "engine_shaders/passes/msaa/composite_pass_transparent.ps" : "engine_shaders/passes/composite_pass_transparent.ps");
                cShaderManager::BuildShader(shader);

                vector<sRenderPassBinding> bindings = {
                        { "Shader",        sRenderPassBinding::eType::SHADER,         shader },
                        { "RenderTarget",  sRenderPassBinding::eType::RENDER_TARGET,  opaqueRT },
                        { "AccumTexture",  sRenderPassBinding::eType::TEXTURE,       transparentRT->Colors[0], sShaderStage::eType::PIXEL, 0 },
                        { "RevealTexture", sRenderPassBinding::eType::TEXTURE,       transparentRT->Colors[1], sShaderStage::eType::PIXEL, 1 }
                };

                m_RenderSystem->AddRenderPass<cCompositeTransparentPass>(bindings);
            }

            // Final pass
            {
                sShader* shader = cShaderManager::CreateShader("final_pass");
                cShaderManager::AddVertexStageFromFile(shader, "engine_shaders/passes/screen.vs");
                cShaderManager::AddPixelStageFromFile(shader, Config.MsaaSampleCount > 1 ? "engine_shaders/passes/msaa/final_pass.ps" : "engine_shaders/passes/final_pass.ps");
                cShaderManager::BuildShader(shader);

                vector<sRenderPassBinding> bindings = {
                        { "Shader",       sRenderPassBinding::eType::SHADER,        shader },
                        { "RenderTarget", sRenderPassBinding::eType::RENDER_TARGET, finalRT },
                        { "CameraBuffer", sRenderPassBinding::eType::BUFFER,   cCameraManager::GetBuffer(), sShaderStage::eType::PIXEL, K_SLOT_DEFAULT },
                        { "SceneTexture", sRenderPassBinding::eType::TEXTURE,       opaqueRT->Colors[0], sShaderStage::eType::PIXEL, 0 },
                        { "AOTexture",    sRenderPassBinding::eType::TEXTURE,       m_SsaoPass->GetRenderTarget()->Colors[0], sShaderStage::eType::PIXEL, 1 },
                        { "UITexture",    sRenderPassBinding::eType::TEXTURE,       uiRT->Colors[0],    sShaderStage::eType::PIXEL, 2 }
                };

                m_RenderSystem->AddRenderPass<cFinalPass>(bindings);
            }
        }

        void cApp::ClearRenderPasses()
        {
            if (m_SsaoPass->Enable)
            {
                m_SsaoPass->GetRenderTarget()->ClearColor(0, { 1, 1, 1, 1 });
                m_SsaoPass->GetRenderTarget()->ClearDepth(1);
            }
        }

        void cApp::Render()
        {
            m_RenderSystem->Update(m_Scene, DeltaTime);
            m_Canvas->SetTexture(m_CanvasTexture);
            m_Canvas->Draw();
        }

    }

}
