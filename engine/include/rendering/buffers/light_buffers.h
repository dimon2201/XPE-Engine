#pragma once

#include <rendering/buffers/structure_buffer.h>

namespace xpe {

    namespace ecs
    {
        struct DirectLightComponent;
        struct PointLightComponent;
        struct SpotLightComponent;
    }

    namespace render {

        using namespace core;
        using namespace ecs;

        struct ENGINE_API DirectLightBufferData final
        {
            glm::vec3 Position = { 0, 0, 0 };
            glm::vec3 Color = { 1, 1, 1 };
        };

        struct ENGINE_API DirectLightBuffer : public StructureBuffer<DirectLightBufferData>
        {

            DirectLightBuffer(usize size = 0) : StructureBuffer<DirectLightBufferData>(size, K_SLOT_DIRECT_LIGHTS) {}

            void AddComponent(DirectLightComponent& component);

        };

        struct ENGINE_API PointLightBufferData final
        {
            glm::vec3 Position;
            glm::vec3 Color;
            float Constant;
            float Linear;
            float Quadratic;
        };

        struct ENGINE_API PointLightBuffer : public StructureBuffer<PointLightBufferData>
        {

            PointLightBuffer(usize size = 0) : StructureBuffer<PointLightBufferData>(size, K_SLOT_POINT_LIGHTS) {}

            void AddComponent(PointLightComponent& component);

        };

        struct ENGINE_API SpotLightBufferData final
        {
            glm::vec3 Position;
            glm::vec3 Direction;
            glm::vec3 Color;
            float Cutoff;
            float Outer;
        };

        struct ENGINE_API SpotLightBuffer : public StructureBuffer<SpotLightBufferData>
        {

            SpotLightBuffer(usize size = 0) : StructureBuffer<SpotLightBufferData>(size, K_SLOT_SPOT_LIGHTS) {}

            void AddComponent(SpotLightComponent& component);

        };

    }

}