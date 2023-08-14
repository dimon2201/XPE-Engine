#pragma once

#include <rendering/buffers/structure_buffer.h>

namespace xpe {

    namespace render {

        struct ENGINE_API MaterialData
        {
            // base color
            glm::vec4 BaseColor = { 0.75, 0.75, 0.75, 1 };
            Bool EnableAlbedo = false;
            // bumping
            Bool EnableBumping = false;
            // parallax
            Bool EnableParallax = false;
            float HeightScale = 0.1;
            float ParallaxMinLayers = 8;
            float ParallaxMaxLayers = 32;
            // metallic
            float MetallicFactor = 0.5f;
            Bool EnableMetallic = false;
            // roughness
            float RoughnessFactor = 0.5f;
            Bool EnableRoughness = false;
            // ambient occlusion
            float AOFactor = 0.5f;
            Bool EnableAO = false;
            // emission
            glm::vec3 EmissionColor = { 0, 0, 0 };
            Bool EnableEmission = false;
        };

        struct ENGINE_API MaterialDataBuffer : public StructureBuffer<MaterialData>
        {
            MaterialDataBuffer(usize size = 0) : StructureBuffer<MaterialData>(size, K_SLOT_MATERIALS) {}
        };

    }

}