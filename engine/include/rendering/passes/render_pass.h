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
                SHADER = 3
            };

            enum eStage
            {
                VERTEX = 0,
                PIXEL = 1
            };

            static constexpr u32 SLOT_DEFAULT = UINT32_MAX;

            RenderPassBinding(
                const string& tag,
                const eType& type,
                const eStage& stage,
                const u32 slot,
                GPUResource* resource
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

            virtual void Update(Scene* scene) = 0;
            virtual void Draw(Scene* scene) = 0;

            void Bind();
            void Unbind();

        protected:
            vector<RenderPassBinding> m_Bindings;
            Pipeline* m_Pipeline = nullptr;

        };

    }

}