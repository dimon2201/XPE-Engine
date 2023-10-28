#pragma once

#include <rendering/core/texture.h>

#include <rendering/passes/render_pass.h>

#include <rendering/buffers/viewport_buffer.h>
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
            RenderSystem(const Viewport& viewport, u32 sampleCount);
            ~RenderSystem();

            template<typename T, typename ... Args>
            T* AddRenderPass(Args &&... args);

            void Update(Scene* scene, const Time& dt) override final;

            void Prepare();

            inline ViewportBuffer* GetViewportBuffer() { return m_ViewportBuffer; }
            inline MonitorBuffer* GetMonitorBuffer() { return m_MonitorBuffer; }
            inline CameraBuffer* GetCameraBuffer() { return m_CameraBuffer; }
            inline DirectLightBuffer* GetDirectLightBuffer() { return m_DirectLightBuffer; }
            inline PointLightBuffer* GetPointLightBuffer() { return m_PointLightBuffer; }
            inline SpotLightBuffer* GetSpotLightBuffer() { return m_SpotLightBuffer; }
            inline ShadowFilterBuffer* GetShadowFilterBuffer() { return m_ShadowFilterBuffer; }

            inline RenderTarget* GetFinalRT() { return m_FinalRenderTarget; }
            inline RenderTarget* GetSceneRT() { return m_SceneRenderTarget; }
            inline RenderTarget* GetShadowRT() { return m_ShadowRenderTarget; }
            inline RenderTarget* GetOpaqueRT() { return m_OpaqueRenderTarget; }
            inline RenderTarget* GetTransparentRT() { return m_TransparentRenderTarget; }
            inline RenderTarget* GetUiRT() { return m_UiRenderTarget; }

            inline Texture* GetSharedDepthTexture() { return m_SharedDepthTexture; }
            inline Texture* GetShadowMap() { return m_ShadowRenderTarget->DepthStencil; }

            inline TextureSampler* GetShadowSampler() { return m_ShadowSampler; }

        private:
            void InitBuffers(const Viewport& viewport, u32 sampleCount);
            void InitSamplers(const Viewport& viewport, u32 sampleCount);
            void InitRenderTargets(const Viewport& viewport, u32 sampleCount);

            void UpdateLight(Scene* scene);
            void UpdatePasses(Scene* scene);

            vector<RenderPass*> m_FinalRenderPasses;
            vector<RenderPass*> m_ShadowRenderPasses;
            vector<RenderPass*> m_OpaqueRenderPasses;
            vector<RenderPass*> m_TransparentRenderPasses;
            vector<RenderPass*> m_PostFXRenderPasses;
            vector<RenderPass*> m_UiRenderPasses;

            ViewportBuffer* m_ViewportBuffer;
            MonitorBuffer* m_MonitorBuffer;
            CameraBuffer* m_CameraBuffer;
            DirectLightBuffer* m_DirectLightBuffer;
            PointLightBuffer* m_PointLightBuffer;
            SpotLightBuffer* m_SpotLightBuffer;
            ShadowFilterBuffer* m_ShadowFilterBuffer;

            RenderTarget* m_SceneRenderTarget;
            RenderTarget* m_FinalRenderTarget;
            RenderTarget* m_ShadowRenderTarget;
            RenderTarget* m_OpaqueRenderTarget;
            RenderTarget* m_TransparentRenderTarget;
            RenderTarget* m_UiRenderTarget;

            Texture* m_SharedDepthTexture;

            TextureSampler* m_ShadowSampler;

        };

        template<typename T, typename... Args>
        T* RenderSystem::AddRenderPass(Args &&... args)
        {
            T* renderPass = new T(std::forward<Args>(args)...);
            renderPass->Init();

            switch (renderPass->GetType())
            {
                case RenderPass::eType::FINAL:
                    m_FinalRenderPasses.emplace_back(renderPass);
                    break;

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

                case RenderPass::eType::UI:
                    m_UiRenderPasses.emplace_back(renderPass);
                    break;
            }

            return renderPass;
        }

    }

}