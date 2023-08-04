#pragma once

#include <rendering/core/context.hpp>
#include <rendering/buffers/camera_buffer.h>

namespace xpe
{
    namespace core
    {
        class ENGINE_API CameraManager final {

        public:
            static void Init(xpe::render::Context* context);
            static void Free();

            static render::CameraBuffer* GetCameraBuffer();
            static render::CameraBuffer* GetCameraBuffer2D();

        private:
            static render::CameraBuffer* s_Buffer;
            static render::CameraBuffer* s_Buffer2d;
        };
    }
}