#include <rendering/passes/skeleton_pass.h>
#include <rendering/material/material_manager.h>

#include <anim/skeleton_manager.h>

#include <ecs/components.hpp>

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
            scene->EachComponent<ModelComponent>([this](ModelComponent* component)
            {
                 if (!component->Transparent && component->Visible && component->HasSkeleton) {
                     auto& model = *component;
                     auto* skeleton = component->Entity->Get<SkeletonComponent>();

                     SkeletonManager::Flush(skeleton->Index);
                     SkeletonManager::Bind(skeleton->Index);

                     DrawInstanced(
                             model.PrimitiveTopology,
                             model.VertexOffset,
                             model.VertexCount,
                             model.IndexOffset,
                             model.Indices.size(),
                             component->Entity,
                             component->Entities,
                             [](Entity* entity, RenderInstance& instance) {
                                 auto* materialComponent = entity->Get<MaterialComponent>();
                                 if (materialComponent != nullptr) {
                                     instance.MaterialIndex = materialComponent->Index;
                                 }
                             }
                     );

                     SkeletonManager::Unbind(skeleton->Index);
                 }
            });
        }

        void SkeletonPass::DrawTransparent(Scene *scene)
        {
            scene->EachComponent<ModelComponent>([this](ModelComponent* component)
            {
                 if (component->Transparent && component->Visible && component->HasSkeleton) {
                     auto& model = *component;
                     auto* skeleton = component->Entity->Get<SkeletonComponent>();

                     SkeletonManager::Flush(skeleton->Index);
                     SkeletonManager::Bind(skeleton->Index);

                     DrawInstanced(
                             model.PrimitiveTopology,
                             model.VertexOffset,
                             model.VertexCount,
                             model.IndexOffset,
                             model.Indices.size(),
                             component->Entity,
                             component->Entities,
                             [](Entity* entity, RenderInstance& instance) {
                                 auto* materialComponent = entity->Get<MaterialComponent>();
                                 if (materialComponent != nullptr) {
                                     instance.MaterialIndex = materialComponent->Index;
                                 }
                             }
                     );

                     SkeletonManager::Unbind(skeleton->Index);
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
                glm::mat4x4 lightMatrix = LightMatrixUpdate(*lightComponent, lightView);

                scene->EachComponent<ModelComponent>([this, &lightMatrix](ModelComponent* component)
                {
                     if (component->CastShadow && component->Visible && component->HasSkeleton) {
                         auto& model = *component;
                         auto* skeleton = component->Entity->Get<SkeletonComponent>();

                         SkeletonManager::Flush(skeleton->Index);
                         SkeletonManager::Bind(skeleton->Index);

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

                         SkeletonManager::Unbind(skeleton->Index);
                     }
                });
            });

            scene->EachComponent<SpotLightComponent>([this, scene](SpotLightComponent* lightComponent) {
                ViewMatrix lightView;
                lightView.Position = lightComponent->Position;
                lightView.Front = glm::vec3(0, 0, 0);
                lightView.Up = glm::vec3(0, 1, 0);
                glm::mat4x4 lightMatrix = LightMatrixUpdate(*lightComponent, lightView);

                scene->EachComponent<ModelComponent>([this, &lightMatrix](ModelComponent* component)
                {
                     if (component->CastShadow && component->Visible && component->HasSkeleton) {
                         auto& model = *component;
                         auto* skeleton = component->Entity->Get<SkeletonComponent>();

                         SkeletonManager::Flush(skeleton->Index);
                         SkeletonManager::Bind(skeleton->Index);

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

                         SkeletonManager::Unbind(skeleton->Index);
                     }
                });
            });
        }

    }

}