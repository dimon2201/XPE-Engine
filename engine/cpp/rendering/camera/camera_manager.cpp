#include <rendering/camera/camera_manager.h>

namespace xpe {

    namespace render {

        CameraStorage* CameraManager::s_Storage = nullptr;

        CameraStorage::CameraStorage(Context *context)
        {
            CameraBuffer2D = CameraBuffer(context, 1);
            CameraBuffer3D = CameraBuffer(context, 1);
        }

        void CameraManager::Init(Context* context)
        {
            s_Storage = new CameraStorage(context);
        }

        void CameraManager::Free()
        {
            delete s_Storage;
        }

        CameraBuffer *CameraManager::GetBuffer2D() {
            return &s_Storage->CameraBuffer2D;
        }

        CameraBuffer *CameraManager::GetBuffer() {
            return &s_Storage->CameraBuffer3D;
        }

    }

}