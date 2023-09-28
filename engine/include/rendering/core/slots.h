#pragma once

namespace xpe {

    namespace render {

        // ---------- For Structure Buffers --------------- //

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

        // core
        const core::u32 K_SLOT_MONITOR                 = 0;

        // materials
        const core::u32 K_SLOT_MATERIALS               = 1;
        const core::u32 K_SLOT_ALBEDO                  = 2;
        const core::u32 K_SLOT_BUMPING                 = 3;
        const core::u32 K_SLOT_PARALLAX                = 4;
        const core::u32 K_SLOT_METALLIC                = 5;
        const core::u32 K_SLOT_ROUGHNESS               = 6;
        const core::u32 K_SLOT_AO                      = 7;
        const core::u32 K_SLOT_EMISSION                = 8;

        // lights
        const core::u32 K_SLOT_DIRECT_LIGHTS           = 9;
        const core::u32 K_SLOT_POINT_LIGHTS            = 10;
        const core::u32 K_SLOT_SPOT_LIGHTS             = 11;

        // shadows
        const core::u32 K_SLOT_SHADOW_FILTER           = 12;
        const core::u32 K_SLOT_SHADOW_MAP              = 13;
        const core::u32 K_SLOT_SHADOW_COORDS           = 14;

        // ---------- For Samplers --------------- //

        // materials
        const core::u32 K_SLOT_MATERIAL_SAMPLER        = 0;

        // shadows
        const core::u32 K_SLOT_SHADOW_SAMPLER          = 1;

        // ---------- For Constant Buffers --------------- //

    }

}