#pragma once

#include <rendering/buffers/structure_buffer.h>

namespace xpe {

    namespace render {

        using namespace core;

        struct ENGINE_API PointLightBufferData final {
            glm::vec3 Position;
            glm::vec3 Color;
            float Constant;
            float Linear;
            float Quadratic;
        };

        struct ENGINE_API PointLightComponent : public Component {

            PointLightComponent(const string& usid) : Component(usid) {}

            PointLightComponent(const string& usid, u32 index) : Component(usid), Index(index) {}

            u32 Index = 0;
            glm::vec3 Position = { 0, 0, 0 };
            glm::vec3 Color = { 1, 1, 1 };
            float Constant;
            float Linear;
            float Quadratic;

        };

        class ENGINE_API PointLightBuffer : public StructureBuffer<PointLightBufferData> {

        public:
            PointLightBuffer() = default;

            PointLightBuffer(usize size) : StructureBuffer<PointLightBufferData>(size, K_SLOT_POINT_LIGHTS, K_FALSE) {}

        };

    }

}