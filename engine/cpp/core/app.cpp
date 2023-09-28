#include <core/app.hpp>
#include <core/windowing.hpp>

#include <ecs/scenes.hpp>
#include <ecs/entities.hpp>

#include <rendering/core/debugger.h>

#include <rendering/monitor.h>
#include <rendering/renderer.h>

#include <rendering/draw/canvas.hpp>
#include <rendering/draw/instance_drawer.h>
#include <rendering/draw/skeletal_anim_drawer.h>
#include <rendering/draw/text2d_drawer.h>
#include <rendering/draw/text3d_drawer.h>
#include <rendering/draw/skybox_drawer.h>

#include <rendering/shadow/shadow.h>

#include <rendering/storages/texture_storage.h>
#include <rendering/storages/font_storage.h>

#include <anim/animator.h>
#include <anim/storages/anim_storage.h>

#include <audio/system/audio_system.h>
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
            // Canvas drawing
            {
                Shader* shader = ShaderManager::CreateShader("canvas");
                ShaderManager::AddVertexStageFromFile(shader, "engine_shaders/draw/canvas.vs");
                ShaderManager::AddPixelStageFromFile(shader, "engine_shaders/draw/canvas.ps");
                ShaderManager::BuildShader(shader);
                m_Canvas = new Canvas(WindowManager::GetWidth(), WindowManager::GetHeight(), shader);
            }
            // Skybox drawing
            {
                Shader* shader = ShaderManager::CreateShader("skybox_drawer");
                ShaderManager::AddVertexStageFromFile(shader, "engine_shaders/draw/skybox_drawer.vs");
                ShaderManager::AddPixelStageFromFile(shader, "engine_shaders/draw/skybox_drawer.ps");
                ShaderManager::BuildShader(shader);
                m_Renderer->AddDrawer<SkyboxDrawer>(
                        shader,
                        m_Canvas->GetRenderTarget(),
                        m_GeometryStorage,
                        vector<Buffer*> { m_Renderer->CameraBuffer }
                );
            }
            // Instance drawing for 3D
            {
                Shader* shader = ShaderManager::CreateShader("instance_drawer");
                ShaderManager::AddVertexStageFromFile(shader, "engine_shaders/draw/instance_drawer.vs");
                ShaderManager::AddPixelStageFromFile(shader, "engine_shaders/draw/pbr.ps");
                ShaderManager::BuildShader(shader);
                m_Renderer->AddDrawer<InstanceDrawer>(
                        shader,
                        m_Canvas->GetRenderTarget(),
                        m_MaterialStorage,
                        vector<Buffer*> {
                            m_Renderer->CameraBuffer
                        },
                        vector<Buffer*> {
                            m_Renderer->DirectLightBuffer,
                            m_Renderer->PointLightBuffer,
                            m_Renderer->SpotLightBuffer,
                            Shadow::Get().GetBuffer(),
                            Monitor::Get().GetBuffer()
                        }
                );
            }
            // Instance drawing for 3D skeletal skin
            {
                Shader* shader = ShaderManager::CreateShader("skeletal_anim_drawer");
                ShaderManager::AddVertexStageFromFile(shader, "engine_shaders/draw/skeletal_anim_drawer.vs");
                ShaderManager::AddPixelStageFromFile(shader, "engine_shaders/draw/pbr.ps");
                ShaderManager::BuildShader(shader);
                m_Renderer->AddDrawer<SkeletalAnimDrawer>(
                        shader,
                        m_Canvas->GetRenderTarget(),
                        m_MaterialStorage,
                        m_SkeletStorage,
                        m_SkinStorage,
                        vector<Buffer*> {
                            m_Renderer->CameraBuffer
                        },
                        vector<Buffer*> {
                            m_Renderer->DirectLightBuffer,
                            m_Renderer->PointLightBuffer,
                            m_Renderer->SpotLightBuffer,
                            Shadow::Get().GetBuffer(),
                            Monitor::Get().GetBuffer()
                        }
                );
            }
            // Text 2D drawing
            {
                Shader* shader = ShaderManager::CreateShader("text2d_drawer");
                ShaderManager::AddVertexStageFromFile(shader, "engine_shaders/draw/text2d_drawer.vs");
                ShaderManager::AddPixelStageFromFile(shader, "engine_shaders/draw/text2d_drawer.ps");
                ShaderManager::BuildShader(shader);
                m_Renderer->AddDrawer<Text2DDrawer>(
                        shader,
                        m_Canvas->GetRenderTarget(),
                        m_GeometryStorage,
                        vector<Buffer*> { m_Renderer->CameraBuffer, m_Canvas->GetBuffer() }
                );
            }
            // Text 3D drawing
            {
                Shader* shader = ShaderManager::CreateShader("text3d_drawer");
                ShaderManager::AddVertexStageFromFile(shader, "engine_shaders/draw/text3d_drawer.vs");
                ShaderManager::AddPixelStageFromFile(shader, "engine_shaders/draw/text3d_drawer.ps");
                ShaderManager::BuildShader(shader);
                m_Renderer->AddDrawer<Text3DDrawer>(
                        shader,
                        m_Canvas->GetRenderTarget(),
                        m_GeometryStorage,
                        vector<Buffer*> { m_Renderer->CameraBuffer }
                );
            }
        }

        void Application::Render()
        {
            m_Canvas->Clear(ClearColor);
            m_Renderer->Render(m_MainScene);
            m_Canvas->Present();
        }

    }

}
