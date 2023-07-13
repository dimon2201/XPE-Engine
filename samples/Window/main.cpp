#include <core/core.hpp>

#include "test_config.h"

using namespace xpe::core;
using namespace xpe::render;
using namespace xpe::control;
using namespace xpe::ttf;
using namespace xpe::io;
using namespace xpe::math;

class GameApp;

class GameApp : public Application
{
public:
    GameApp() {}
    ~GameApp() {}

    void Init() override final
    {
        LogInfo("GameApp::Init()");

        _testConfig = TestConfigReader::Read("test_config.json");

        Input::WindowClosedEvents.AddEvent(this, OnWindowClosed<GameApp>, 1);
        Input::KeyPressedEvents.AddEvent(this, OnKeyPressed<GameApp>, 1);
        Input::KeyHoldEvents.AddEvent(this, OnKeyHold<GameApp>, 1);
        Input::CursorMovedEvents.AddEvent(this, OnCursorMoved<GameApp>, 1);

        _canvas = new Canvas(WindowManager::GetWidth(), WindowManager::GetHeight(), context);
        _ecs = new ECSManager();
        _batch = new BatchManager(context);

        Font font = TTFManager::Load("resources/fonts/Roboto-Italic.ttf", 32);
        TTFManager::Free(font);

        TextureCubeFile textureCubeFile;
        textureCubeFile.Name = "test";
        textureCubeFile.FrontFilepath = "resources/skybox/front.jpg";
        textureCubeFile.BackFilepath = "resources/skybox/back.jpg";
        textureCubeFile.RightFilepath = "resources/skybox/right.jpg";
        textureCubeFile.LeftFilepath = "resources/skybox/left.jpg";
        textureCubeFile.TopFilepath = "resources/skybox/top.jpg";
        textureCubeFile.BottomFilepath = "resources/skybox/bottom.jpg";

        Texture* textureCube = TextureManager::LoadTextureCubeFile(textureCubeFile, Texture::eFormat::RGBA8);

//        PlaneGeometry plane;
//        _batch->StoreGlobalGeometryData(
//                "PlaneGeometry",
//                plane.Vertices.GetData(),
//                plane.Vertices.Size(),
//                plane.Vertices.Count(),
//                plane.Indices.GetData(),
//                plane.Indices.Size(),
//                plane.Indices.Count()
//        );
//
//        RenderInstance planeInstance;
//        TransformComponent planeTransform("PlaneTransform");
//        planeTransform.Position = { 0, -50, 0 };
//        TransformManager::UpdateTransform(0, planeTransform);
//        _batch->AddInstance("PlaneGeometry", planeInstance);

//        Model3D cubeModel;
//        bool cubeImported = GLTFImporter::Import("resources/cube.gltf", cubeModel);
//        Mesh& cubeMesh = cubeModel[0];

        Model3D cubeModel = xpe::gltf::GLTFImporter::Import("resources/cube.gltf");
        Mesh& cubeMesh = cubeModel[0];
        _batch->StoreGlobalGeometryData(
                "CubeMesh",
                cubeMesh.Vertices.GetData(),
                cubeMesh.Vertices.Size(),
                cubeMesh.Vertices.Count(),
                cubeMesh.Indices.GetData(),
                cubeMesh.Indices.Size(),
                cubeMesh.Indices.Count()
        );

//        CubeGeometry cube;
//        _batch->StoreGlobalGeometryData(
//                "CubeGeometry",
//                cube.Vertices.GetData(),
//                cube.Vertices.Size(),
//                cube.Vertices.Count(),
//                cube.Indices.GetData(),
//                cube.Indices.Size(),
//                cube.Indices.Count()
//        );

//        SphereGeometry sphere;
//        _batch->StoreGlobalGeometryData(
//                "SphereGeometry",
//                sphere.Vertices.GetData(),
//                sphere.Vertices.Size(),
//                sphere.Vertices.Count(),
//                sphere.Indices.GetData(),
//                sphere.Indices.Size(),
//                sphere.Indices.Count()
//        );

        // Put instances of geometry
        u32 transformIndex = 1;
        for (f32 y = -50.0f; y < 50.0f; y += 4.0f)
        {
            for (f32 x = -50.0f; x < 50.0f; x += 4.0f)
            {
                u32 materialIndex = 0;
                for (f32 z = -50.0f; z < 50.0f; z += 4.0f)
                {
                    float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
                    float g = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
                    float b = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);

                    TransformComponent transformComponent("Transform_" + transformIndex);
                    transformComponent.Position = { x, y, z };
                    transformComponent.Rotation = { r * 360.0f, g * 360.0f, b * 360.0f };
                    transformComponent.Scale    = { r, g, b };

                    TransformManager::UpdateTransform(transformIndex, transformComponent);

                    RenderInstance instance;
                    instance.TransformIndex = transformIndex;
                    instance.MaterialIndex = materialIndex;

                    Material* material = MaterialManager::Builder().Build("Material_" + materialIndex);
                    material->Index = materialIndex;
                    material->Data = MaterialManager::GetMaterialData(materialIndex);
                    material->Data->BaseColor = { r, g, b, 1 };
                    material->Data->MetallicFactor = r;
                    material->Data->RoughnessFactor = g;
                    material->Data->AOFactor = b;

                    _batch->AddInstance("CubeGeometry", instance);
                    _batch->AddInstance("CubeMesh", instance);
                    _batch->AddInstance("SphereGeometry", instance);

                    transformIndex++;
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
        _pipeline.VSBuffers.emplace_back(_batch->GetInstance2DBuffer());
        _pipeline.VSBuffers.emplace_back(TransformManager::GetBuffer());
        _pipeline.VSBuffers.emplace_back(TransformManager::GetBuffer2D());
        _pipeline.VSBuffers.emplace_back(&m_CameraBuffer);
        _pipeline.PSBuffers.emplace_back(MaterialManager::GetBuffer());
        _pipeline.PSBuffers.emplace_back(LightManager::GetDirectBuffer());
        _pipeline.PSBuffers.emplace_back(LightManager::GetPointBuffer());
        _pipeline.PSBuffers.emplace_back(LightManager::GetSpotBuffer());

        // setup shader
        _pipeline.Shader = ShaderManager::Builder()
                .AddVertexStageFromFile("shaders/window.vs")
                .AddPixelStageFromFile("shaders/window.ps")
                .Build();
        _pipeline.Shader->PrimitiveTopology = ePrimitiveTopology::TRIANGLE_LIST;

        // setup input layout
        _layout.PrimitiveTopology = ePrimitiveTopology::TRIANGLE_LIST;
        _layout.Format = Vertex3D::Format;
        _pipeline.InputLayout = _layout;

        // setup render target
        _pipeline.RenderTarget = _canvas->GetRenderTarget();
        _pipeline.DepthStencilState.UseDepthTest = K_TRUE;

        // init pipeline
        context->CreateRenderPipeline(_pipeline);

        static PerspectiveCameraComponent perspectiveCamera("PerspectiveCamera");
        perspectiveCamera.Projection.Far = _testConfig.CameraFar;
        _camera = new PerspectiveCamera(&m_CameraBuffer, &perspectiveCamera, &DeltaTime);
        _camera->MoveSpeed = _testConfig.CameraMoveSpeed;
        _camera->ZoomSpeed = _testConfig.CameraZoomSpeed;
        _camera->HorizontalSensitivity = _testConfig.CameraHorizontalSens;
        _camera->VerticalSensitivity = _testConfig.CameraVerticalSens;

        // todo maybe we will automate it in future and make it more easy to use
        LightManager::InitLight(directLightComponent.Light);
        directLightComponent.Light.Data->Position = { 0, 0, 0 };
        directLightComponent.Light.Data->Color = { 1, 1, 1 };
        LightManager::UpdateLight(directLightComponent.Light);
    }

    void Update() override final
    {
        {
            _camera->Move();

            Simulate();

            _canvas->Clear(glm::vec4(1.0f));

            context->BindRenderPipeline(&_pipeline);

            // todo bug: canvas is not update or resized because of BindMaterials()
//            MaterialManager::BindMaterials();

            _batch->DrawAll();
            _batch->DrawAll2D();

            _canvas->Present();
        }
    }

    void Free()
    {
        LogInfo("GameApp::Free()");
        delete _camera;
        delete _ecs;
        delete _batch;
        delete _canvas;
    }

    void WindowClosed()
    {
        LogWarning("GameApp::WindowClosed()");
    }

    void KeyPressed(const eKey key)
    {
        if (key == eKey::Esc)
        {
            WindowManager::Close();
        }

        MoveLight(key);
    }

    void KeyHold(const eKey key)
    {
        MoveLight(key);
    }

    void CursorMoved(const double x, const double y)
    {
    }

private:

    void MoveLight(const eKey key) {
        if (key == eKey::Up) {
            glm::vec3& pos = directLightComponent.Light.Data->Position;
            pos.y += 1;
            LightManager::UpdateLight(directLightComponent.Light);
        }

        if (key == eKey::Down) {
            glm::vec3& pos = directLightComponent.Light.Data->Position;
            pos.y -= 1;
            LightManager::UpdateLight(directLightComponent.Light);
        }

        if (key == eKey::Left) {
            glm::vec3& pos = directLightComponent.Light.Data->Position;
            pos.x -= 1;
            LightManager::UpdateLight(directLightComponent.Light);
        }

        if (key == eKey::Right) {
            glm::vec3& pos = directLightComponent.Light.Data->Position;
            pos.x += 1;
            LightManager::UpdateLight(directLightComponent.Light);
        }
    }

    void Simulate() {
        if (_testConfig.AnimateLight) {
            auto& pos = directLightComponent.Light.Data->Position;

            // translation light up and down every N ticks
            static int tick = 1;
            pos.y = 100 * sin(tick++ / 3000.0f);

            // update light color every N ticks
            if (tick % 10000 == 0) {
                auto& color = directLightComponent.Light.Data->Color;
                float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
                float g = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
                float b = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
                color = { r, g, b };
            }

            LightManager::UpdateLight(directLightComponent.Light);
        }
    }

private:
    Canvas* _canvas;
    ECSManager* _ecs;
    BatchManager* _batch;
    Pipeline _pipeline;
    InputLayout _layout;
    PerspectiveCamera* _camera;
    DirectLightComponent directLightComponent = string("DirectLight");
    TestConfig _testConfig;

};

Application* CreateApplication() {
    return new GameApp();
}
