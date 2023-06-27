#pragma once

namespace xpe
{
    namespace core
    {
        struct cTransformComponent;
        struct cViewerComponent;
        class cUserInputManager;
    }

    namespace viewer
    {
        void ENGINE_API ViewerUpdate(core::f32 time, core::cUserInputManager* ui, core::cTransformComponent* transform, core::cViewerComponent* viewer);
    }
}