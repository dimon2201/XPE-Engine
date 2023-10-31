#pragma once

#include <core/app.hpp>
#include <core/project_manager.h>

using namespace xpe::core;
using namespace xpe::render;
using namespace xpe::math;

namespace focus {

    namespace core {

        class cEditor : public cApp, public sProjectManagerCallback
        {

        public:
            cEditor() {}
            ~cEditor() {}

            void Init() override final;
            void Update() override final;
            void Free() override final;

        private:
            void LoadGame(cApp* game);

        };

    }

}