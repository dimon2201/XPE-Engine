#pragma once

#include <rendering/buffers/structure_buffer.h>

namespace xpe {

    namespace render {

        struct ENGINE_API MaterialData
        {
            // base color
            glm::vec4 BaseColor = { 0.9, 0.9, 0.9, 1 };
            core::Boolean EnableAlbedo = core::K_FALSE;
            // bumping
            core::Boolean EnableBumping = core::K_FALSE;
            // parallax
            core::Boolean EnableParallax = core::K_FALSE;
            float HeightScale = 0.1;
            float ParallaxMinLayers = 8;
            float ParallaxMaxLayers = 32;
            // metallic
            float MetallicFactor = 0.5f;
            core::Boolean EnableMetallic = core::K_FALSE;
            // roughness
            float RoughnessFactor = 0.5f;
            core::Boolean EnableRoughness = core::K_FALSE;
            // ambient occlusion
            float AOFactor = 0.5f;
            core::Boolean EnableAO = core::K_FALSE;
            // emission
            glm::vec3 EmissionColor = { 0, 0, 0 };
            core::Boolean EnableEmission = core::K_FALSE;
            // transparency
            core::Boolean EnableTransparency = core::K_FALSE;
        };

        struct ENGINE_API MaterialDataBuffer : public StructureBuffer<MaterialData>
        {
            MaterialDataBuffer(usize size = 0) : StructureBuffer<MaterialData>(size, K_SLOT_MATERIALS) {}
        };

    }

}