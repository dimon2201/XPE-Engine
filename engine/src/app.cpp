#include <app.hpp>

namespace xpe
{
    namespace core
    {
        void cApp::Run()
        {
            if (!MFile::Exists("res"))
            {
                MFile::CreateDir("res");
            }

            if (!MFile::Exists("res/audio"))
            {
                MFile::CreateDir("res/audio");
            }

            if (!MFile::Exists("res/fonts"))
            {
                MFile::CreateDir("res/fonts");
            }

            if (!MFile::Exists("res/materials"))
            {
                MFile::CreateDir("res/materials");
            }

            if (!MFile::Exists("res/models"))
            {
                MFile::CreateDir("res/models");
            }

            if (!MFile::Exists("res/skybox"))
            {
                MFile::CreateDir("res/skybox");
            }

            if (!MFile::Exists("config"))
            {
                MFile::CreateDir("config");
            }

            if (!MFile::Exists("res/shaders"))
            {
                MFile::CreateDir("res/shaders");
            }

            if (!MFile::Exists("scripts"))
            {
                MFile::CreateDir("scripts");
            }

            LoadJSON("config/app.json", m_Config);

            sLoggerDescriptor logDesc;
            logDesc.Name = m_Config.Title.c_str();
            logDesc.Backtrace = m_Config.LogBacktrace;
            InitLogger(logDesc);

            sWindowDescriptor winDesc;
            winDesc.Title = m_Config.Title.c_str();
            winDesc.Width = m_Config.WinWidth;
            winDesc.Height = m_Config.WinHeight;
            winDesc.FrameWidth = m_Config.WinWidth;
            winDesc.FrameHeight = m_Config.WinHeight;
            winDesc.X = m_Config.WinX;
            winDesc.Y = m_Config.WinY;
            winDesc.VSync = m_Config.VSync;
            winDesc.Fullscreen = m_Config.Fullscreen;
            winDesc.Gamma = m_Config.Gamma;
            winDesc.Exposure = m_Config.Exposure;

            MTask::Init();
            MWindow::Init();
            MWindow::InitWindow(winDesc);
            MInput::Init();
            MAudio::Init();
            MPhysics::Init(MTask::GetSimulationDispatcher());

            render::context::EnableInfoLog = m_Config.GpuInfo;
            render::context::EnableWarnLog = m_Config.GpuWarning;
            render::context::EnableErrorLog = m_Config.GpuError;

            MCamera::Camera = new CPerspectiveCamera();
            MCamera::Camera->Viewport.Width = MWindow::GetWidth();
            MCamera::Camera->Viewport.Height = MWindow::GetHeight();
            MCamera::Camera->Exposure = winDesc.Exposure;
            MCamera::Camera->Gamma = winDesc.Gamma;
            MRender::Init(MCamera::Camera->Viewport, m_Config.MsaaSampleCount);
            m_Canvas = new cCanvas(&MCamera::Camera->Viewport);

            InitShaders();

            // TODO : move into serialization file
            m_Scene = new cScene(MPhysics::AddScene("Untitled"));

            Init();

            MRender::Prepare();

#ifdef K_EDITOR
            MEditor::SelectedScene = m_Scene;
            MEditor::SelectedCamera = MCamera::Camera;
            MEditor::ViewportImage = RenderTargets::Final->Colors[0];
            MEditor::Init();
            // StartWatchFiles();
#endif

            while (m_Opened)
            {
                long long beginTime = time_point_cast<milliseconds>(high_resolution_clock::now()).time_since_epoch().count();

                Update();

                // can be null during hot-reloading
                // TODO : create game scripting class
//                if (m_GameSystem != nullptr)
//                {
//                    m_GameSystem->Update(m_Scene, DeltaTime);
//                }

                // submit audio task with current scene state
                MTask::Add({sTask::eType::AUDIO, [this]()
                {
                    MAudio::Update(m_Scene, m_DeltaTime);
                }});

                // submit animation task with current scene state
                MTask::Add({sTask::eType::ANIMATION, [this]()
                {
                    MAnim::Update(m_Scene, m_DeltaTime);
                }});

                // calls physics simulation that will be automatically dispatched into TaskManager
                // we don't need to submit physics system into task
                MPhysics::Update(m_Scene, m_DeltaTime);

                Render();

                MWindow::PollEvents();
                MWindow::Swap();

                m_Opened = !MWindow::ShouldClose();

                long long endTime = time_point_cast<milliseconds>(high_resolution_clock::now()).time_since_epoch().count();
                m_DeltaTime = endTime - beginTime;
            }

            SaveJSON("config/app.json", m_Config);

#ifdef K_EDITOR
            // StopWatchFiles();
            MEditor::Free();
#endif

            Free();

            delete m_Scene;

            delete m_Canvas;
            MRender::Free();

            MAudio::Free();
            MPhysics::Free();
            MInput::Free();
            MWindow::FreeWindow();
            MWindow::Free();
            MTask::Free();

            FreeLogger();
        }

