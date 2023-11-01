#pragma once

#include <rendering/camera.h>

namespace xpe {

    namespace render {

        class ENGINE_API cCameraManager final
        {

        public:
            static void Init();
            static void Free();

            static sCameraBuffer* GetBuffer();

            static void Flush();

            static void Bind();
            static void Unbind();

            static void SetExposure(float exposure);
            static void SetGamma(float gamma);
            static float GetExposure();
            static float GetGamma();

            static void SetViewport(const sViewport& viewport);
            static sViewport* GetViewport();

            static cPerspectiveCamera* AddPerspectiveCamera(int width, int height);
            static cOrthoCamera* AddOrthoCamera(int width, int height);
            static cCamera* GetCamera();

        private:
            static sCameraBuffer* s_Buffer;
            static cCamera* s_Camera;

        };

    }

}