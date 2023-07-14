#pragma once

#include <rendering/buffers/structure_buffer.h>

namespace xpe {

    namespace render {

        using namespace core;

        struct ENGINE_API SpotLightBufferData final {
            glm::vec4 Position;
            glm::vec4 Direction;
            glm::vec4 Color;
            float Cutoff;
            float Outer;
            float Refraction;
        };

        struct ENGINE_API SpotLight final {
            u32 Index = 0;
            SpotLightBufferData* Data = nullptr;
        };

        struct ENGINE_API SpotLightComponent : public Component {

            SpotLightComponent(const string& usid) : Component(usid) {}

            SpotLight Light;

        };

        class ENGINE_API SpotLightBuffer : public StructureBuffer<SpotLightBufferData> {

        public:
            SpotLightBuffer() = default;

            SpotLightBuffer(Context* context, usize size)
            : StructureBuffer<SpotLightBufferData>(context, size, K_SLOT_SPOT_LIGHTS, K_FALSE) {}

        };

    }

}