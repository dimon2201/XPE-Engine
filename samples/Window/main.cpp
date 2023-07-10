#include <core/core.hpp>
#include <gltf/gltf.hpp>
#include <ttf/ttf_manager.hpp>

using namespace xpe::core;
using namespace xpe::render;
using namespace xpe::control;
using namespace xpe::ttf;

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

        xFont font = TTFManager::Load("Roboto-Italic.ttf", 32);
        TTFManager::Free(font);

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
                "NewGeometryData",
                &mesh->Vertices[0],
                mesh->VertexCount * xpe::gltf::cGLTFModel::k_vertexSize,
                mesh->VertexCount,
                &mesh->Indices[0],
                mesh->IndexCount * 4,
                mesh->IndexCount
        );

        // Put instances of geometry
        for (f32 y = -50.0f; y < 50.0f; y += 4.0f)
        {
            for (f32 x = -50.0f; x < 50.0f; x += 4.0f)
            {
                u32 materialIndex = 0;
                for (f32 z = -50.0f; z < 50.0f; z += 4.0f)
                {
                    RenderInstance instance;
                    instance.Position = glm::vec4(x, y, z, 0.0f);
                    instance.MaterialIndex = materialIndex;

                    Material* material = MaterialManager::Builder().Build("Material_" + materialIndex);
                    material->Index = materialIndex;
                    material->Data = MaterialManager::GetMaterialData(materialIndex);
                    float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
                    float g = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
                    float b = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
                    material->Data->BaseColor = { r, g, b, 1 };

                    _batch->AddInstance("NewGeometryData", instance);

                    materialIndex++;
                }
            }
        }

        // it will flush all materials data into GPU memory
        MaterialManager::UpdateMaterials();

        // Create render pipeline data
        // setup buffers
        _pipeline.VertexBuffer = _batch->GetVertexBuffer();
        _pipeline.IndexBuffer = _batch->GetIndexBuffer();
        _pipeline.VSBuffers.emplace_back(_batch->GetInstanceBuffer());
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

        static cPerspectiveCameraComponent perspectiveCamera("PerspectiveCamera");
        _cameraController = new cPerspectiveCameraController(context, &perspectiveCamera, &time);
        _pipeline.VSBuffers.emplace_back(_cameraController->GetBuffer());

        _pipeline.PSBuffers.emplace_back(MaterialManager::GetBuffer());

        _pipeline.PSBuffers.emplace_back(LightManager::GetDirectBuffer());
        _pipeline.PSBuffers.emplace_back(LightManager::GetPointBuffer());
        _pipeline.PSBuffers.emplace_back(LightManager::GetSpotBuffer());
    }

    void Update() override final
    {
        {
            _cameraController->Move();

            _canvas->Clear(glm::vec4(1.0f));

            context->BindRenderPipeline(&_pipeline);

            // todo bug: canvas is not update or resized because of BindMaterials()
//            MaterialManager::BindMaterials();

            static cTransformComponent tr("transform");

            _batch->DrawAll();

            _canvas->Present();
        }
    }

    void Free() override
    {
        LogInfo("GameApp::Free()");
        delete _cameraController;
        delete _ecs;
        delete _batch;
        delete _canvas;
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
    }

private:
    Canvas* _canvas;
    ECSManager* _ecs;
    BatchManager* _batch;
    Pipeline _pipeline;
    InputLayout _layout;
    cPerspectiveCameraController* _cameraController;
};

Application* CreateApplication() {
    return new GameApp();
}
