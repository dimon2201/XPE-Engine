#pragma once

namespace xpe {

    namespace render {

        // ---------- For Structure Buffer --------------- //

        // ---------- For Vertex Stage --------------- //

        const core::u32 K_SLOT_INSTANCES               = 0;
        const core::u32 K_SLOT_TRANSFORMS              = 1;
        const core::u32 K_SLOT_TRANSFORMS2D            = 2;
        const core::u32 K_SLOT_CAMERAS                 = 3;

        // ---------- For Pixel Stage --------------- //

        const core::u32 K_SLOT_MATERIALS               = 0;
        const core::u32 K_SLOT_ALBEDO                  = 1;
        const core::u32 K_SLOT_BUMPING                 = 2;
        const core::u32 K_SLOT_PARALLAX                = 3;
        const core::u32 K_SLOT_METALLIC                = 4;
        const core::u32 K_SLOT_ROUGHNESS               = 5;
        const core::u32 K_SLOT_AO                      = 6;
        const core::u32 K_SLOT_EMISSION                = 7;

        const core::u32 K_SLOT_DIRECT_LIGHTS           = 8;
        const core::u32 K_SLOT_POINT_LIGHTS            = 9;
        const core::u32 K_SLOT_SPOT_LIGHTS             = 10;
        const core::u32 K_SLOT_ENV_LIGHTS              = 11;

        // ---------- For Constant Buffer --------------- //

    }

}