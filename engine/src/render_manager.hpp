#pragma once

#include <texture_manager.hpp>
#include <components.hpp>

namespace xpe
{
    namespace render
    {
        class ENGINE_API MRender final
        {

        public:
            static void Init(sViewport& viewport, u32 sampleCount);
            static void Free();
            static void Prepare();
            static void Update(cScene* scene, float dt);

            static void WindowFrameResized(int width, int height);

        private:
            static void InitManagers(sViewport& viewport, u32 sampleCount);
            static void InitBuffers(sViewport& viewport, u32 sampleCount);
            static void InitRenderTargets(sViewport& viewport, u32 sampleCount);

            static void FreeManagers();
            static void FreeBuffers();
            static void FreeRenderTargets();

            static void UpdateLight(cScene* scene);
            static void UpdateShaders(cScene* scene);
        };
    }
}