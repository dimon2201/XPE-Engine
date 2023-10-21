#pragma once

#include <core/types.hpp>
#include <rendering/buffers/structure_buffer.h>

namespace xpe {

    namespace render {

        struct ENGINE_API MaterialData
        {
            // albedo mapping
            glm::vec4 Albedo = {0.9, 0.9, 0.9, 1 };
            core::Boolean EnableAlbedoMap = false;
            // normal mapping
            core::Boolean EnableNormalMap = false;
            // parallax occlusion mapping
            core::Boolean EnableParallaxMap = false;
            float ParallaxHeightScale = 0.1;
            float ParallaxMinLayers = 8;
            float ParallaxMaxLayers = 32;
            // metal mapping
            float Metallness = 0.5f;
            core::Boolean EnableMetalMap = false;
            // roughness mapping
            float Roughness = 0.5f;
            core::Boolean EnableRoughnessMap = false;
            // ambient occlusion mapping
            float AO = 0.5f;
            core::Boolean EnableAOMap = false;
            // emission mapping
            glm::vec3 Emission = {0, 0, 0 };
            core::Boolean EnableEmissionMap = false;
        };

        struct ENGINE_API MaterialDataBuffer : public StructureBuffer<MaterialData>
        {
            MaterialDataBuffer(usize size = 0) : StructureBuffer<MaterialData>(size, K_SLOT_MATERIALS) {}
        };

    }

}