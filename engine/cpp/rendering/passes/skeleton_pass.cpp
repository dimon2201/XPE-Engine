#include <rendering/passes/skeleton_pass.h>
#include <rendering/material/material_manager.h>

#include <ecs/scenes.hpp>

namespace xpe {

    namespace render {

        SkeletonPass::SkeletonPass(eType type, const vector<RenderPassBinding>& bindings) : InstancingPass(type, bindings)
        {
        }

        void SkeletonPass::InitOpaque()
        {
            RenderPass::InitOpaque();
            MaterialManager::Bind(*m_Pipeline);
        }

        void SkeletonPass::InitTransparent()
        {
            RenderPass::InitTransparent();
            MaterialManager::Bind(*m_Pipeline);
        }

        void SkeletonPass::DrawOpaque(Scene *scene)
        {
            scene->EachComponent<SkeletonModelComponent>([this](SkeletonModelComponent* component)
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
                             component->Entities,
                             [](Entity* entity, RenderInstance& instance) {
                                 auto* materialComponent = entity->GetComponent<MaterialComponent>(entity->GetTag());
                                 if (materialComponent != nullptr) {
                                     instance.MaterialIndex = materialComponent->Material->Index;
                                 }
                             }
                     );

                     if (skeleton.Get() != nullptr) {
                         context::UnbindVSBuffer(skeleton->BoneBuffer);
                     }
                 }
            });
        }

        void SkeletonPass::DrawTransparent(Scene *scene)
        {
            scene->EachComponent<SkeletonModelComponent>([this](SkeletonModelComponent* component)
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
                             component->Entities,
                             [](Entity* entity, RenderInstance& instance) {
                                 auto* materialComponent = entity->GetComponent<MaterialComponent>(entity->GetTag());
                                 if (materialComponent != nullptr) {
                                     instance.MaterialIndex = materialComponent->Material->Index;
                                 }
                             }
                     );

                     if (skeleton.Get() != nullptr) {
                         context::UnbindVSBuffer(skeleton->BoneBuffer);
                     }
                 }
            });
        }

        void SkeletonPass::DrawShadow(Scene* scene)
        {
            scene->EachComponent<DirectLightComponent>([this, scene](DirectLightComponent* lightComponent) {
                ViewMatrix lightView;
                lightView.Position = lightComponent->Position;
                lightView.Front = glm::vec3(0, 0, 0);
                lightView.Up = glm::vec3(0, 1, 0);
                glm::mat4x4 lightMatrix = LightMatrixUpdate(lightComponent->Projection, lightView);

                scene->EachComponent<SkeletonModelComponent>([this, &lightMatrix](SkeletonModelComponent* component)
                {
                     if (component->CastShadow && component->Visible && component->Model.Get() != nullptr) {
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
                                 component->Entities,
                                 {},
                                 lightMatrix
                         );

                         if (skeleton.Get() != nullptr) {
                             context::UnbindVSBuffer(skeleton->BoneBuffer);
                         }
                     }
                });
            });

            scene->EachComponent<SpotLightComponent>([this, scene](SpotLightComponent* lightComponent) {
                ViewMatrix lightView;
                lightView.Position = lightComponent->Position;
                lightView.Front = glm::vec3(0, 0, 0);
                lightView.Up = glm::vec3(0, 1, 0);
                glm::mat4x4 lightMatrix = LightMatrixUpdate(lightComponent->Projection, lightView);

                scene->EachComponent<SkeletonModelComponent>([this, &lightMatrix](SkeletonModelComponent* component)
                {
                     if (component->CastShadow && component->Visible && component->Model.Get() != nullptr) {
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
                                 component->Entities,
                                 {},
                                 lightMatrix
                         );

                         if (skeleton.Get() != nullptr) {
                             context::UnbindVSBuffer(skeleton->BoneBuffer);
                         }
                     }
                });
            });
        }

    }

}