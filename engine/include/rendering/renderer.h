#pragma once

namespace xpe {

    namespace ecs
    {
        class Scene;
    }

    namespace render {

        class CameraBuffer;
        class Drawer;
        class RenderTarget;

        class ENGINE_API Renderer : public core::Object
        {

        public:
            Renderer();
            ~Renderer();

            inline CameraBuffer* GetCameraBuffer()
            {
                return m_CameraBuffer;
            }

            template<typename T, typename ... Args>
            void AddDrawer(Args &&... args);

            void RemoveDrawer(Drawer* drawer);

            void Render(ecs::Scene* scene, RenderTarget* renderTarget);

        private:
            CameraBuffer* m_CameraBuffer = nullptr;
            core::vector<Drawer*> m_Drawers;
        };

        template<typename T, typename... Args>
        void Renderer::AddDrawer(Args &&... args)
        {
            m_Drawers.emplace_back(new T(std::forward<Args>(args)...));
        }

    }

}