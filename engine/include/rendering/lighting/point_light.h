#pragma once

#include <rendering/structure_buffer.h>

namespace xpe {

    namespace render {

        using namespace core;

        struct ENGINE_API PointLightBufferData final {
            glm::vec4 Position;
            glm::vec4 Color;
            float Constant;
            float Linear;
            float Quadratic;
            float Refraction;
        };

        struct ENGINE_API PointLight final {
            u32 Index = 0;
            PointLightBufferData* Data = nullptr;
        };

        struct ENGINE_API PointLightComponent : public Component {

            PointLightComponent(const string& usid) : Component(usid) {}

            PointLight Light;

        };

        class ENGINE_API PointLightBuffer : public StructureBuffer<PointLightBufferData> {

        public:
            PointLightBuffer() = default;

            PointLightBuffer(Context* context, usize size)
            : StructureBuffer<PointLightBufferData>(context, size, K_SLOT_POINT_LIGHTS, K_FALSE) {}

        };

    }

}