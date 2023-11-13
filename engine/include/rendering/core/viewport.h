#pragma once

namespace xpe {

    namespace render {

        struct ENGINE_API sViewport : public core::cObject
        {
            float Left = 0;
            float Top = 0;
            float Width = 0;
            float Height = 0;
            float MinDepth = 0;
            float MaxDepth = 1;
        };

    }

}