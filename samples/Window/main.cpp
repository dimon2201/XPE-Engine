#include <iostream>
#include <gtc/matrix_transform.hpp>
#include "../../engine/src/core/core.hpp"
#include "../../engine/src/viewer/viewer.hpp"

using namespace xpe::core;

class GameApp : public App_Interface
{
    public:
        GameApp() {}
        ~GameApp() {}

        void Init(Window* window, RenderingContext_Interface* context, cUserInputManager* ui) override final
        {
            _canvas = new Canvas(window->GetWidth(), window->GetHeight(), context);
            _ecs = new ECSManager();
            _batch = new BatchManager(context);

            // Put geometry
            glm::vec3 vertices[3] = {
                glm::vec3(-0.02f, -0.02f, 0.0f),
                glm::vec3(0.0f, 0.02f, 0.0f),
                glm::vec3(0.02f, -0.02f, 0.0f)
            };
            
            u32 indices[3] = { 0, 1, 2 };

            _batch->StoreGlobalGeometryData(std::string("NewGeometryData"), &vertices[0], 3 * 12, 3, &indices[0], 3 * 4, 3);
        
            // Create render pipeline data
            _pipeline.InputVertexBuffer = _batch->GetVertexBuffer();
            _pipeline.InputIndexBuffer = _batch->GetIndexBuffer();
            _pipeline.InputInstanceBuffer = _batch->GetInstanceBuffer();
            _pipeline.InputConstantBuffer = _batch->GetConstantBuffer();
            const char* vertexStr =
                "\
                    struct VSIn\
                    {\
                        uint instanceIndex : SV_InstanceID;\
                        float3 positionLocal : XPE_POSITION_LOCAL;\
                    };\
                    struct VSOut\
                    {\
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
                    VSOut vsOut;\
                    vsOut.positionClip = mul(ViewProjection, float4(15.0 * vsIn.positionLocal + instances[vsIn.instanceIndex].Position.xyz, 1.0));\
                    return vsOut;\
                    }\
                ";
            const char* pixelStr =
                "\
                    float4 ps_main() : SV_TARGET\
                    {\
                        return float4(0.0, 0.0, 1.0, 1.0);\
                    }\
                ";
            _pipeline.Shaders = new xShader();
            context->CreateShaderFromString(*_pipeline.Shaders, xShader::PrimitiveTopology::TRIANGLE_LIST, xShader::Type::VERTEX, vertexStr, "vs_main", "vs_4_0", 0);
            context->CreateShaderFromString(*_pipeline.Shaders, xShader::PrimitiveTopology::TRIANGLE_LIST, xShader::Type::PIXEL, pixelStr, "ps_main", "ps_4_0", 0);
            _layout.EntryCount = 1;
            _layout.Entries[0].Name = "XPE_POSITION_LOCAL";
            _layout.Entries[0].Format = xInputLayout::xEntry::eFormat::VEC3;
            _layout.Entries[0].ByteSize = 12;
            _layout.VertexShaderByteCode = _pipeline.Shaders->VertexShaderByteCode;
            _layout.VertexShaderByteCodeSize = _pipeline.Shaders->VertexShaderByteCodeSize;
            context->CreateInputLayout(_layout);
            _pipeline.InputLayout = _layout;
            _pipeline.RenderTarget = _canvas->GetRenderTarget();
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
                context->OutputErrors();
            }
        }

    private:
        Canvas* _canvas;
        ECSManager* _ecs;
        BatchManager* _batch;
        xPipeline _pipeline;
        xInputLayout _layout;
};

int main()
{
    GameApp app;
    WindowDescriptor desc;
    desc.Width = 800;
    desc.Height = 600;
    desc.Title = "Game App Test";
    desc.GPUApi = K_GPUAPI_D3D11;

    RunApp(&app, desc);

    return 0;
}