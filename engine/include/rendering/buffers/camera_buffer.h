#pragma once

#include <rendering/buffers/structure_buffer.h>

#include <components/camera_components.h>

namespace xpe {

    namespace render {

        using namespace ecs;

        struct ENGINE_API CameraBufferData final {
            glm::vec3 Position;
            glm::mat4 View;
            glm::mat4 Projection;
        };

        class ENGINE_API CameraBuffer : public render::StructureBuffer<CameraBufferData> {

        public:
            CameraBuffer() = default;
            CameraBuffer(Context* context, usize count)
            : render::StructureBuffer<CameraBufferData>(context, count, K_SLOT_CAMERAS, K_FALSE) {}

        };

    }

}