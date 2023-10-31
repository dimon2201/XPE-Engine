#include <rendering/passes/skeleton_pass.h>
#include <rendering/material/material_manager.h>

#include <anim/skeleton_manager.h>

#include <ecs/components.hpp>

namespace xpe {

    namespace render {

        cSkeletonPass::cSkeletonPass(eType type, const vector<sRenderPassBinding>& bindings) : cInstancingPass(type, bindings)
        {
        }

        void cSkeletonPass::InitOpaque()
        {
            cRenderPass::InitOpaque();
            cMaterialManager::Bind(*m_Pipeline);
        }

        void cSkeletonPass::InitTransparent()
        {
            cRenderPass::InitTransparent();
            cMaterialManager::Bind(*m_Pipeline);
        }

        void cSkeletonPass::DrawOpaque(cScene *scene)
        {
            scene->EachComponent<sCSkeletonModel>([this](sCSkeletonModel* component)
            {
                 if (component->Visible && !component->Transparent) {
                     auto& model = *component;
                     auto& skeleton = component->Skeleton;

                     cSkeletonManager::Flush(skeleton.Index);
                     cSkeletonManager::Bind(skeleton.Index);

                     DrawInstanced(
                             model.PrimitiveTopology,
                             model.VertexOffset,
                             model.VertexCount,
                             model.IndexOffset,
                             model.Indices.size(),
                             component->Entity,
                             component->Entities,
                             [](cEntity* entity, sRenderInstance& instance) {
                                 auto* materialComponent = entity->Get<sCMaterial>();
                                 if (materialComponent != nullptr) {
                                     instance.MaterialIndex = materialComponent->Index;
                                 }
                             }
                     );

                     cSkeletonManager::Unbind(skeleton.Index);
                 }
            });
        }

        void cSkeletonPass::DrawTransparent(cScene *scene)
        {
            scene->EachComponent<sCSkeletonModel>([this](sCSkeletonModel* component)
            {
                 if (component->Visible && component->Transparent) {
                     auto& model = *component;
                     auto& skeleton = component->Skeleton;

                     cSkeletonManager::Flush(skeleton.Index);
                     cSkeletonManager::Bind(skeleton.Index);

                     DrawInstanced(
                             model.PrimitiveTopology,
                             model.VertexOffset,
                             model.VertexCount,
                             model.IndexOffset,
                             model.Indices.size(),
                             component->Entity,
                             component->Entities,
                             [](cEntity* entity, sRenderInstance& instance) {
                                 auto* materialComponent = entity->Get<sCMaterial>();
                                 if (materialComponent != nullptr) {
                                     instance.MaterialIndex = materialComponent->Index;
                                 }
                             }
                     );

                     cSkeletonManager::Unbind(skeleton.Index);
                 }
            });
        }

        void cSkeletonPass::DrawShadow(cScene* scene)
        {
            scene->EachComponent<sCDirectionalLight>([this, scene](sCDirectionalLight* lightComponent) {
                sViewMatrix lightView;
                lightView.Position = lightComponent->Position;
                lightView.Front = glm::vec3(0, 0, 0);
                lightView.Up = glm::vec3(0, 1, 0);
                glm::mat4x4 lightMatrix = MathManager::UpdateLightMatrix(*lightComponent, lightView);

                scene->EachComponent<sCSkeletonModel>([this, &lightMatrix](sCSkeletonModel* component)
                {
                     if (component->Visible && component->CastShadow) {
                         auto& model = *component;
                         auto& skeleton = component->Skeleton;

                         cSkeletonManager::Flush(skeleton.Index);
                         cSkeletonManager::Bind(skeleton.Index);

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

                         cSkeletonManager::Unbind(skeleton.Index);
                     }
                });
            });

            scene->EachComponent<sCSpotLight>([this, scene](sCSpotLight* lightComponent) {
                sViewMatrix lightView;
                lightView.Position = lightComponent->Position;
                lightView.Front = glm::vec3(0, 0, 0);
                lightView.Up = glm::vec3(0, 1, 0);
                glm::mat4x4 lightMatrix = MathManager::UpdateLightMatrix(*lightComponent, lightView);

                scene->EachComponent<sCSkeletonModel>([this, &lightMatrix](sCSkeletonModel* component)
                {
                     if (component->Visible && component->CastShadow) {
                         auto& model = *component;
                         auto& skeleton = component->Skeleton;

                         cSkeletonManager::Flush(skeleton.Index);
                         cSkeletonManager::Bind(skeleton.Index);

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

                         cSkeletonManager::Unbind(skeleton.Index);
                     }
                });
            });
        }

    }

}