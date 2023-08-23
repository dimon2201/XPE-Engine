#pragma once

namespace xpe {

    namespace render {

        enum class eDepthWriteMask {
            ZERO = 0,
            ALL = 1
        };

        enum class eDepthStencilFunc {
            NEVER = 1,
            LESS = 2,
            EQUAL = 3,
            LESS_EQUAL = 4,
            GREATER = 5,
            NOT_EQUAL = 6,
            GREATER_EQUAL = 7,
            ALWAYS = 8,
        };

        enum class eDepthStencilOp {
            KEEP = 1,
            ZERO = 2,
            REPLACE = 3,
            INCR_SAT = 4,
            DECR_SAT = 5,
            INVERT = 6,
            INCR = 7,
            DECR = 8
        };

        struct ENGINE_API StencilTest : public core::Object
        {
            eDepthStencilOp StencilFailOp = eDepthStencilOp::KEEP;
            eDepthStencilOp StencilDepthFailOp = eDepthStencilOp::KEEP;
            eDepthStencilOp StencilPassOp = eDepthStencilOp::KEEP;
            eDepthStencilFunc StencilFunc = eDepthStencilFunc::ALWAYS;
        };

        struct ENGINE_API DepthStencilMode : public core::Object
        {
            void* State = nullptr;
            bool EnableDepth = true;
            eDepthWriteMask DepthWriteMask = eDepthWriteMask::ALL;
            eDepthStencilFunc DepthFunc = eDepthStencilFunc::LESS;
            bool EnableStencil = false;
            u8 StencilReadMask = 0xff;
            u8 StencilWriteMask = 0xff;
            StencilTest FrontFace;
            StencilTest BackFace;
        };

    }

}