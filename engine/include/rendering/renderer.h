#pragma once

namespace xpe {

    namespace ecs
    {
        class Scene;
    }

    namespace render {

        class RenderTarget;

        class Drawer;

        class MonitorBuffer;

        class CameraBuffer;

        class DirectLightBuffer;
        class PointLightBuffer;
        class SpotLightBuffer;

        class ENGINE_API Renderer : public core::Object
        {

        public:
            render::MonitorBuffer* MonitorBuffer;
            render::CameraBuffer* CameraBuffer;
            render::DirectLightBuffer* DirectLightBuffer;
            render::PointLightBuffer* PointLightBuffer;
            render::SpotLightBuffer* SpotLightBuffer;

            Renderer();
            ~Renderer();

            template<typename T, typename ... Args>
            void AddDrawer(Args &&... args);

            void RemoveDrawer(Drawer* drawer);

            void Render(ecs::Scene* scene);

        private:
            void FlushLights(ecs::Scene* scene);

            core::vector<Drawer*> m_Drawers;
        };

        template<typename T, typename... Args>
        void Renderer::AddDrawer(Args &&... args)
        {
            m_Drawers.emplace_back(new T(std::forward<Args>(args)...));
        }

    }

}