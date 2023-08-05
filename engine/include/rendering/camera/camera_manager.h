#pragma once

#include <rendering/buffers/camera_buffer.h>

namespace xpe {

    namespace render {

        struct ENGINE_API CameraStorage final
        {
            CameraBuffer Buffer2D;
            CameraBuffer Buffer3D;

            CameraStorage();
        };

        class ENGINE_API CameraManager final
        {

        public:
            static void Init();
            static void Free();

            static CameraBuffer* GetBuffer();
            static CameraBuffer* GetBuffer2D();

        private:
            static CameraStorage* s_Storage;

        };

    }

}