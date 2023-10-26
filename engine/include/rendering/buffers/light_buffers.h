#pragma once

#include <rendering/buffers/structure_buffer.h>

namespace xpe {

    namespace render {

        using namespace core;

        struct ENGINE_API DirectLightData
        {
            glm::vec3 Position = { 0, 0, 0 };
            glm::vec3 Color = { 1, 1, 1 };
        };

        struct ENGINE_API DirectLightBuffer : public StructureBuffer<DirectLightData>
        {
            DirectLightBuffer(usize size = 0) : StructureBuffer<DirectLightData>(size, K_SLOT_DIRECT_LIGHTS) {}
        };

        struct ENGINE_API PointLightData
        {
            glm::vec3 Position = { 0, 0, 0 };
            glm::vec3 Color = { 1, 1, 1 };
            float Constant = 1.0f;
            float Linear = 0.09f;
            float Quadratic = 0.032f;
        };

        struct ENGINE_API PointLightBuffer : public StructureBuffer<PointLightData>
        {
            PointLightBuffer(usize size = 0) : StructureBuffer<PointLightData>(size, K_SLOT_POINT_LIGHTS) {}
        };

        struct ENGINE_API SpotLightData
        {
            glm::vec3 Position = { 0, 0, 0 };
            glm::vec3 Direction = { 1, 1, 1 };
            glm::vec3 Color = { 1, 1, 1 };
            float Cutoff = 1;
            float Outer = 1;
        };

        struct ENGINE_API SpotLightBuffer : public StructureBuffer<SpotLightData>
        {
            SpotLightBuffer(usize size = 0) : StructureBuffer<SpotLightData>(size, K_SLOT_SPOT_LIGHTS) {}
        };

    }

}