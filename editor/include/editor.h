#pragma once

#include <core/core.hpp>

using namespace xpe::core;
using namespace xpe::render;
using namespace xpe::control;
using namespace xpe::ttf;
using namespace xpe::io;
using namespace xpe::math;
using namespace xpe::gltf;

namespace focus {

    class Editor : public Application
    {

    public:
        Editor() {}
        ~Editor() {}

        void Init() override final;

        void Update() override final;

        void Free();

        void WindowClosed();

        void KeyPressed(const eKey key);

        void KeyHold(const eKey key);

        void CursorMoved(const double x, const double y);

    private:

        void InitPipeline();

        void InitPipeline2D();

        void InitCamera();

        void InitCamera2D();

        void UpdateCamera();

        void MoveLight(const eKey key);

        void Simulate();

    private:
        Canvas* m_Canvas;
        ECSManager* m_ECS;

        BatchManager* m_BatchManager;
        BatchManager2d* m_BatchManager2d;

        Pipeline m_Pipeline;
        Pipeline m_Pipeline2d;

        InputLayout m_Layout;
        InputLayout m_Layout2d;

        PerspectiveCamera* m_Camera;
        OrthoCamera* m_Camera2D;

        DirectLightComponent m_DirectLightComponent = string("DirectLight", 0);

        PerspectiveCameraComponent m_PerspectiveCameraComponent = string("PerspectiveCamera");
        OrthoCameraComponent m_OrthoCameraComponent = string("OrthoCamera");

    };

}

Application* CreateApplication() {
    return new focus::Editor();
}