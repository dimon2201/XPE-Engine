#include <core/app.hpp>
#include <core/windowing.hpp>

#include <ecs/scenes.hpp>
#include <ecs/entities.hpp>

#include <rendering/core/debugger.h>

#include <rendering/renderer.h>

#include <rendering/buffers/light_buffers.h>

#include <rendering/draw/canvas.hpp>
#include <rendering/draw/instance_render_pass.h>
#include <rendering/draw/skeletal_render_pass.h>
#include <rendering/draw/text2d_render_pass.h>
#include <rendering/draw/text3d_render_pass.h>
#include <rendering/draw/skybox_render_pass.h>
#include <rendering/draw/merge_render_pass.h>
#include <rendering/draw/ssao_render_pass.hpp>

#include <rendering/storages/texture_storage.h>
#include <rendering/storages/font_storage.h>

#include <anim/animator.h>
#include <anim/storages/anim_storage.h>

xpe::render::RenderTarget* mainRT;
xpe::render::RenderTarget* ssaoRT;
xpe::render::RenderPass* ssaoRP;

namespace xpe {

    using namespace render;
    using namespace ecs;
    using namespace anim;

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

            m_SkeletStorage = new SkeletStorage();
            m_SkinStorage = new SkinStorage();
            m_AnimStorage = new AnimStorage();
            m_Animator = new Animator(m_SkeletStorage);

            InitRenderer();

            m_MainScene = new MainScene();
            m_MainScene->PerspectiveCamera->Buffer = m_Renderer->CameraBuffer;
            m_MainScene->OrthoCamera->Buffer = m_Renderer->CameraBuffer;

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

                m_Animator->Animate(m_MainScene, DeltaTime);

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

            delete m_AnimStorage;
            delete m_SkinStorage;
            delete m_SkeletStorage;
            delete m_Animator;

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
            ShaderManager::AddVertexStageFromFile(shader, "engine_shaders/draw/canvas.vs");
            ShaderManager::AddPixelStageFromFile(shader, "engine_shaders/draw/canvas.ps");
            ShaderManager::BuildShader(shader);
            m_Canvas = new Canvas(WindowManager::GetWidth(), WindowManager::GetHeight(), shader);

            RenderTarget* canvasRT = m_Canvas->GetRenderTarget();
            Viewport* canvasViewport = m_Canvas->GetBuffer()->Get(0);

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

            mainRT = new RenderTarget({ mainColor, mainPosition, mainNormal }, (const Texture*)mainDepth, *canvasViewport);

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

            ssaoRT = new RenderTarget({ ssaoColor }, ssaoDepth, *canvasViewport);

