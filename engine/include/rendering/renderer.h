#pragma once

namespace xpe {

    namespace ecs
    {
        class Scene;
    }

    namespace render {

        class RenderTarget;
        class RenderPass;
        class CameraBuffer;
        class DirectLightBuffer;
        class PointLightBuffer;
        class SpotLightBuffer;

        class ENGINE_API Renderer : public core::Object
        {

        public:
            render::CameraBuffer* CameraBuffer;
            render::DirectLightBuffer* DirectLightBuffer;
            render::PointLightBuffer* PointLightBuffer;
            render::SpotLightBuffer* SpotLightBuffer;

            Renderer();
            ~Renderer();

            template<typename T, typename ... Args>
            void AddRenderPass(Args &&... args);

            void Render(ecs::Scene* scene);

        private:
            void FlushLights(ecs::Scene* scene);

            core::vector<RenderPass*> m_RenderPasses;
        };

        template<typename T, typename... Args>
        void Renderer::AddRenderPass(Args &&... args)
        {
            T* renderPass = new T(std::forward<Args>(args)...);
            renderPass->Init();
            m_RenderPasses.emplace_back(renderPass);
        }

    }

}