#pragma once

#include <rendering/core/texture.h>

namespace xpe {

    namespace render {

        using namespace core;
        using namespace ecs;

        class ENGINE_API cRenderSystem : public cSystem
        {

        public:
            cRenderSystem(sViewport& viewport, u32 sampleCount);
            ~cRenderSystem();

            void Prepare();
            void Update(cScene* scene, const cTime& dt) override final;

            void WindowFrameResized(int width, int height);

        private:
            void InitManagers(sViewport& viewport, u32 sampleCount);
            void InitBuffers(sViewport& viewport, u32 sampleCount);
            void InitRenderTargets(sViewport& viewport, u32 sampleCount);

            void FreeManagers();
            void FreeBuffers();
            void FreeRenderTargets();

            void UpdateLight(cScene* scene);
            void UpdateShaders(cScene* scene);
        };

    }

}