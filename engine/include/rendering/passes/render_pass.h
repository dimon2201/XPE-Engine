#pragma once

#include <geometry/geometry_manager.h>

namespace xpe {

    namespace ecs
    {
        class Scene;
    }

    namespace render {

        using namespace core;
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
                SHADER = 3,
                RASTERIZER = 4,
                DEPTH_STENCIL = 5,
                BLENDING = 6,
                VERTEX_2D = 7,
                VERTEX_3D = 8,
                VERTEX_SKELETAL = 9,
                RENDER_TARGET = 10
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
                void* instance = nullptr,
                const eStage& stage = eStage::NONE,
                const u32 slot = 0
            ) : Tag(tag), Type(type), Stage(stage), Slot(slot), Instance(instance) {}

            string Tag;
            eType Type;
            eStage Stage;
            u32 Slot;
            void* Instance;
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

            virtual void Draw(Scene* scene) = 0;

            void Init();
            void Bind();
            void Unbind();

            inline RenderTarget* GetRenderTarget() { return m_Pipeline->RenderTarget; }

        protected:
            vector<RenderPassBinding> m_Bindings;
            RenderPassBinding* m_VertexBinding = nullptr;
            Pipeline* m_Pipeline = nullptr;

        };

    }

}