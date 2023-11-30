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
                SHADER = 0,
                RENDER_TARGET = 1,
                BUFFER = 2,
                TEXTURE = 3,
                SAMPLER = 4,
                RASTERIZER = 5,
                DEPTH_STENCIL = 6,
                BLENDING = 7
            };

            sRenderPassBinding(
                const string& tag,
                const eType& type,
                void* resource = nullptr,
                const sShaderStage::eType& stage = sShaderStage::eType::NONE,
                const u32 slot = 0
            ) : Tag(tag), Type(type), Resource(resource), Stage(stage), Slot(slot) {}

            string Tag;
            eType Type;
            sShaderStage::eType Stage;
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
            sVertexPipeline* m_Pipeline = nullptr;

        };

        struct ENGINE_API sComputePassBinding
        {
            enum eType
            {
                SHADER = 0,
                RENDER_TARGET = 1,
                BUFFER = 2,
                TEXTURE = 3,
                SAMPLER = 4
            };

            sComputePassBinding(
                const string& tag,
                const eType& type,
                void* resource = nullptr,
                const u32 slot = 0
            ) : Tag(tag), Type(type), Resource(resource), Slot(slot) {}

            string Tag;
            eType Type;
            u32 Slot;
            void* Resource;
        };

        class ENGINE_API cComputePass : public cObject
        {

        public:
            cComputePass(const vector<sComputePassBinding>& bindings, const glm::ivec3& threadGroups = { 1, 1, 1 });
            virtual ~cComputePass();

            bool Enable = true;

            virtual void Update(cScene* scene) {}

            virtual void Init();
            void Bind();
            void Unbind();
            void Dispatch();

            sRenderTarget* GetRenderTarget();

        protected:
            vector<sComputePassBinding> m_Bindings;
            sComputePipeline* m_Pipeline = nullptr;
            glm::ivec3 m_ThreadGroups;
        };

    }

}