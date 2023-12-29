#include <core/app.hpp>

#include <rendering/core/debugger.hpp>
#include <rendering/render_system.hpp>
#include <rendering/canvas.hpp>
#include <rendering/bindings.hpp>
#include <rendering/shaders.hpp>

#include <anim/anim_system.hpp>

#include <audio/audio_system.hpp>

#include <physics/physics_manager.hpp>
#include <physics/physics_system.hpp>

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
            m_Canvas = new cCanvas();
            m_AnimSystem = new cAnimSystem();
            m_AudioSystem = new cAudioSystem();
            m_PhysicsSystem = new cPhysicsSystem();

            InitShaders();

            cCameraManager::SetExposure(winDesc.Exposure);
            cCameraManager::SetGamma(winDesc.Gamma);

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
                cTaskManager::SubmitTask({ sTask::eType::AUDIO, [this]() {
                    m_AudioSystem->Update(m_Scene, DeltaTime);
                }});

                // submit animation task with current scene state
                cTaskManager::SubmitTask({ sTask::eType::ANIMATION, [this]() {
                    m_AnimSystem->Update(m_Scene, DeltaTime);
                }});

                // calls physics simulation that will be automatically dispatched into TaskManager
                // we don't need to submit physics system into task
                m_PhysicsSystem->Update(m_Scene, DeltaTime);

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

        void cApp::InitShaders()
        {
            cShaderManager::SetShader(new cSkyboxShader("skybox"));
            cShaderManager::SetShader(new cDirectionalShadowShader("directional_shadow"));
//            cShaderManager::SetShader(new cPointShadowShader("point_shadow"));
//            cShaderManager::SetShader(new cSpotShadowShader("spot_shadow"));
            cShaderManager::SetShader(new cOpaqueShader("opaque"));
            cShaderManager::SetShader(new cTransparentShader("transparent"));
            cShaderManager::SetShader(new cCompositeTransparentShader("composite_transparent", Config.MsaaSampleCount));
            cShaderManager::SetShader(new cSsaoShader("ssao", Config.MsaaSampleCount));
            cShaderManager::SetShader(new cWidgetShader("widget"));
            cShaderManager::SetShader(new cTextShader("text"));
            cShaderManager::SetShader(new cFinalShader("final", Config.MsaaSampleCount));

            cShaderManager::GetShader("ssao")->Enable = Config.EnableSSAO;
        }

        void cApp::Render()
        {
            RenderTargets::SSAO->Clear();
            m_RenderSystem->Update(m_Scene, DeltaTime);
            m_Canvas->Draw();
        }

    }

}