        void cApp::InitShaders()
        {
            MShader::SetShader(new cSkyboxShader("skybox"));
            MShader::SetShader(new cDirectionalShadowShader("directional_shadow"));
//            MShader::SetShader(new cPointShadowShader("point_shadow"));
//            MShader::SetShader(new cSpotShadowShader("spot_shadow"));
            MShader::SetShader(new cOpaqueShader("opaque"));
            MShader::SetShader(new cTransparentShader("transparent"));
            MShader::SetShader(new cCompositeTransparentShader("composite_transparent", m_Config.MsaaSampleCount));
            MShader::SetShader(new cSsaoShader("ssao", m_Config.MsaaSampleCount));
            MShader::SetShader(new cWidgetShader("widget"));
            MShader::SetShader(new cTextShader("text"));
            MShader::SetShader(new cFinalShader("final", m_Config.MsaaSampleCount));
            MShader::GetShader("ssao")->Enable = m_Config.EnableSSAO;
        }

        void cApp::Render()
        {
            RenderTargets::SSAO->Clear();
            MRender::Update(m_Scene, m_DeltaTime);
#ifdef K_EDITOR
            MEditor::Update(m_DeltaTime);
#else
            m_Canvas->Draw();
#endif
        }

        void cApp::LoadGameLib(const string &dllpath)
        {
            // TODO : add game script class
//            auto* gameSystem = MProc::Load<>(dllpath.c_str(), "CreateGameSystem");
//            if (gameSystem == nullptr)
//            {
//                LogError("Failed to load game system from DLL {}", dllpath);
//                return;
//            }
//
//            delete m_GameSystem;
//            m_GameSystem = gameSystem;
//            m_GameSystem->Init();
        }

        void cApp::GameLibModified(const string& watchpath, const string& filepath)
        {
            LogInfo("App: Game code of {} has been changed!", filepath);
            LoadGameLib(filepath);
        }

        void cApp::ShaderModified(const string& watchpath, const string& filepath)
        {
            LogInfo("App: Shader file {} has been changed!", filepath);
            MShader::ReloadStage(filepath.c_str());
        }

        void cApp::ModelModified(const string& watchpath, const string& filepath)
        {
            LogInfo("App: Model file {} has been changed!", filepath);
        }

        void cApp::FontModified(const string& watchpath, const string& filepath)
        {
            LogInfo("App: Font file {} has been changed!", filepath);
        }

        void cApp::MaterialModified(const string& watchpath, const string& filepath)
        {
            LogInfo("App: Material file {} has been changed!", filepath);
        }

        void cApp::SkyboxModified(const string& watchpath, const string& filepath)
        {
            LogInfo("App: Skybox file {} has been changed!", filepath);
        }

