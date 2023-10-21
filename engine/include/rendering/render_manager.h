#pragma once

#include <rendering/passes/render_pass.h>

namespace xpe {

    namespace ecs
    {
        class Scene;
    }

    namespace render {

        struct Viewport;
        struct BlendMode;
        struct RenderTarget;
        class RenderPass;
        struct CameraBuffer;
        struct DirectLightBuffer;
        struct PointLightBuffer;
        struct SpotLightBuffer;

        class ENGINE_API RenderManager : public core::Object
        {
           
        public:
            static void Init(Viewport* viewport, core::Boolean useMSAA, core::usize msaaSampleCount);
            static void Free();

            template<typename T, typename ... Args>
            static T* AddRenderPass(const RenderPass::eType& type, Args &&... args);

            static void Render(ecs::Scene* scene);

            static inline CameraBuffer* GetCameraBuffer() { return m_CameraBuffer; }
            static inline DirectLightBuffer* GetDirectLightBuffer() { return m_DirectLightBuffer; }
            static inline PointLightBuffer* GetPointLightBuffer() { return m_PointLightBuffer; }
            static inline SpotLightBuffer* GetSpotLightBuffer() { return m_SpotLightBuffer; }
            static inline RenderTarget* GetOpaqueRenderTarget() { return m_OpaqueRenderTarget; }
            static inline RenderTarget* GetTransparentRenderTarget() { return m_TransparentRenderTarget; }

        private:
            static void FlushLights(ecs::Scene* scene);

            static CameraBuffer* m_CameraBuffer;
            static DirectLightBuffer* m_DirectLightBuffer;
            static PointLightBuffer* m_PointLightBuffer;
            static SpotLightBuffer* m_SpotLightBuffer;
            static vector<RenderPass*>* m_OpaqueRenderPasses;
            static vector<RenderPass*>* m_TransparentRenderPasses;
            static vector<RenderPass*>* m_GUIRenderPasses;
            static Texture* m_SharedDepthTexture;
            static RenderTarget* m_OpaqueRenderTarget;
            static RenderTarget* m_TransparentRenderTarget;
        };

        template<typename T, typename... Args>
        T* RenderManager::AddRenderPass(const RenderPass::eType& type, Args &&... args)
        {
            T* renderPass = new T(std::forward<Args>(args)...);

            switch (type)
            {

            case RenderPass::eType::OPAQUE:
                m_OpaqueRenderPasses->emplace_back(renderPass);
                break;

            case RenderPass::eType::TRANSPARENT:
                m_TransparentRenderPasses->push_back(renderPass);
                break;

            case RenderPass::eType::GUI:
                m_GUIRenderPasses->emplace_back(renderPass);
                break;

            default:
                m_OpaqueRenderPasses->emplace_back(renderPass);
                break;

            }

            return renderPass;
        }

    }

}