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
                FINAL = 0,
                SHADOW = 1,
                OPAQUE = 2,
                TRANSPARENT = 3,
                POSTFX = 4,
                UI = 5,
            };

            RenderPass(eType type, const vector<RenderPassBinding>& bindings);
            virtual ~RenderPass();

            bool Enable = true;

            virtual void Update(Scene* scene) {}
            virtual void DrawFinal(Scene* scene);
            virtual void DrawShadow(Scene* scene) {}
            virtual void DrawOpaque(Scene* scene) {}
            virtual void DrawTransparent(Scene* scene) {}
            virtual void DrawPostFX(Scene* scene);
            virtual void DrawUI(Scene* scene) {}

            void Init();
            void Bind();
            void Unbind();

            RenderTarget* GetRenderTarget();
            inline eType GetType() { return m_Type; }

        protected:

            virtual void InitFinal();
            virtual void InitShadow();
            virtual void InitOpaque();
            virtual void InitTransparent();
            virtual void InitPostFX();
            virtual void InitUI();

            eType m_Type;
            vector<RenderPassBinding> m_Bindings;
            Pipeline* m_Pipeline = nullptr;

        };

    }

}