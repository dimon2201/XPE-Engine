#pragma once

#include <rendering/core/pipeline.h>

namespace xpe {

    namespace render {

        using namespace core;
        using namespace ecs;

        struct ENGINE_API RenderPassBinding
        {
            enum eType
            {
                BUFFER = 0,
                TEXTURE = 1,
                SAMPLER = 2,
                SHADER = 3,
                RASTERIZER = 4,
                DEPTH_STENCIL = 5,
                BLENDING = 6,
                RENDER_TARGET = 7,
            };

            enum eStage
            {
                NONE = -1,
                VERTEX = 0,
                PIXEL = 1
            };

            static constexpr u32 SLOT_DEFAULT = UINT32_MAX;

            RenderPassBinding(
                const string& tag,
                const eType& type,
                void* resource = nullptr,
                const eStage& stage = eStage::NONE,
                const u32 slot = 0
            ) : Tag(tag), Type(type), Stage(stage), Slot(slot), Resource(resource) {}

            string Tag;
            eType Type;
            eStage Stage;
            u32 Slot;
            void* Resource;
        };

        class ENGINE_API RenderPass : public Object
        {

        public:
            enum eType
            {
                OPAQUE = 0,
                TRANSPARENT = 1,
                GUI = 2
            };

            RenderPass(const vector<RenderPassBinding>& bindings);
            virtual ~RenderPass();

            virtual void Update(Scene* scene) {}
            virtual void Draw(Scene* scene) = 0;

            void Init();
            void Bind();
            void Unbind();

            RenderTarget* GetRenderTarget();

        protected:
            vector<RenderPassBinding> m_Bindings;
            Pipeline* m_Pipeline = nullptr;

        };

    }

}