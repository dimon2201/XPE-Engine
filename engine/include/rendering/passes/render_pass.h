#pragma once

#include <rendering/core/pipeline.h>

namespace xpe {

    namespace render {

        using namespace core;
        using namespace ecs;

        struct ENGINE_API sRenderPassBinding
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

            sRenderPassBinding(
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

        class ENGINE_API cRenderPass : public cObject
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

            cRenderPass(eType type, const vector<sRenderPassBinding>& bindings);
            virtual ~cRenderPass();

            bool Enable = true;

            virtual void Update(cScene* scene) {}
            virtual void DrawFinal(cScene* scene);
            virtual void DrawShadow(cScene* scene) {}
            virtual void DrawOpaque(cScene* scene) {}
            virtual void DrawTransparent(cScene* scene) {}
            virtual void DrawPostFX(cScene* scene);
            virtual void DrawUI(cScene* scene) {}

            void Init();
            void Bind();
            void Unbind();

            sRenderTarget* GetRenderTarget();
            inline eType GetType() { return m_Type; }

        protected:

            virtual void InitFinal();
            virtual void InitShadow();
            virtual void InitOpaque();
            virtual void InitTransparent();
            virtual void InitPostFX();
            virtual void InitUI();

            eType m_Type;
            vector<sRenderPassBinding> m_Bindings;
            sPipeline* m_Pipeline = nullptr;

        };

    }

}