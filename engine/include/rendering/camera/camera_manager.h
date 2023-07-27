#pragma once

#include <rendering/buffers/camera_buffer.h>

namespace xpe {

    namespace render {

        struct ENGINE_API CameraStorage : public Object {
            CameraBuffer CameraBuffer2D;
            CameraBuffer CameraBuffer3D;

            CameraStorage(Context* context);
        };

        class ENGINE_API CameraManager final {

        public:
            static void Init(Context* context);
            static void Free();

            static CameraBuffer* GetBuffer2D();
            static CameraBuffer* GetBuffer();

        private:
            static CameraStorage* s_Storage;
        };

    }

}