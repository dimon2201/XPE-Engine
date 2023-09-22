#pragma once

#include "stl/vector.h"

namespace xpe {

    namespace ecs
    {
        class Scene;
    }

    namespace render {

        using namespace ecs;

        struct CameraBuffer;
        struct Shader;
        struct RenderTarget;
        struct Pipeline;
        struct GPUResource;

        struct ENGINE_API RenderPassBinding
        {
            enum eType
            {
                BUFFER = 0,
                TEXTURE = 1,
                SAMPLER = 2,
                SHADER = 3
            };

            enum eStage
            {
                VERTEX = 0,
                PIXEL = 1
            };

            static constexpr core::u32 SLOT_DEFAULT = UINT32_MAX;

            RenderPassBinding(const core::string& tag, const eType& type, const eStage& stage, const core::u32 slot, GPUResource* resource)
                : Tag(tag), Type(type), Stage(stage), Slot(slot), Resource(resource)
            {
            }

            core::string Tag;
            eType Type;
            eStage Stage;
            core::u32 Slot;
            GPUResource* Resource;
        };

        class ENGINE_API RenderPass : public core::Object
        {

        public:
            RenderPass(const core::vector<RenderPassBinding>& bindings, RenderTarget* output);
            virtual ~RenderPass();

            virtual void Update(Scene* scene) = 0;
            virtual void Draw(Scene* scene) = 0;

            void Bind();
            void Unbind();

        protected:
            core::vector<RenderPassBinding> m_Bindings;
            Pipeline* m_Pipeline = nullptr;

        };

    }

}