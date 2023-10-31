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

        void SkeletonPass::DrawOpaque(cScene *scene)
        {
            scene->EachComponent<sCSkeletonModel>([this](sCSkeletonModel* component)
            {
                 if (component->Visible && !component->Transparent) {
                     auto& model = *component;
                     auto& skeleton = component->Skeleton;

                     mSkeletonManager::Flush(skeleton.Index);
                     mSkeletonManager::Bind(skeleton.Index);

                     DrawInstanced(
                             model.PrimitiveTopology,
                             model.VertexOffset,
                             model.VertexCount,
                             model.IndexOffset,
                             model.Indices.size(),
                             component->Entity,
                             component->Entities,
                             [](cEntity* entity, RenderInstance& instance) {
                                 auto* materialComponent = entity->Get<sCMaterial>();
                                 if (materialComponent != nullptr) {
                                     instance.MaterialIndex = materialComponent->Index;
                                 }
                             }
                     );

                     mSkeletonManager::Unbind(skeleton.Index);
                 }
            });
        }

        void SkeletonPass::DrawTransparent(cScene *scene)
        {
            scene->EachComponent<sCSkeletonModel>([this](sCSkeletonModel* component)
            {
                 if (component->Visible && component->Transparent) {
                     auto& model = *component;
                     auto& skeleton = component->Skeleton;

                     mSkeletonManager::Flush(skeleton.Index);
                     mSkeletonManager::Bind(skeleton.Index);

                     DrawInstanced(
                             model.PrimitiveTopology,
                             model.VertexOffset,
                             model.VertexCount,
                             model.IndexOffset,
                             model.Indices.size(),
                             component->Entity,
                             component->Entities,
                             [](cEntity* entity, RenderInstance& instance) {
                                 auto* materialComponent = entity->Get<sCMaterial>();
                                 if (materialComponent != nullptr) {
                                     instance.MaterialIndex = materialComponent->Index;
                                 }
                             }
                     );

                     mSkeletonManager::Unbind(skeleton.Index);
                 }
            });
        }

        void SkeletonPass::DrawShadow(cScene* scene)
        {
            scene->EachComponent<sCDirectionalLight>([this, scene](sCDirectionalLight* lightComponent) {
                ViewMatrix lightView;
                lightView.Position = lightComponent->Position;
                lightView.Front = glm::vec3(0, 0, 0);
                lightView.Up = glm::vec3(0, 1, 0);
                glm::mat4x4 lightMatrix = LightMatrixUpdate(*lightComponent, lightView);

                scene->EachComponent<sCSkeletonModel>([this, &lightMatrix](sCSkeletonModel* component)
                {
                     if (component->Visible && component->CastShadow) {
                         auto& model = *component;
                         auto& skeleton = component->Skeleton;

                         mSkeletonManager::Flush(skeleton.Index);
                         mSkeletonManager::Bind(skeleton.Index);

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

                         mSkeletonManager::Unbind(skeleton.Index);
                     }
                });
            });

            scene->EachComponent<sCSpotLight>([this, scene](sCSpotLight* lightComponent) {
                ViewMatrix lightView;
                lightView.Position = lightComponent->Position;
                lightView.Front = glm::vec3(0, 0, 0);
                lightView.Up = glm::vec3(0, 1, 0);
                glm::mat4x4 lightMatrix = LightMatrixUpdate(*lightComponent, lightView);

                scene->EachComponent<sCSkeletonModel>([this, &lightMatrix](sCSkeletonModel* component)
                {
                     if (component->Visible && component->CastShadow) {
                         auto& model = *component;
                         auto& skeleton = component->Skeleton;

                         mSkeletonManager::Flush(skeleton.Index);
                         mSkeletonManager::Bind(skeleton.Index);

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

                         mSkeletonManager::Unbind(skeleton.Index);
                     }
                });
            });
        }

    }

}