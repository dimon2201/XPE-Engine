#include <rendering/passes/skeletal_anim_pass.h>
#include <rendering/material/material_manager.h>

#include <ecs/scenes.hpp>

namespace xpe {

    namespace render {

        SkeletalAnimPass::SkeletalAnimPass(eType type, const vector<RenderPassBinding>& bindings) : InstancingPass(type, bindings)
        {
            MaterialManager::Bind(*m_Pipeline);
        }

        void SkeletalAnimPass::DrawOpaque(Scene *scene)
        {
            scene->EachComponent<SkinModelComponent>([this](SkinModelComponent* component)
            {
                 if (!component->Transparent && component->Visible && component->Model.Get() != nullptr) {
                     auto& model = *component->Model;
                     auto& skeleton = component->Skeleton;

                     if (skeleton.Get() != nullptr) {
                         skeleton->BoneBuffer.Flush();
                         context::BindVSBuffer(skeleton->BoneBuffer);
                     }

                     DrawInstanced(
                             model.PrimitiveTopology,
                             model.VertexOffset,
                             model.VertexCount,
                             model.IndexOffset,
                             model.Indices.size(),
                             component->Entity,
                             component->Entities
                     );

                     if (skeleton.Get() != nullptr) {
                         context::UnbindVSBuffer(skeleton->BoneBuffer);
                     }
                 }
            });
        }

        void SkeletalAnimPass::DrawTransparent(Scene *scene)
        {
            scene->EachComponent<SkinModelComponent>([this](SkinModelComponent* component)
            {
                 if (component->Transparent && component->Visible && component->Model.Get() != nullptr) {
                     auto& model = *component->Model;
                     auto& skeleton = component->Skeleton;

                     if (skeleton.Get() != nullptr) {
                         skeleton->BoneBuffer.Flush();
                         context::BindVSBuffer(skeleton->BoneBuffer);
                     }

                     DrawInstanced(
                             model.PrimitiveTopology,
                             model.VertexOffset,
                             model.VertexCount,
                             model.IndexOffset,
                             model.Indices.size(),
                             component->Entity,
                             component->Entities
                     );

                     if (skeleton.Get() != nullptr) {
                         context::UnbindVSBuffer(skeleton->BoneBuffer);
                     }
                 }
            });
        }

    }

}