#pragma once

#include <rendering/buffers/structure_buffer.h>

namespace xpe {

    namespace render {

        using namespace core;

        struct ENGINE_API DirectLightBufferData final {
            glm::vec3 Position = { 0, 0, 0 };
            glm::vec3 Color = { 1, 1, 1 };
        };

        struct ENGINE_API DirectLight final {
            u32 Index = 0;
            DirectLightBufferData* Data = nullptr;
        };

        struct ENGINE_API DirectLightComponent : public Component {

            DirectLightComponent(const string& usid) : Component(usid) {}

            DirectLight Light;

        };

        class ENGINE_API DirectLightBuffer : public StructureBuffer<DirectLightBufferData> {

        public:
            DirectLightBuffer() = default;

            DirectLightBuffer(Context* context, usize size)
            : StructureBuffer<DirectLightBufferData>(context, size, K_SLOT_DIRECT_LIGHTS, K_FALSE) {}

        };

    }

}