            // Skybox drawing
//            {
//                Shader* shader = ShaderManager::CreateShader("skybox_drawer");
//                ShaderManager::AddVertexStageFromFile(shader, "engine_shaders/draw/skybox_drawer.vs");
//                ShaderManager::AddPixelStageFromFile(shader, "engine_shaders/draw/skybox_drawer.ps");
//                ShaderManager::BuildShader(shader);
//                m_Renderer->AddDrawer<SkyboxDrawer>(
//                        m_Renderer->CameraBuffer,
//                        shader,
//                        mainRT,
//                        m_GeometryStorage
//                );
//            }
            // Instance drawing for 3D
            {
                Shader* shader = ShaderManager::CreateShader("instance_drawer");
                ShaderManager::AddVertexStageFromFile(shader, "engine_shaders/draw/instance_drawer.vs");
                ShaderManager::AddPixelStageFromFile(shader, "engine_shaders/draw/instance_drawer.ps");
                ShaderManager::BuildShader(shader);

                vector<RenderPassBinding> bindings = {
                        { "Shader", RenderPassBinding::eType::SHADER, RenderPassBinding::eStage::VERTEX, 0, (GPUResource*)shader },
                        { "CameraBuffer", RenderPassBinding::eType::BUFFER, RenderPassBinding::eStage::VERTEX, RenderPassBinding::SLOT_DEFAULT, (GPUResource*)m_Renderer->CameraBuffer },
                        { "DirectLightBuffer", RenderPassBinding::eType::BUFFER, RenderPassBinding::eStage::PIXEL, RenderPassBinding::SLOT_DEFAULT, (GPUResource*)m_Renderer->DirectLightBuffer },
                        { "PointLightBuffer", RenderPassBinding::eType::BUFFER, RenderPassBinding::eStage::PIXEL, RenderPassBinding::SLOT_DEFAULT, (GPUResource*)m_Renderer->PointLightBuffer },
                        { "SpotLightBuffer", RenderPassBinding::eType::BUFFER, RenderPassBinding::eStage::PIXEL, RenderPassBinding::SLOT_DEFAULT, (GPUResource*)m_Renderer->SpotLightBuffer },
                };
                m_Renderer->AddRenderPass<InstanceRenderPass>(
                    bindings,
                    mainRT,
                    m_MaterialStorage
                );
            }
            // Instance drawing for 3D skeletal skin
            {
                Shader* shader = ShaderManager::CreateShader("skeletal_anim_drawer");
                ShaderManager::AddVertexStageFromFile(shader, "engine_shaders/draw/skeletal_anim_drawer.vs");
                ShaderManager::AddPixelStageFromFile(shader, "engine_shaders/draw/skeletal_anim_drawer.ps");
                ShaderManager::BuildShader(shader);

                vector<RenderPassBinding> bindings = {
                    { "Shader", RenderPassBinding::eType::SHADER, RenderPassBinding::eStage::VERTEX, 0, (GPUResource*)shader },
                    { "CameraBuffer", RenderPassBinding::eType::BUFFER, RenderPassBinding::eStage::VERTEX, RenderPassBinding::SLOT_DEFAULT, (GPUResource*)m_Renderer->CameraBuffer },
                    { "DirectLightBuffer", RenderPassBinding::eType::BUFFER, RenderPassBinding::eStage::PIXEL, RenderPassBinding::SLOT_DEFAULT, (GPUResource*)m_Renderer->DirectLightBuffer },
                    { "PointLightBuffer", RenderPassBinding::eType::BUFFER, RenderPassBinding::eStage::PIXEL, RenderPassBinding::SLOT_DEFAULT, (GPUResource*)m_Renderer->PointLightBuffer },
                    { "SpotLightBuffer", RenderPassBinding::eType::BUFFER, RenderPassBinding::eStage::PIXEL, RenderPassBinding::SLOT_DEFAULT, (GPUResource*)m_Renderer->SpotLightBuffer },
                };
                m_Renderer->AddRenderPass<SkeletalAnimRenderPass>(
                    bindings,
                    mainRT,
                    m_MaterialStorage,
                    m_SkeletStorage,
                    m_SkinStorage
                );
            }
            // Text 2D drawing
            {
                Shader* shader = ShaderManager::CreateShader("text2d_drawer");
                ShaderManager::AddVertexStageFromFile(shader, "engine_shaders/draw/text2d_drawer.vs");
                ShaderManager::AddPixelStageFromFile(shader, "engine_shaders/draw/text2d_drawer.ps");
                ShaderManager::BuildShader(shader);

                vector<RenderPassBinding> bindings = {
                    { "Shader", RenderPassBinding::eType::SHADER, RenderPassBinding::eStage::VERTEX, 0, (GPUResource*)shader },
                    { "CameraBuffer", RenderPassBinding::eType::BUFFER, RenderPassBinding::eStage::VERTEX, RenderPassBinding::SLOT_DEFAULT, (GPUResource*)m_Renderer->CameraBuffer },
                    { "ViewportBuffer", RenderPassBinding::eType::BUFFER, RenderPassBinding::eStage::VERTEX, RenderPassBinding::SLOT_DEFAULT, (GPUResource*)m_Canvas->GetBuffer() }
                };
                m_Renderer->AddRenderPass<Text2DRenderPass>(
                    bindings,
                    mainRT,
                    m_GeometryStorage
                );
            }
            // Text 3D drawing
            {
                Shader* shader = ShaderManager::CreateShader("text3d_drawer");
                ShaderManager::AddVertexStageFromFile(shader, "engine_shaders/draw/text3d_drawer.vs");
                ShaderManager::AddPixelStageFromFile(shader, "engine_shaders/draw/text3d_drawer.ps");
                ShaderManager::BuildShader(shader);

                vector<RenderPassBinding> bindings = {
                    { "Shader", RenderPassBinding::eType::SHADER, RenderPassBinding::eStage::VERTEX, 0, (GPUResource*)shader },
                    { "CameraBuffer", RenderPassBinding::eType::BUFFER, RenderPassBinding::eStage::VERTEX, RenderPassBinding::SLOT_DEFAULT, (GPUResource*)m_Renderer->CameraBuffer },
                    { "ViewportBuffer", RenderPassBinding::eType::BUFFER, RenderPassBinding::eStage::VERTEX, RenderPassBinding::SLOT_DEFAULT, (GPUResource*)m_Canvas->GetBuffer() }
                };
                m_Renderer->AddRenderPass<Text3DRenderPass>(
                    bindings,
                    mainRT,
                    m_GeometryStorage
                );
            }
            // SSAO drawing
            {
                m_GeometryStorage->AddGeometryIndexed2D("SSAODrawerQuad", Quad2D());

                Shader* shader = ShaderManager::CreateShader("ssao_drawer");
                ShaderManager::AddVertexStageFromFile(shader, "engine_shaders/draw/ssao_drawer.vs");
                ShaderManager::AddPixelStageFromFile(shader, "engine_shaders/draw/ssao_drawer.ps");
                ShaderManager::BuildShader(shader);

                vector<RenderPassBinding> bindings = {
                    { "Shader", RenderPassBinding::eType::SHADER, RenderPassBinding::eStage::VERTEX, 0, (GPUResource*)shader },
                    { "PositionTexture", RenderPassBinding::eType::TEXTURE, RenderPassBinding::eStage::PIXEL, 0, (GPUResource*)mainRT->Colors[1] },
                    { "NormalTexture", RenderPassBinding::eType::TEXTURE, RenderPassBinding::eStage::PIXEL, 1, (GPUResource*)mainRT->Colors[2] },
                    { "DepthTexture", RenderPassBinding::eType::TEXTURE, RenderPassBinding::eStage::PIXEL, 2, (GPUResource*)mainRT->DepthStencil }
                };
                m_Renderer->AddRenderPass<SSAORenderPass>(
                    m_GeometryStorage,
                    bindings,
                    ssaoRT
                );
            }
            // Output to canvas
            {
                Shader* shader = ShaderManager::CreateShader("merge");
                ShaderManager::AddVertexStageFromFile(shader, "engine_shaders/draw/merge_render_pass.vs");
                ShaderManager::AddPixelStageFromFile(shader, "engine_shaders/draw/merge_render_pass.ps");
                ShaderManager::BuildShader(shader);

                vector<RenderPassBinding> bindings = {
                    { "Shader", RenderPassBinding::eType::SHADER, RenderPassBinding::eStage::VERTEX, 0, (GPUResource*)shader },
                    { "ColorTexture", RenderPassBinding::eType::TEXTURE, RenderPassBinding::eStage::PIXEL, 0, (GPUResource*)mainRT->Colors[0] },
                    { "AOTexture", RenderPassBinding::eType::TEXTURE, RenderPassBinding::eStage::PIXEL, 2, (GPUResource*)ssaoRT->Colors[0] }
                };
                m_Renderer->AddRenderPass<MergeRenderPass>(
                    bindings,
                    canvasRT,
                    m_GeometryStorage
                );
            }
        }

        void Application::Render()
        {
            context::ClearColorTarget((void*)mainRT->ColorViews[0], glm::vec4(1.0f));
            context::ClearColorTarget((void*)mainRT->ColorViews[1], glm::vec4(0.0f));
            context::ClearColorTarget((void*)mainRT->ColorViews[2], glm::vec4(0.0f));
            context::ClearDepthTarget((void*)mainRT->DepthStencilView, 1.0f);
            context::ClearColorTarget((void*)ssaoRT->ColorViews[0], glm::vec4(1.0f));
            context::ClearDepthTarget((void*)ssaoRT->DepthStencilView, 1.0f);

            m_Canvas->Clear(ClearColor);
            m_Renderer->Render(m_MainScene);
            m_Canvas->Present();
        }

    }

}
