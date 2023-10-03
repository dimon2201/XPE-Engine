#pragma once

#include "passes/render_pass.h"

namespace xpe {

    namespace ecs
    {
        class Scene;
    }

    namespace render {

        struct Viewport;
        class RenderTarget;
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

            Renderer(Viewport* viewport, core::Boolean useMSAA, core::usize msaaSampleCount);
            ~Renderer();

            template<typename T, typename ... Args>
            T* AddRenderPass(Args &&... args);
            void RemoveRenderPass(RenderPass* pass);
            void ClearRenderTargets();
            void Render(ecs::Scene* scene);

            RenderTarget* GetRenderTarget();
            void SetMSAA(core::Boolean value) { m_UseMSAA = value; }
            void SetMSAASampleCount(core::usize count) { m_MSAASampleCount = count; }

        private:
            void FlushLights(ecs::Scene* scene);

            core::Boolean m_UseMSAA;
            core::usize m_MSAASampleCount;
            RenderTarget* m_Main;
            core::vector<RenderPass*> m_RenderPasses;
        };

        template<typename T, typename... Args>
        T* Renderer::AddRenderPass(Args &&... args)
        {
            T* pass = new T(std::forward<Args>(args)...);
            m_RenderPasses.emplace_back(new T(std::forward<Args>(args)...));

            return pass;
        }

    }

}