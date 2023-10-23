#pragma once

#include <rendering/buffers/shadow_filter_buffer.h>

namespace xpe {

    namespace render {

        using namespace core;
        using namespace ecs;

        class Canvas;
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
            RenderSystem();
            ~RenderSystem();

            template<typename T, typename ... Args>
            T* AddRenderPass(Args &&... args);

            template<typename T>
            T* GetRenderPass();

            void Update(Scene* scene, const Time& dt) override final;

            void InitRenderTargets(render::Canvas* canvas);

            void Prepare();

            inline Canvas* GetCanvas() { return m_Canvas; }

            inline CameraBuffer* GetCameraBuffer() { return m_CameraBuffer; }

            inline DirectLightBuffer* GetDirectLightBuffer() { return m_DirectLightBuffer; }
            inline PointLightBuffer* GetPointLightBuffer() { return m_PointLightBuffer; }
            inline SpotLightBuffer* GetSpotLightBuffer() { return m_SpotLightBuffer; }

            inline TextureSampler* GetShadowSampler() { return m_ShadowSampler; }
            inline Texture* GetShadowMap() { return m_ShadowMap; }
            inline Texture* GetShadowCoords() { return m_ShadowCoords; }
            inline ShadowFilterBuffer* GetShadowFilterBuffer() { return m_ShadowFilterBuffer; }

            inline RenderTarget* GetMainRT() { return m_MainRT; }
            inline RenderTarget* GetSsaoRT() { return m_SsaoRT; }

        private:
            void UpdateLight(Scene* scene);
            void UpdatePasses(Scene* scene);

            vector<RenderPass*> m_RenderPasses;

            render::Canvas* m_Canvas;

            render::CameraBuffer* m_CameraBuffer;

            render::DirectLightBuffer* m_DirectLightBuffer;
            render::PointLightBuffer* m_PointLightBuffer;
            render::SpotLightBuffer* m_SpotLightBuffer;

            render::TextureSampler* m_ShadowSampler;
            render::Texture* m_ShadowMap;
            render::Texture* m_ShadowCoords;
            render::ShadowFilterBuffer* m_ShadowFilterBuffer;

            render::RenderTarget* m_MainRT;
            render::RenderTarget* m_SsaoRT;

        };

        template<typename T, typename... Args>
        T* RenderSystem::AddRenderPass(Args &&... args)
        {
            T* renderPass = new T(std::forward<Args>(args)...);
            renderPass->Init();
            m_RenderPasses.emplace_back(renderPass);
            return renderPass;
        }

        template<typename T>
        T* RenderSystem::GetRenderPass()
        {
            T* resultPass = nullptr;

            for (auto* renderPass : m_RenderPasses)
            {
                if (strcmp(typeid(T).name(), typeid(renderPass).name()) == 0) {
                    return renderPass;
                }
            }

            return resultPass;
        }

    }

}