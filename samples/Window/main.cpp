#include <core/core.hpp>
#include <viewer/viewer.hpp>
#include <gltf/gltf.hpp>

using namespace xpe::core;

class GameApp : public App_Interface, public WindowEventListener, public KeyEventListener, public CursorEventListener
{
public:
    GameApp() {}
    ~GameApp() {}

    void Init(Window* window, RenderingContext_Interface* context, cUserInputManager* ui) override final
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
        char* vertexStr =
                "\
                    struct VSIn\
                    {\
                        float3 positionLocal : XPE_POSITION_LOCAL;\
                        float2 texcoord : XPE_TEXCOORD;\
                        float3 normal : XPE_NORMAL;\
                        uint instanceIndex : SV_InstanceID;\
                    };\
                    struct VSOut\
                    {\
                        float3 positionWorld : XPE_POSITION_WORLD;\
                        float2 texcoord : XPE_TEXCOORD2;\
                        float3 normal : XPE_NORMAL2;\
                        float4 positionClip : SV_POSITION;\
                    };\
                    struct RenderInstance\
                    {\
                        float4 Position;\
                    };\
                    cbuffer ConstantBuffer : register(b0)\
                    {\
                        float4x4 ViewProjection;\
                    };\
                    StructuredBuffer<RenderInstance> instances : register(t0);\
                    \
                    VSOut vs_main(VSIn vsIn)\
                    {\
                        VSOut vsOut = (VSOut)0;\
                        vsOut.positionWorld = 0.5 * vsIn.positionLocal + instances[vsIn.instanceIndex].Position.xyz;\
                        vsOut.texcoord = vsIn.texcoord;\
                        vsOut.normal = vsIn.normal;\
                        vsOut.positionClip = mul(ViewProjection, float4(vsOut.positionWorld, 1.0));\
                        return vsOut;\
                    }";

        char* pixelStr =
                "\
                    struct VSOut\
                    {\
                        float3 positionWorld : XPE_POSITION_WORLD;\
                        float2 texcoord : XPE_TEXCOORD2;\
                        float3 normal : XPE_NORMAL2;\
                        float4 positionClip : SV_POSITION;\
                    };\
                    float4 ps_main(VSOut psIn) : SV_TARGET\
                    {\
                        float3 lightPos = float3(0.0, 100.0, 0.0);\
                        float3 pointToLight = normalize(lightPos - psIn.positionWorld);\
                        float lambert = max(0.15, dot(pointToLight, psIn.normal));\
                        return float4(lambert, lambert, lambert, 1.0);\
                    }\
                ";

        _pipeline.VertexBuffer = _batch->GetVertexBuffer();
        _pipeline.IndexBuffer = _batch->GetIndexBuffer();
        _pipeline.InstanceBuffer = _batch->GetInstanceBuffer();
        _pipeline.ConstantBuffer = _batch->GetConstantBuffer();
        _pipeline.Shaders = new xShader();
        _pipeline.Shaders->PrimitiveTopology = ePrimitiveTopology::TRIANGLE_LIST;
        _pipeline.Shaders->Type = xShader::eType::VERTEX_PIXEL;
        _pipeline.Shaders->Sources[0] = vertexStr;
        _pipeline.Shaders->SourceEntryPoints[0] = "vs_main";
        _pipeline.Shaders->SourceProfiles[0] = "vs_4_0";
        _pipeline.Shaders->SourceFlags[0] = 0;
        _pipeline.Shaders->Sources[1] = pixelStr;
        _pipeline.Shaders->SourceEntryPoints[1] = "ps_main";
        _pipeline.Shaders->SourceProfiles[1] = "ps_4_0";
        _pipeline.Shaders->SourceFlags[1] = 0;
        _layout.PrimitiveTopology = ePrimitiveTopology::TRIANGLE_LIST;
        _layout.StrideByteSize = xpe::gltf::cGLTFModel::k_vertexSize;
        _layout.EntryCount = 3;
        _layout.Entries[0].Name = "XPE_POSITION_LOCAL";
        _layout.Entries[0].Format = xInputLayout::xEntry::eFormat::VEC3;
        _layout.Entries[0].ByteSize = 12;
        _layout.Entries[1].Name = "XPE_TEXCOORD";
        _layout.Entries[1].Format = xInputLayout::xEntry::eFormat::VEC2;
        _layout.Entries[1].ByteSize = 8;
        _layout.Entries[2].Name = "XPE_NORMAL";
        _layout.Entries[2].Format = xInputLayout::xEntry::eFormat::VEC3;
        _layout.Entries[2].ByteSize = 12;
        _pipeline.InputLayout = _layout;
        _pipeline.RenderTarget = _canvas->GetRenderTarget();
        _pipeline.DepthStencilState.UseDepthTest = K_TRUE;
        context->CreateRenderPipeline(_pipeline);
    }

    void Update(Window* window, RenderingContext_Interface* context, cUserInputManager* ui) override final
    {
        static float time = 0.0f;
        {
            xpe::core::xCPUProfiler pro(&time);

            _canvas->Clear(glm::vec4(1.0f));

            context->BindRenderPipeline(&_pipeline);

            static cTransformComponent tr("transform");
            static cViewerComponent vi("viewer");

            xpe::viewer::ViewerUpdate(time, ui, &tr, &vi);

            xConstantBuffer cbuffer;
            cbuffer.ViewerViewProjection = vi.ViewProjection;

            _batch->BeginBatch(std::string("NewGeometryData"));
            _batch->RecordConstantBuffer(&cbuffer);
            for (f32 y = -50.0f; y < 50.0f; y += 4.0f)
            {
                for (f32 x = -50.0f; x < 50.0f; x += 4.0f)
                {
                    for (f32 z = -50.0f; z < 50.0f; z += 4.0f)
                    {
                        xRenderInstance instance;
                        instance.Position = glm::vec4(x, y, z, 0.0f);
                        _batch->RecordInstance(instance);
                    }
                }
            }
            _batch->EndBatch();
            _batch->DrawBatch();

            _canvas->Present();
        }

        static int logDeltaLimit;
        logDeltaLimit++;
        if (logDeltaLimit > 2000)
        {
            logDeltaLimit = 0;
            LogDelta(time);
        }
    }

    void Free() override
    {
        LogInfo("GameApp::Free()");
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
    Canvas* _canvas;
    ECSManager* _ecs;
    BatchManager* _batch;
    xPipeline _pipeline;
    xInputLayout _layout;
    Window* _window;
    cUserInputManager* _ui;
};

int main()
{
    GameApp app;

    WindowDescriptor desc;
    desc.Width = 800;
    desc.Height = 600;
    desc.Title = "Game App Test";
    desc.GPUApi = K_GPUAPI_D3D11;

    LoggerDescriptor logDesc;
    logDesc.Name = "XPE-Window";
    logDesc.Backtrace = 32;

    RunApp(&app, desc, logDesc);

    return 0;
}