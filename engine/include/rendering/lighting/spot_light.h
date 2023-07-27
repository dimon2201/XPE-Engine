#pragma once

#include <rendering/buffers/structure_buffer.h>

namespace xpe {

    namespace render {

        using namespace core;

        struct ENGINE_API SpotLightBufferData final {
            glm::vec3 Position;
            glm::vec3 Direction;
            glm::vec3 Color;
            float Cutoff;
            float Outer;
        };

        struct ENGINE_API SpotLightComponent : public Component {

            SpotLightComponent(const string& usid) : Component(usid) {}

            SpotLightComponent(const string& usid, u32 index) : Component(usid), Index(index) {}

            u32 Index = 0;
            glm::vec3 Position = { 0, 0, 0 };
            glm::vec3 Direction = { 0, 0, 0 };
            glm::vec3 Color = { 1, 1, 1 };
            float Cutoff;
            float Outer;

        };

        class ENGINE_API SpotLightBuffer : public StructureBuffer<SpotLightBufferData> {

        public:
            SpotLightBuffer() = default;

            SpotLightBuffer(Context* context, usize size)
            : StructureBuffer<SpotLightBufferData>(context, size, K_SLOT_SPOT_LIGHTS, K_FALSE) {}

        };

    }

}