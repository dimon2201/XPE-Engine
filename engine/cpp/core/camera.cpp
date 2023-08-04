#include <core/memory_pool.hpp>
#include <core/camera.h>
#include <rendering/buffers/camera_buffer.h>

xpe::render::CameraBuffer* xpe::core::CameraManager::s_Buffer = nullptr;
xpe::render::CameraBuffer* xpe::core::CameraManager::s_Buffer2d = nullptr;

void xpe::core::CameraManager::Init(xpe::render::Context* context) {
    s_Buffer = new xpe::render::CameraBuffer(context, 1);
    s_Buffer2d = new xpe::render::CameraBuffer(context, 1);
}

void xpe::core::CameraManager::Free()
{
    delete s_Buffer;
    delete s_Buffer2d;
}

xpe::render::CameraBuffer* xpe::core::CameraManager::GetCameraBuffer()
{
    return s_Buffer;
}

xpe::render::CameraBuffer* xpe::core::CameraManager::GetCameraBuffer2D()
{
    return s_Buffer2d;
}