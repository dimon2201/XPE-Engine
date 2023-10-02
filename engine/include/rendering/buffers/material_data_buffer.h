#pragma once

#include <rendering/buffers/structure_buffer.h>

namespace xpe {

    namespace render {

        struct ENGINE_API MaterialData
        {
            // albedo mapping
            glm::vec4 Albedo = {0.9, 0.9, 0.9, 1 };
            Bool EnableAlbedoMap = false;
            // normal mapping
            Bool EnableNormalMap = false;
            // parallax occlusion mapping
            Bool EnableParallaxMap = false;
            float ParallaxHeightScale = 0.1;
            float ParallaxMinLayers = 8;
            float ParallaxMaxLayers = 32;
            // metal mapping
            float Metallness = 0.5f;
            Bool EnableMetalMap = false;
            // roughness mapping
            float Roughness = 0.5f;
            Bool EnableRoughnessMap = false;
            // ambient occlusion mapping
            float AO = 0.5f;
            Bool EnableAOMap = false;
            // emission mapping
            glm::vec3 Emission = {0, 0, 0 };
            Bool EnableEmissionMap = false;
        };

        struct ENGINE_API MaterialDataBuffer : public StructureBuffer<MaterialData>
        {
            MaterialDataBuffer(usize size = 0) : StructureBuffer<MaterialData>(size, K_SLOT_MATERIALS) {}
        };

    }

}