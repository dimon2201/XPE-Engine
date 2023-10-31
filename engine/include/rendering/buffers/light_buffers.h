#pragma once

#include <rendering/buffers/structure_buffer.h>

namespace xpe {

    namespace render {

        using namespace core;

        struct ENGINE_API sDirectLightData
        {
            glm::vec3 Position = { 0, 0, 0 };
            glm::vec3 Color = { 1, 1, 1 };
        };

        struct ENGINE_API sDirectLightBuffer : public sListBuffer<sDirectLightData>
        {
            sDirectLightBuffer(usize size = 0) : sListBuffer<sDirectLightData>(size, K_SLOT_DIRECT_LIGHTS) {}
        };

        struct ENGINE_API sPointLightData
        {
            glm::vec3 Position = { 0, 0, 0 };
            glm::vec3 Color = { 1, 1, 1 };
            float Constant = 1.0f;
            float Linear = 0.09f;
            float Quadratic = 0.032f;
        };

        struct ENGINE_API sPointLightBuffer : public sListBuffer<sPointLightData>
        {
            sPointLightBuffer(usize size = 0) : sListBuffer<sPointLightData>(size, K_SLOT_POINT_LIGHTS) {}
        };

        struct ENGINE_API sSpotLightData
        {
            glm::vec3 Position = { 0, 0, 0 };
            glm::vec3 Direction = { 1, 1, 1 };
            glm::vec3 Color = { 1, 1, 1 };
            float Cutoff = 1;
            float Outer = 1;
        };

        struct ENGINE_API sSpotLightBuffer : public sListBuffer<sSpotLightData>
        {
            sSpotLightBuffer(usize size = 0) : sListBuffer<sSpotLightData>(size, K_SLOT_SPOT_LIGHTS) {}
        };

    }

}