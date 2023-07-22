#pragma once

namespace xpe {

    namespace render {

        struct ENGINE_API Viewport final
        {
            float TopLeftX = 0;
            float TopLeftY = 0;
            float Width = 0;
            float Height = 0;
            float MinDepth = 0;
            float MaxDepth = 1;
        };

    }

}