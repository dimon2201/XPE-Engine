#pragma once

#include <rendering/core/core.hpp>

namespace xpe {

    namespace render {

        using namespace core;
        using namespace ecs;

        class ENGINE_API cRenderSystem : public cSystem
        {

        public:
            cRenderSystem(const glm::vec2& windowSize, u32 sampleCount);
            ~cRenderSystem();

            void Prepare();
            void Update(cScene* scene, const cTime& dt) override final;

            void WindowFrameResized(int width, int height);

        private:
            void InitManagers(const glm::vec2& windowSize, u32 sampleCount);
            void InitBuffers(const glm::vec2& windowSize, u32 sampleCount);
            void InitRenderTargets(const glm::vec2& windowSize, u32 sampleCount);

            void FreeManagers();
            void FreeBuffers();
            void FreeRenderTargets();

            void UpdateLight(cScene* scene);
            void UpdateShaders(cScene* scene);
        };

    }

}