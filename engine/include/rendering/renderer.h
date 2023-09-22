#pragma once

namespace xpe {

    namespace ecs
    {
        class Scene;
    }

    namespace render {

        class RenderTarget;
        class Drawer;
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
            void AddDrawer(Args &&... args);

            template<typename T, typename ... Args>
            void AddRenderPass(Args &&... args);

            void RemoveDrawer(Drawer* drawer);

            void Render(ecs::Scene* scene);

        private:
            void FlushLights(ecs::Scene* scene);

            core::vector<Drawer*> m_Drawers;
            core::vector<RenderPass*> m_RenderPasses;
        };

        template<typename T, typename... Args>
        void Renderer::AddDrawer(Args &&... args)
        {
            m_Drawers.emplace_back(new T(std::forward<Args>(args)...));
        }

        template<typename T, typename... Args>
        void Renderer::AddRenderPass(Args &&... args)
        {
            m_RenderPasses.emplace_back(new T(std::forward<Args>(args)...));
        }

    }

}