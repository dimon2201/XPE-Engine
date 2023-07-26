#pragma once

#include <core/core.hpp>
#include <project_manager.h>

using namespace xpe::core;
using namespace xpe::render;
using namespace xpe::control;
using namespace xpe::ttf;
using namespace xpe::io;
using namespace xpe::math;
using namespace xpe::gltf;

namespace focus {

    class Editor : public Application, public ProjectManagerCallback
    {

    public:
        Editor() {}
        ~Editor() {}

        void Init() override final;

        void Update() override final;

        void Free();

        void GameCodeReloaded(Game* game) override;

    };

}

Application* CreateApplication() {
    return new focus::Editor();
}