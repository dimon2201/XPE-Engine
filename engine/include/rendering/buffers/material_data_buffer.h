#pragma once

#include <rendering/buffers/structure_buffer.h>

namespace xpe {

    namespace render {

        struct ENGINE_API sMaterialData
        {
            // albedo mapping
            glm::vec4 Albedo = { 0.5, 0.5, 0.5, 1.0 };
            Boolean EnableAlbedoMap = false;
            // normal mapping
            Boolean EnableNormalMap = false;
            // parallax occlusion mapping
            Boolean EnableParallaxMap = false;
            float ParallaxHeightScale = 0.1;
            float ParallaxMinLayers = 8;
            float ParallaxMaxLayers = 32;
            // metal mapping
            float Metallness = 0.5f;
            Boolean EnableMetalMap = false;
            // roughness mapping
            float Roughness = 0.5f;
            Boolean EnableRoughnessMap = false;
            // ambient occlusion mapping
            float AO = 0.5f;
            Boolean EnableAOMap = false;
            // emission mapping
            glm::vec3 Emission = { 0, 0, 0 };
            Boolean EnableEmissionMap = false;
        };

        struct ENGINE_API sMaterialDataBuffer : public sListBuffer<sMaterialData>
        {
            sMaterialDataBuffer(usize size = 0) : sListBuffer<sMaterialData>(size, K_SLOT_MATERIALS) {}
        };

    }

}