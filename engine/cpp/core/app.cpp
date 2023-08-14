#include <core/app.hpp>
#include <core/windowing.hpp>

#include <ecs/scenes.hpp>
#include <ecs/entities.hpp>

#include <rendering/core/debugger.h>

#include <rendering/renderer.h>

#include <rendering/draw/canvas.hpp>

#include <rendering/storages/geometry_storage.h>
#include <rendering/storages/texture_storage.h>
#include <rendering/storages/material_storage.h>
#include <rendering/storages/font_storage.h>

namespace xpe {

    using namespace render;
    using namespace ecs;

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

            InitRenderer();

            m_MainScene = new MainScene();
            m_MainScene->PerspectiveCamera->Buffer = m_Renderer->GetCameraBuffer();
            m_MainScene->OrthoCamera->Buffer = m_Renderer->GetCameraBuffer();

            Init();
            m_Game = CreateGame();
            InitGame();

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

                Update();

                m_Game->Update();

                Render();

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

            m_Game->Free();
            delete m_Game;
            Free();
            delete m_MainScene;
            delete m_FontStorage;
            delete m_GeometryStorage;
            delete m_MaterialStorage;
            delete m_TextureStorage;
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
            m_Game->Renderer = m_Renderer;
            m_Game->FontStorage = m_FontStorage;
            m_Game->MaterialStorage = m_MaterialStorage;
            m_Game->GeometryStorage = m_GeometryStorage;
            m_Game->TextureStorage = m_TextureStorage;
            m_Game->MainScene = m_MainScene;
            m_Game->Init();
        }

        Game* Application::CreateGame()
        {
            return new Game();
        }

        void Application::InitRenderer()
        {
            // Canvas
            {
                Shader* shader = ShaderManager::CreateShader("canvas");
                ShaderManager::AddVertexStageFromFile(shader, "engine_shaders/canvas.vs");
                ShaderManager::AddPixelStageFromFile(shader, "engine_shaders/canvas.ps");
                ShaderManager::BuildShader(shader);
                m_Canvas = new Canvas(WindowManager::GetWidth(), WindowManager::GetHeight(), shader);
            }
        }

        void Application::Render()
        {
            m_Canvas->Clear(ClearColor);
            m_Renderer->Render(m_MainScene, m_Canvas->GetRenderTarget());
            m_Canvas->Present();
        }

    }

}
