#pragma once

namespace xpe {

    namespace render {

        enum class eFillMode {
            WIREFRAME,
            SOLID,

            DEFAULT = SOLID
        };

        enum class eCullMode {
            NONE,
            FRONT,
            BACK,

            DEFAULT = NONE,
        };

        struct ENGINE_API Rasterizer final {
            void*     State = nullptr;
            eFillMode FillMode = eFillMode::DEFAULT;
            eCullMode CullMode = eCullMode::DEFAULT;
            bool      FrontCounterClockwise = false;
            int       DepthBias = 0;
            float     DepthBiasClamp = 0;
            float     SlopeScaledDepthBias = 0;
            bool      DepthClipEnable = false;
            bool      ScissorEnable = false;
            bool      MultisampleEnable = false;
            bool      AntialiasedLineEnable = false;
        };

    }

}