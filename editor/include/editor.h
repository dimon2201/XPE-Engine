#pragma once

#include <core/app.hpp>
#include <core/project_manager.h>

using namespace xpe::core;
using namespace xpe::render;
using namespace xpe::math;

namespace focus {

    namespace core {

        class Editor : public Application, public ProjectManagerCallback
        {

        public:
            Editor() {}
            ~Editor() {}

            void Init() override final;
            void Update() override final;
            void Free() override final;

        private:
            void LoadGame(Application* game);

        };

    }

}