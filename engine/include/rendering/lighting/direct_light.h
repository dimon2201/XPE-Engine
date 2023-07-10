#pragma once

#include <rendering/structure_buffer.h>

namespace xpe {

    namespace render {

        using namespace core;

        struct ENGINE_API DirectLightBufferData final {
            glm::vec4 Position;
            glm::vec4 Color;
        };

        struct ENGINE_API DirectLight final {
            u32 Index = 0;
            DirectLightBufferData* Data = nullptr;
        };

        struct ENGINE_API cDirectLightComponent : public cComponent {

            cDirectLightComponent(const string& usid) : cComponent(usid) {}

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