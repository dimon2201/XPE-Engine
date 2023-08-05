#include <rendering/camera/camera_manager.h>

namespace xpe {

    namespace render {

        CameraStorage* CameraManager::s_Storage = nullptr;

        CameraStorage::CameraStorage()
        {
            Buffer2D = CameraBuffer(1);
            Buffer3D = CameraBuffer(1);
        }

        void CameraManager::Init()
        {
            s_Storage = new CameraStorage();
        }

        void CameraManager::Free()
        {
            delete s_Storage;
        }

        CameraBuffer *CameraManager::GetBuffer2D()
        {
            return &s_Storage->Buffer2D;
        }

        CameraBuffer *CameraManager::GetBuffer()
        {
            return &s_Storage->Buffer3D;
        }

    }

}