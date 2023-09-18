#pragma once

namespace xpe {

    namespace render {

        // ---------- For Structure Buffer --------------- //

        // ---------- For Vertex Stage --------------- //

        const core::u32 K_SLOT_INSTANCES               = 0;
        const core::u32 K_SLOT_INSTANCES2D             = 1;
        const core::u32 K_SLOT_TRANSFORMS              = 2;
        const core::u32 K_SLOT_TRANSFORMS2D            = 3;
        const core::u32 K_SLOT_CAMERA                  = 4;
        const core::u32 K_SLOT_TEXT_CHARS              = 5;
        const core::u32 K_SLOT_VIEWPORT                = 6;
        const core::u32 K_SLOT_BONES                   = 7;

        // ---------- For Pixel Stage --------------- //

        const core::u32 K_SLOT_MONITOR                 = 0;
        const core::u32 K_SLOT_MATERIALS               = 1;
        const core::u32 K_SLOT_ALBEDO                  = 2;
        const core::u32 K_SLOT_BUMPING                 = 3;
        const core::u32 K_SLOT_PARALLAX                = 4;
        const core::u32 K_SLOT_METALLIC                = 5;
        const core::u32 K_SLOT_ROUGHNESS               = 6;
        const core::u32 K_SLOT_AO                      = 7;
        const core::u32 K_SLOT_EMISSION                = 8;

        const core::u32 K_SLOT_DIRECT_LIGHTS           = 9;
        const core::u32 K_SLOT_POINT_LIGHTS            = 10;
        const core::u32 K_SLOT_SPOT_LIGHTS             = 11;
        const core::u32 K_SLOT_ENV_LIGHTS              = 12;

        // ---------- For Constant Buffer --------------- //

    }

}