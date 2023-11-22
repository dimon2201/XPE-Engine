#pragma once

#include <rendering/core/texture.h>

#include <rendering/passes/render_pass.h>

#include <rendering/buffers/camera_buffer.h>
#include <rendering/buffers/light_buffers.h>

namespace xpe {

    namespace render {

        using namespace core;
        using namespace ecs;

        class ENGINE_API cRenderSystem : public cSystem
        {

        public:
            cRenderSystem(sViewport& viewport, u32 sampleCount);
            ~cRenderSystem();

            template<typename T, typename ... Args>
            T* AddRenderPass(Args &&... args);

            void Update(cScene* scene, const cTime& dt) override final;

            void Prepare();

            void WindowFrameResized(int width, int height);

            inline sDirectLightBuffer* GetDirectLightBuffer() { return m_DirectLightBuffer; }
            inline sDirectLightMatrixBuffer* GetDirectLightMatrixBuffer() { return m_DirectLightMatrixBuffer; }
            inline sPointLightBuffer* GetPointLightBuffer() { return m_PointLightBuffer; }
            inline sSpotLightBuffer* GetSpotLightBuffer() { return m_SpotLightBuffer; }

            inline sRenderTarget* GetFinalRT() { return m_FinalRenderTarget; }
            inline sRenderTarget* GetSceneRT() { return m_SceneRenderTarget; }
            inline sRenderTarget* GetShadowRT() { return m_ShadowRenderTarget; }
            inline sRenderTarget* GetOpaqueRT() { return m_OpaqueRenderTarget; }
            inline sRenderTarget* GetTransparentRT() { return m_TransparentRenderTarget; }
            inline sRenderTarget* GetUiRT() { return m_UiRenderTarget; }

            inline sTexture* GetSharedDepthTexture() { return m_SharedDepthTexture; }

        private:
            void InitManagers(sViewport& viewport, u32 sampleCount);
            void InitBuffers(sViewport& viewport, u32 sampleCount);
            void InitSamplers(sViewport& viewport, u32 sampleCount);
            void InitRenderTargets(sViewport& viewport, u32 sampleCount);

            void FreeManagers();
            void FreeBuffers();
            void FreeSamplers();
            void FreeRenderTargets();

            void UpdateLight(cScene* scene);
            void UpdatePasses(cScene* scene);

            vector<cRenderPass*> m_FinalRenderPasses;
            vector<cRenderPass*> m_ShadowRenderPasses;
            vector<cRenderPass*> m_OpaqueRenderPasses;
            vector<cRenderPass*> m_TransparentRenderPasses;
            vector<cRenderPass*> m_PostFXRenderPasses;
            vector<cRenderPass*> m_UiRenderPasses;

            sDirectLightBuffer* m_DirectLightBuffer;
            sDirectLightMatrixBuffer* m_DirectLightMatrixBuffer;
            sPointLightBuffer* m_PointLightBuffer;
            sSpotLightBuffer* m_SpotLightBuffer;

            sRenderTarget* m_SceneRenderTarget;
            sRenderTarget* m_FinalRenderTarget;
            sRenderTarget* m_ShadowRenderTarget;
            sRenderTarget* m_OpaqueRenderTarget;
            sRenderTarget* m_TransparentRenderTarget;
            sRenderTarget* m_UiRenderTarget;

            sTexture* m_SharedDepthTexture;
        };

        template<typename T, typename... Args>
        T* cRenderSystem::AddRenderPass(Args &&... args)
        {
            T* renderPass = new T(std::forward<Args>(args)...);
            renderPass->Init();

            switch (renderPass->GetType())
            {
                case cRenderPass::eType::FINAL:
                    m_FinalRenderPasses.emplace_back(renderPass);
                    break;

                case cRenderPass::eType::SHADOW:
                    m_ShadowRenderPasses.emplace_back(renderPass);
                    break;

                case cRenderPass::eType::OPAQUE:
                    m_OpaqueRenderPasses.emplace_back(renderPass);
                    break;

                case cRenderPass::eType::TRANSPARENT:
                    m_TransparentRenderPasses.emplace_back(renderPass);
                    break;

                case cRenderPass::eType::POSTFX:
                    m_PostFXRenderPasses.emplace_back(renderPass);
                    break;

                case cRenderPass::eType::UI:
                    m_UiRenderPasses.emplace_back(renderPass);
                    break;
            }

            return renderPass;
        }

    }

}