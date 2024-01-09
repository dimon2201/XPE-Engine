#pragma once

#include <rendering/camera.hpp>

namespace xpe {

    namespace render {

        class ENGINE_API cCameraManager final
        {

        public:
            static void Init();
            static void Free();

            static glm::mat4 GetViewProjection();

            static void Flush();

            static void Bind();
            static void Unbind();

            static void SetExposure(float exposure);
            static void SetGamma(float gamma);
            static float GetExposure();
            static float GetGamma();

            static cCamera* GetCamera();
            static void SetCamera(cCamera* camera);

        private:
            static cCamera* s_Camera;

        };

    }

}