#include <core/core.hpp>
#include <gltf/gltf.hpp>

using namespace xpe::core;
using namespace xpe::render;
using namespace xpe::control;

class GameApp : public App_Interface, public WindowEventListener, public KeyEventListener, public CursorEventListener
{
public:
    GameApp() {}
    ~GameApp() {}

    void Init(Window* window, Context* context, cUserInputManager* ui) override final
    {
        LogInfo("GameApp::Init()");

        _window = window;
        _ui = ui;

        ui->AddWindowEventListener(this, 1);
        ui->AddKeyEventListener(this, 1);
        ui->AddCursorEventListener(this, 1);

        _canvas = new Canvas(window->GetWidth(), window->GetHeight(), context);
        _ecs = new ECSManager();
        _batch = new BatchManager(context);

        _cameraBuffer.Init(context);

        static cPerspectiveCameraComponent perspectiveCamera("PerspectiveCamera");
        _cameraController = new cPerspectiveCameraController(ui, &_cameraBuffer, &perspectiveCamera, &_time);

        xpe::gltf::cGLTFModel model("files/cube.gltf");
        xpe::gltf::xMesh* mesh = model.GetMesh(0);

        // Put geometry
        _batch->StoreGlobalGeometryData(
                std::string("NewGeometryData"),
                &mesh->Vertices[0],
                mesh->VertexCount * xpe::gltf::cGLTFModel::k_vertexSize,
                mesh->VertexCount,
                &mesh->Indices[0],
                mesh->IndexCount * 4,
                mesh->IndexCount
        );

        // Create render pipeline data
        // setup buffers
        _pipeline.VertexBuffer = _batch->GetVertexBuffer();
        _pipeline.IndexBuffer = _batch->GetIndexBuffer();
        _pipeline.InstanceBuffer = _batch->GetInstanceBuffer();
        _pipeline.ConstantBuffers.emplace_back(&_cameraBuffer);
        _pipeline.ConstantBuffers.emplace_back(MaterialManager::GetBuffer());
        // setup shader
        _pipeline.Shader = ShaderManager::Builder()
                .AddVertexStageFromFile("shaders/window.vs")
                .AddPixelStageFromFile("shaders/window.ps")
                .Build();
        _pipeline.Shader->PrimitiveTopology = ePrimitiveTopology::TRIANGLE_LIST;
        // setup input layout
        _layout.PrimitiveTopology = ePrimitiveTopology::TRIANGLE_LIST;
        _layout.StrideByteSize = xpe::gltf::cGLTFModel::k_vertexSize;
        _layout.EntryCount = 3;
        _layout.Entries[0].Name = "XPE_POSITION_LOCAL";
        _layout.Entries[0].Format = InputLayout::Entry::eFormat::VEC3;
        _layout.Entries[0].ByteSize = 12;
        _layout.Entries[1].Name = "XPE_TEXCOORD";
        _layout.Entries[1].Format = InputLayout::Entry::eFormat::VEC2;
        _layout.Entries[1].ByteSize = 8;
        _layout.Entries[2].Name = "XPE_NORMAL";
        _layout.Entries[2].Format = InputLayout::Entry::eFormat::VEC3;
        _layout.Entries[2].ByteSize = 12;
        _pipeline.InputLayout = _layout;
        _pipeline.RenderTarget = _canvas->GetRenderTarget();
        _pipeline.DepthStencilState.UseDepthTest = K_TRUE;
        context->CreateRenderPipeline(_pipeline);
    }

    void Update(Window* window, Context* context, cUserInputManager* ui) override final
    {
        {
            xpe::core::xCPUProfiler pro(&_time);

            _cameraController->Move();

            _canvas->Clear(glm::vec4(1.0f));

            context->BindRenderPipeline(&_pipeline);

            static cTransformComponent tr("transform");
            static cMaterialComponent materialComponent(
                    "material",
                    MaterialManager::Builder()
                            .AddAlbedoFromFile("files/sinister_man.png")
                            .Build("material")
            );

            _batch->BeginBatch(std::string("NewGeometryData"));
            for (f32 y = -50.0f; y < 50.0f; y += 4.0f)
            {
                for (f32 x = -50.0f; x < 50.0f; x += 4.0f)
                {
                    for (f32 z = -50.0f; z < 50.0f; z += 4.0f)
                    {
                        RenderInstance instance;
                        instance.Position = glm::vec4(x, y, z, 0.0f);
                        _batch->RecordInstance(instance);
                    }
                }
            }
            _batch->EndBatch();

            MaterialManager::BindMaterial(*materialComponent.Material, eShaderType::PIXEL, 0);
            MaterialManager::UpdateMaterial(*materialComponent.Material);

            _batch->DrawBatch();

            _canvas->Present();
        }

        static int logDeltaLimit;
        logDeltaLimit++;
        if (logDeltaLimit > 2000)
        {
            logDeltaLimit = 0;
            LogDelta(_time.Seconds());
        }
    }

    void Free() override
    {
        LogInfo("GameApp::Free()");
        _cameraBuffer.Free();
    }

    void WindowClosed() override
    {
        LogWarning("GameApp::WindowClosed()");
    }

    void KeyPressed(const eKey key) override
    {
        if (key == eKey::Esc)
        {
            CloseWindow(*_window);
        }
    }

    void CursorMoved(const double x, const double y) override
    {
        LogInfo("GameApp::CursorMoved({}, {})", x, y);
    }

private:
    Time _time = 0;
    Canvas* _canvas;
    ECSManager* _ecs;
    BatchManager* _batch;
    Pipeline _pipeline;
    InputLayout _layout;
    Window* _window;
    cUserInputManager* _ui;
    CameraBuffer _cameraBuffer;
    cPerspectiveCameraController* _cameraController;
};

int main()
{
    GameApp app;

    EngineConfig::GPU_API = eGPU_API::DX11;

    WindowDescriptor winDesc;
    winDesc.Width = 800;
    winDesc.Height = 600;
    winDesc.Title = "Game App Test";

    LoggerDescriptor logDesc;
    logDesc.Name = "XPE-Window";
    logDesc.Backtrace = 32;

    RunApp(&app, winDesc, logDesc);

    return 0;
}