        void cApp::StartWatchFiles()
        {
            m_FileWatcher = new cMultiFileWatcher();
            m_DirWatcher = new cDirectoryWatcher();

            // watch gamelib
            {
                hstringstream ss;
                ss << "GameLib.dll";
                hstring gameCodeDllPath = ss.str();
                if (MFile::Exists(gameCodeDllPath.c_str()))
                {
                    auto& watch = m_FileWatcher->AddWatch(gameCodeDllPath.c_str());
                    watch.FileModifiedEvents.Add("GameLib", 1, [this](const string& watchpath, const string& filepath)
                    {
                        GameLibModified(watchpath, filepath);
                    });
                }
            }

            // watch shaders
            {
                hstringstream ss;
                ss << "res/shaders";
                hstring shadersPath = ss.str();
                if (MFile::Exists(shadersPath.c_str()))
                {
                    auto& watch = m_DirWatcher->AddWatch(shadersPath.c_str());
                    watch.FileModifiedEvents.Add("Shaders", 1, [this](const string& watchpath, const string& filepath)
                    {
                        ShaderModified(watchpath, filepath);
                    });
                    watch.FileAddedEvents.Add("Shaders", 1, [this](const string& watchpath, const string& filepath)
                    {
                        ShaderModified(watchpath, filepath);
                    });
                    watch.FileDeletedEvents.Add("Shaders", 1, [this](const string& watchpath, const string& filepath)
                    {
                        ShaderModified(watchpath, filepath);
                    });
                }
            }

            // watch res
            {
                hstringstream ss;
                ss << "res/models";
                hstring modelsPath = ss.str();
                if (MFile::Exists(modelsPath.c_str()))
                {
                    auto& watch = m_DirWatcher->AddWatch(modelsPath.c_str());
                    watch.FileModifiedEvents.Add("Models", 1, [this](const string& watchpath, const string& filepath)
                    {
                        ModelModified(watchpath, filepath);
                    });
                    watch.FileAddedEvents.Add("Models", 1, [this](const string& watchpath, const string& filepath)
                    {
                        ModelModified(watchpath, filepath);
                    });
                    watch.FileDeletedEvents.Add("Models", 1, [this](const string& watchpath, const string& filepath)
                    {
                        ModelModified(watchpath, filepath);
                    });
                }

                ss = {};
                ss << "res/fonts";
                hstring fontsPath = ss.str();
                if (MFile::Exists(fontsPath.c_str()))
                {
                    auto& watch = m_DirWatcher->AddWatch(fontsPath.c_str());
                    watch.FileModifiedEvents.Add("Fonts", 1, [this](const string& watchpath, const string& filepath)
                    {
                        FontModified(watchpath, filepath);
                    });
                    watch.FileAddedEvents.Add("Fonts", 1, [this](const string& watchpath, const string& filepath)
                    {
                        FontModified(watchpath, filepath);
                    });
                    watch.FileDeletedEvents.Add("Fonts", 1, [this](const string& watchpath, const string& filepath)
                    {
                        FontModified(watchpath, filepath);
                    });
                }

                ss = {};
                ss << "res/materials";
                hstring materialsPath = ss.str();
                if (MFile::Exists(materialsPath.c_str()))
                {
                    auto& watch = m_DirWatcher->AddWatch(materialsPath.c_str());
                    watch.FileModifiedEvents.Add("Materials", 1, [this](const string& watchpath, const string& filepath)
                    {
                        MaterialModified(watchpath, filepath);
                    });
                    watch.FileAddedEvents.Add("Materials", 1, [this](const string& watchpath, const string& filepath)
                    {
                        MaterialModified(watchpath, filepath);
                    });
                    watch.FileDeletedEvents.Add("Materials", 1, [this](const string& watchpath, const string& filepath)
                    {
                        MaterialModified(watchpath, filepath);
                    });
                }

                ss = {};
                ss << "res/skybox";
                hstring skyboxPath = ss.str();
                if (MFile::Exists(skyboxPath.c_str()))
                {
                    auto& watch = m_DirWatcher->AddWatch(skyboxPath.c_str());
                    watch.FileModifiedEvents.Add("Skybox", 1, [this](const string& watchpath, const string& filepath)
                    {
                        SkyboxModified(watchpath, filepath);
                    });
                    watch.FileAddedEvents.Add("Skybox", 1, [this](const string& watchpath, const string& filepath)
                    {
                        SkyboxModified(watchpath, filepath);
                    });
                    watch.FileDeletedEvents.Add("Skybox", 1, [this](const string& watchpath, const string& filepath)
                    {
                        SkyboxModified(watchpath, filepath);
                    });
                }
            }

            m_FileWatcher->Start();
            m_DirWatcher->Start();
        }

        void cApp::StopWatchFiles()
        {
            m_FileWatcher->Stop();
            m_DirWatcher->Stop();
            delete m_FileWatcher;
            delete m_DirWatcher;
        }
    }
}
