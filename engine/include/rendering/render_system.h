#pragma once

#include <rendering/buffers/shadow_filter_buffer.h>

namespace xpe {

    namespace render {

        using namespace core;
        using namespace ecs;

        class RenderTarget;
        class RenderPass;
        class CameraBuffer;
        class DirectLightBuffer;
        class PointLightBuffer;
        class SpotLightBuffer;
        class Texture;
        class TextureSampler;

        class ENGINE_API RenderSystem : public System
        {

        public:
            render::CameraBuffer* CameraBuffer;

            render::DirectLightBuffer* DirectLightBuffer;
            render::PointLightBuffer* PointLightBuffer;
            render::SpotLightBuffer* SpotLightBuffer;

            render::TextureSampler* ShadowSampler;
            render::Texture* ShadowMap;
            render::Texture* ShadowCoords;
            render::ShadowFilterBuffer* ShadowFilterBuffer;

            RenderSystem();
            ~RenderSystem();

            template<typename T, typename ... Args>
            void AddRenderPass(Args &&... args);

            void Update(Scene* scene, const Time& dt) override final;

            void Prepare();

        private:
            void UpdateLight(Scene* scene);
            void UpdatePasses(Scene* scene);

            vector<RenderPass*> m_RenderPasses;
        };

        template<typename T, typename... Args>
        void RenderSystem::AddRenderPass(Args &&... args)
        {
            T* renderPass = new T(std::forward<Args>(args)...);
            renderPass->Init();
            m_RenderPasses.emplace_back(renderPass);
        }

    }

}