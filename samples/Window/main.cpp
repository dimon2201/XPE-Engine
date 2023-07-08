#include <core/core.hpp>
#include <gltf/gltf.hpp>

using namespace xpe::core;
using namespace xpe::render;
using namespace xpe::control;

class GameApp;

class GameApp : public Application, public WindowEventListener, public KeyEventListener, public CursorEventListener
{
public:
    GameApp() {}
    ~GameApp() {}

    void Init() override final
    {
        LogInfo("GameApp::Init()");

        Input::AddWindowEventListener(this, 1);
        Input::AddKeyEventListener(this, 1);
        Input::AddCursorEventListener(this, 1);

        _canvas = new Canvas(window->GetWidth(), window->GetHeight(), context);
        _ecs = new ECSManager();
        _batch = new BatchManager(context);

        _cameraBuffer.Init(context);

        static cPerspectiveCameraComponent perspectiveCamera("PerspectiveCamera");
        _cameraController = new cPerspectiveCameraController(&_cameraBuffer, &perspectiveCamera, &time);

        TextureCubeFile textureCubeFile;
        textureCubeFile.Name = "test";
        textureCubeFile.FrontFilepath = "files/skybox/front.jpg";
        textureCubeFile.BackFilepath = "files/skybox/back.jpg";
        textureCubeFile.RightFilepath = "files/skybox/right.jpg";
        textureCubeFile.LeftFilepath = "files/skybox/left.jpg";
        textureCubeFile.TopFilepath = "files/skybox/top.jpg";
        textureCubeFile.BottomFilepath = "files/skybox/bottom.jpg";

        Texture* textureCube = TextureManager::LoadTextureCubeFile(textureCubeFile, Texture::eFormat::RGBA8);

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
        _pipeline.VSBuffers.emplace_back(_batch->GetInstanceBuffer());
        _pipeline.VSBuffers.emplace_back(&_cameraBuffer);
        _pipeline.PSBuffers.emplace_back(MaterialManager::GetBuffer());
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

    void Update() override final
    {
        {
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

            MaterialManager::BindMaterial(*materialComponent.Material);
            MaterialManager::UpdateMaterial(*materialComponent.Material);

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

            _batch->DrawBatch();

            _canvas->Present();
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
            CloseWindow(*window);
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
    Pipeline _pipeline;
    InputLayout _layout;
    CameraBuffer _cameraBuffer;
    cPerspectiveCameraController* _cameraController;
};

Application* CreateApplication() {
    return new GameApp();
}
