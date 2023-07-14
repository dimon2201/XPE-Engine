#pragma once

#include <rendering/buffers/structure_buffer.h>

#include <components/camera_components.h>

namespace xpe {

    namespace render {

        using namespace ecs;

        struct ENGINE_API CameraBufferData final {
            glm::mat4 Projection;
            glm::mat4 View;
            glm::vec3 Position;
        };

        class ENGINE_API CameraBuffer : public render::StructureBuffer<CameraBufferData> {

        public:
            CameraBuffer() = default;
            CameraBuffer(Context* context, usize size)
            : render::StructureBuffer<CameraBufferData>(context, size, K_SLOT_CAMERAS, K_FALSE) {}

        public:
            void SetCamera(PerspectiveCameraComponent* camera);
            void SetCamera(OrthoCameraComponent* camera);
            void SetPerspectiveProjection(u32 index, const math::PerspectiveMatrix& projection);
            void SetOrthoProjection(u32 index, const math::OrthoMatrix& projection);
            void SetView(CameraComponent* camera);
            void SetPosition(CameraComponent* camera);

        };

    }

}