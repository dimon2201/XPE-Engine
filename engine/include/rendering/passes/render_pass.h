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
                GPUResource* resource = nullptr,
                const eStage& stage = eStage::NONE,
                const u32 slot = 0
            ) : Tag(tag), Type(type), Stage(stage), Slot(slot), Resource(resource) {}

            string Tag;
            eType Type;
            eStage Stage;
            u32 Slot;
            GPUResource* Resource;
        };

        class ENGINE_API RenderPass : public Object
        {

        public:
            RenderPass(const vector<RenderPassBinding>& bindings, RenderTarget* output);
            virtual ~RenderPass();

            virtual void Draw(Scene* scene) = 0;

            void Init();
            void Bind();
            void Unbind();

        protected:
            vector<RenderPassBinding> m_Bindings;
            RenderPassBinding* m_VertexBinding = nullptr;
            Pipeline* m_Pipeline = nullptr;

        };

    }

}