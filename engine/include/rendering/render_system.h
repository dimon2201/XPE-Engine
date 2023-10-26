#pragma once

#include <rendering/core/texture.h>
#include <rendering/passes/render_pass.h>
#include <rendering/buffers/monitor_buffer.h>
#include <rendering/buffers/camera_buffer.h>
#include <rendering/buffers/light_buffers.h>
#include <rendering/buffers/shadow_filter_buffer.h>

namespace xpe {

    namespace render {

        using namespace core;
        using namespace ecs;

        class ENGINE_API RenderSystem : public System
        {

        public:
            RenderSystem();
            ~RenderSystem();

            template<typename T, typename ... Args>
            T* AddRenderPass(Args &&... args);

            template<typename T>
            T* GetRenderPass(RenderPass::eType type);

            void Update(Scene* scene, const Time& dt) override final;

            void InitRenderTargets(Viewport* viewport, u32 sampleCount);

            void Prepare();

            inline MonitorBuffer* GetMonitorBuffer() { return m_MonitorBuffer; }

            inline CameraBuffer* GetCameraBuffer() { return m_CameraBuffer; }

            inline DirectLightBuffer* GetDirectLightBuffer() { return m_DirectLightBuffer; }
            inline PointLightBuffer* GetPointLightBuffer() { return m_PointLightBuffer; }
            inline SpotLightBuffer* GetSpotLightBuffer() { return m_SpotLightBuffer; }

            inline TextureSampler* GetShadowSampler() { return m_ShadowSampler; }
            inline ShadowFilterBuffer* GetShadowFilterBuffer() { return m_ShadowFilterBuffer; }

            inline Texture* GetSharedDepthTexture() { return m_SharedDepthTexture; }
            inline RenderTarget* GetOpaqueRT() { return m_OpaqueRenderTarget; }
            inline RenderTarget* GetTransparentRT() { return m_TransparentRenderTarget; }
            inline RenderTarget* GetShadowRT() { return m_ShadowRenderTarget; }

            inline Texture* GetShadowMap() { return m_ShadowRenderTarget->DepthStencil; }

        private:
            void UpdateLight(Scene* scene);
            void UpdatePasses(Scene* scene);

            vector<RenderPass*> m_ShadowRenderPasses;
            vector<RenderPass*> m_OpaqueRenderPasses;
            vector<RenderPass*> m_TransparentRenderPasses;
            vector<RenderPass*> m_PostFXRenderPasses;

            render::MonitorBuffer* m_MonitorBuffer;
            render::CameraBuffer* m_CameraBuffer;
            render::DirectLightBuffer* m_DirectLightBuffer;
            render::PointLightBuffer* m_PointLightBuffer;
            render::SpotLightBuffer* m_SpotLightBuffer;
            render::ShadowFilterBuffer* m_ShadowFilterBuffer;

            render::TextureSampler* m_ShadowSampler;

            Texture* m_SharedDepthTexture;
            RenderTarget* m_OpaqueRenderTarget;
            RenderTarget* m_TransparentRenderTarget;
            RenderTarget* m_ShadowRenderTarget;

        };

        template<typename T, typename... Args>
        T* RenderSystem::AddRenderPass(Args &&... args)
        {
            T* renderPass = new T(std::forward<Args>(args)...);
            renderPass->Init();

            switch (renderPass->GetType())
            {
                case RenderPass::eType::SHADOW:
                    m_ShadowRenderPasses.emplace_back(renderPass);
                    break;

                case RenderPass::eType::OPAQUE:
                    m_OpaqueRenderPasses.emplace_back(renderPass);
                    break;

                case RenderPass::eType::TRANSPARENT:
                    m_TransparentRenderPasses.emplace_back(renderPass);
                    break;

                case RenderPass::eType::POSTFX:
                    m_PostFXRenderPasses.emplace_back(renderPass);
                    break;

            }

            return renderPass;
        }

        template<typename T>
        T* RenderSystem::GetRenderPass(RenderPass::eType type)
        {
            T* resultPass = nullptr;

            switch (type)
            {
                case RenderPass::eType::SHADOW:
                    for (auto* renderPass : m_ShadowRenderPasses)
                    {
                        if (strcmp(typeid(T).name(), typeid(renderPass).name()) == 0) {
                            return renderPass;
                        }
                    }
                    break;

                case RenderPass::eType::OPAQUE:
                    for (auto* renderPass : m_OpaqueRenderPasses)
                    {
                        if (strcmp(typeid(T).name(), typeid(renderPass).name()) == 0) {
                            return renderPass;
                        }
                    }
                    break;

                case RenderPass::eType::TRANSPARENT:
                    for (auto* renderPass : m_TransparentRenderPasses)
                    {
                        if (strcmp(typeid(T).name(), typeid(renderPass).name()) == 0) {
                            return renderPass;
                        }
                    }
                    break;

                case RenderPass::eType::POSTFX:
                    for (auto* renderPass : m_PostFXRenderPasses)
                    {
                        if (strcmp(typeid(T).name(), typeid(renderPass).name()) == 0) {
                            return renderPass;
                        }
                    }
                    break;

            }

            return resultPass;
        }

    }

}