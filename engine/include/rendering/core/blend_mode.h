#pragma once

namespace xpe {

    namespace render {

        enum class eBlend {
            ZERO = 1,
            ONE	= 2,
            SRC_COLOR = 3,
            INV_SRC_COLOR = 4,
            SRC_ALPHA = 5,
            INV_SRC_ALPHA = 6,
            DEST_ALPHA = 7,
            INV_DEST_ALPHA = 8,
            DEST_COLOR = 9,
            INV_DEST_COLOR = 10,
            SRC_ALPHA_SAT = 11,
            BLEND_FACTOR = 14,
            INV_BLEND_FACTOR = 15,
            SRC1_COLOR = 16,
            INV_SRC1_COLOR = 17,
            SRC1_ALPHA = 18,
            INV_SRC1_ALPHA = 19
        };

        enum class eBlendOp {
            ADD	= 1,
            SUBTRACT = 2,
            REV_SUBTRACT = 3,
            MIN	= 4,
            MAX	= 5
        };

        enum eColorWriteEnableMask : u8 {
            RED	= 1,
            GREEN = 2,
            BLUE = 4,
            ALPHA = 8,
            ALL = (((RED | GREEN) | BLUE) | ALPHA)
        };

        struct ENGINE_API BlendTarget : public core::Object
        {
            bool Enable = false;
            eBlend Src = eBlend::ONE;
            eBlend Dest = eBlend::ZERO;
            eBlendOp BlendOp = eBlendOp::ADD;
            eBlend SrcAlpha = eBlend::ONE;
            eBlend DestAlpha = eBlend::ZERO;
            eBlendOp BlendOpAlpha = eBlendOp::ADD;
            u8 RenderTargetWriteMask = eColorWriteEnableMask::ALL;
        };

        struct ENGINE_API BlendMode : public core::Object
        {
            void* State = nullptr;
            bool AlphaToCoverageEnable = false;
            bool IndependentBlendEnable = false;
            // blend mode can only have maximum 8 targets per passess
            array<BlendTarget, 8> Targets;
        };

    }

}