#include <rendering/passes/geometry_pass.h>
#include <rendering/material/material_manager.h>

#include <anim/skeleton_manager.h>

#include <ecs/components.hpp>

namespace xpe {

    namespace render {

        cGeometryPass::cGeometryPass(eType type, const vector<sRenderPassBinding> &bindings) : cInstancingPass(type, bindings)
        {
            m_Pipeline->VSBuffers.emplace_back(cSkeletonManager::GetBuffer());
        }

        void cGeometryPass::InitOpaque()
        {
            cRenderPass::InitOpaque();
            cMaterialManager::Bind(*m_Pipeline);
        }

        void cGeometryPass::InitTransparent()
        {
            cRenderPass::InitTransparent();
            cMaterialManager::Bind(*m_Pipeline);
        }

        void cGeometryPass::DrawOpaque(cScene* scene) {
            scene->EachComponent<sCGeometry>([this](sCGeometry* component)
            {
                 if (!component->Transparent) {
                     auto& geometry = *component;
                     DrawInstanced(
                             geometry.PrimitiveTopology,
                             geometry.VertexOffset,
                             geometry.Vertices.size(),
                             geometry.IndexOffset,
                             geometry.Indices.size(),
                             component->Entity,
                             component->Entities,
                             [](cEntity* entity, sRenderInstance& instance) {
                                 auto* materialComponent = entity->Get<sCMaterial>();
                                 if (materialComponent != nullptr) {
                                     instance.MaterialIndex = materialComponent->Index;
                                 }
                             }
                     );
                 }
            });

            scene->EachComponent<sCModel>([this](sCModel* component)
            {
                  if (!component->Transparent) {
                      auto& model = *component;
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
                  }
            });

            scene->EachComponent<sCSkeletonModel>([this](sCSkeletonModel* component)
            {
                  if (!component->Transparent) {
                      auto& model = *component;
                      auto& skeleton = component->Skeleton;
                      DrawInstanced(
                              model.PrimitiveTopology,
                              model.VertexOffset,
                              model.VertexCount,
                              model.IndexOffset,
                              model.Indices.size(),
                              component->Entity,
                              component->Entities,
                              [&skeleton](cEntity* entity, sRenderInstance& instance) {
                                  auto* materialComponent = entity->Get<sCMaterial>();
                                  if (materialComponent != nullptr) {
                                      instance.MaterialIndex = materialComponent->Index;
                                  }
                                  instance.SkeletonIndex = skeleton.Index;
                              }
                      );
                  }
            });
        }

        void cGeometryPass::DrawTransparent(cScene* scene) {
            scene->EachComponent<sCGeometry>([this](sCGeometry* component)
            {
                 if (component->Transparent) {
                     auto& geometry = *component;
                     DrawInstanced(
                             geometry.PrimitiveTopology,
                             geometry.VertexOffset,
                             geometry.Vertices.size(),
                             geometry.IndexOffset,
                             geometry.Indices.size(),
                             component->Entity,
                             component->Entities,
                             [](cEntity* entity, sRenderInstance& instance) {
                                 auto* materialComponent = entity->Get<sCMaterial>();
                                 if (materialComponent != nullptr) {
                                     instance.MaterialIndex = materialComponent->Index;
                                 }
                             }
                     );
                 }
            });

            scene->EachComponent<sCModel>([this](sCModel* component)
            {
                  if (component->Transparent) {
                      auto& model = *component;
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
                  }
            });

            scene->EachComponent<sCSkeletonModel>([this](sCSkeletonModel* component)
            {
                  if (component->Transparent) {
                      auto& model = *component;
                      auto& skeleton = component->Skeleton;
                      DrawInstanced(
                              model.PrimitiveTopology,
                              model.VertexOffset,
                              model.VertexCount,
                              model.IndexOffset,
                              model.Indices.size(),
                              component->Entity,
                              component->Entities,
                              [&skeleton](cEntity* entity, sRenderInstance& instance) {
                                  auto* materialComponent = entity->Get<sCMaterial>();
                                  if (materialComponent != nullptr) {
                                      instance.MaterialIndex = materialComponent->Index;
                                  }
                                  instance.SkeletonIndex = skeleton.Index;
                              }
                      );
                  }
            });
        }

        void cGeometryPass::DrawShadow(cScene* scene)
        {
            scene->EachComponent<sCGeometry>([this](sCGeometry* component)
            {
                 if (component->CastShadow) {
                     auto& geometry = *component;
                     DrawInstanced(
                             geometry.PrimitiveTopology,
                             geometry.VertexOffset,
                             geometry.Vertices.size(),
                             geometry.IndexOffset,
                             geometry.Indices.size(),
                             component->Entity,
                             component->Entities,
                             {}
                     );
                 }
            });

            scene->EachComponent<sCModel>([this](sCModel* component)
            {
                  if (component->CastShadow) {
                      auto& model = *component;
                      DrawInstanced(
                              model.PrimitiveTopology,
                              model.VertexOffset,
                              model.VertexCount,
                              model.IndexOffset,
                              model.Indices.size(),
                              component->Entity,
                              component->Entities,
                              {}
                      );
                  }
            });

            scene->EachComponent<sCSkeletonModel>([this](sCSkeletonModel* component)
            {
                  if (component->CastShadow) {
                      auto& model = *component;
                      auto& skeleton = component->Skeleton;
                      DrawInstanced(
                              model.PrimitiveTopology,
                              model.VertexOffset,
                              model.VertexCount,
                              model.IndexOffset,
                              model.Indices.size(),
                              component->Entity,
                              component->Entities,
                              [&skeleton](cEntity* entity, sRenderInstance& instance) {
                                  instance.SkeletonIndex = skeleton.Index;
                              }
                      );
                  }
            });
        }

    }

}