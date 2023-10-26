#include <rendering/passes/main_pass.h>
#include <rendering/material/material_manager.h>

#include <ecs/scenes.hpp>

namespace xpe {

    namespace render {

        MainPass::MainPass(eType type, const vector<RenderPassBinding> &bindings) : InstancingPass(type, bindings)
        {
        }

        void MainPass::InitOpaque()
        {
            RenderPass::InitOpaque();
            MaterialManager::Bind(*m_Pipeline);
        }

        void MainPass::InitTransparent()
        {
            RenderPass::InitTransparent();
            MaterialManager::Bind(*m_Pipeline);
        }

        void MainPass::DrawOpaque(Scene* scene) {
            scene->EachComponent<GeometryComponent>([this](GeometryComponent* component)
            {
                if (!component->Transparent && component->Visible && component->Geometry.Get() != nullptr) {
                    auto& geometry = *component->Geometry;
                    DrawInstanced(
                            geometry.PrimitiveTopology,
                            geometry.VertexOffset,
                            geometry.Vertices.size(),
                            geometry.IndexOffset,
                            geometry.Indices.size(),
                            component->Entity,
                            component->Entities,
                            [](Entity* entity, RenderInstance& instance) {
                                auto* materialComponent = entity->GetComponent<MaterialComponent>(entity->GetTag());
                                if (materialComponent != nullptr) {
                                    instance.MaterialIndex = materialComponent->Material->Index;
                                }
                            }
                    );
                }
            });

            scene->EachComponent<ModelComponent>([this](ModelComponent* component)
            {
                if (!component->Transparent && component->Visible && component->Model.Get() != nullptr) {
                    auto& model = *component->Model;
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
                }
            });
        }

        void MainPass::DrawTransparent(Scene* scene) {
            scene->EachComponent<GeometryComponent>([this](GeometryComponent* component)
            {
                if (component->Transparent && component->Visible && component->Geometry.Get() != nullptr) {
                    auto& geometry = *component->Geometry;
                    DrawInstanced(
                            geometry.PrimitiveTopology,
                            geometry.VertexOffset,
                            geometry.Vertices.size(),
                            geometry.IndexOffset,
                            geometry.Indices.size(),
                            component->Entity,
                            component->Entities,
                            [](Entity* entity, RenderInstance& instance) {
                                auto* materialComponent = entity->GetComponent<MaterialComponent>(entity->GetTag());
                                if (materialComponent != nullptr) {
                                    instance.MaterialIndex = materialComponent->Material->Index;
                                }
                            }
                    );
                }
            });

            scene->EachComponent<ModelComponent>([this](ModelComponent* component)
            {
                 if (component->Transparent && component->Visible && component->Model.Get() != nullptr) {
                     auto& model = *component->Model;
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
                 }
            });
        }

        void MainPass::DrawShadow(Scene* scene)
        {
            scene->EachComponent<DirectLightComponent>([this, scene](DirectLightComponent* lightComponent) {
                ViewMatrix lightView;
                lightView.Position = lightComponent->Position;
                lightView.Front = -glm::normalize(lightComponent->Position);
                lightView.Up = glm::vec3(0, 0, 1);
                glm::mat4x4 lightMatrix = LightMatrixUpdate(lightComponent->Projection, lightView);

                scene->EachComponent<GeometryComponent>([this, &lightMatrix](GeometryComponent* component)
                {
                    if (!component->EmbeddedShadow && component->Visible && component->Geometry.Get() != nullptr) {
                        auto& geometry = *component->Geometry;
                        DrawInstanced(
                                geometry.PrimitiveTopology,
                                geometry.VertexOffset,
                                geometry.Vertices.size(),
                                geometry.IndexOffset,
                                geometry.Indices.size(),
                                component->Entity,
                                component->Entities,
                                {},
                                lightMatrix
                        );
                    }
                });
            });

            scene->EachComponent<SpotLightComponent>([this, scene](SpotLightComponent* lightComponent) {
                ViewMatrix lightView;
                lightView.Position = lightComponent->Position;
                lightView.Front = lightComponent->Direction;
                lightView.Up = glm::vec3(0, 0, 1);
                glm::mat4x4 lightMatrix = LightMatrixUpdate(lightComponent->Projection, lightView);

                scene->EachComponent<GeometryComponent>([this, &lightMatrix](GeometryComponent* component)
                {
                    if (!component->EmbeddedShadow && component->Visible && component->Geometry.Get() != nullptr) {
                        auto& geometry = *component->Geometry;
                        DrawInstanced(
                                geometry.PrimitiveTopology,
                                geometry.VertexOffset,
                                geometry.Vertices.size(),
                                geometry.IndexOffset,
                                geometry.Indices.size(),
                                component->Entity,
                                component->Entities,
                                {},
                                lightMatrix
                        );
                    }
                });
            });

            scene->EachComponent<DirectLightComponent>([this, scene](DirectLightComponent* lightComponent) {
                ViewMatrix lightView;
                lightView.Position = lightComponent->Position;
                lightView.Front = -glm::normalize(lightComponent->Position);
                lightView.Up = glm::vec3(0, 0, 1);
                glm::mat4x4 lightMatrix = LightMatrixUpdate(lightComponent->Projection, lightView);

                scene->EachComponent<ModelComponent>([this, &lightMatrix](ModelComponent* component)
                {
                     if (!component->EmbeddedShadow && component->Visible && component->Model.Get() != nullptr) {
                         auto& model = *component->Model;
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
                     }
                });
            });

            scene->EachComponent<SpotLightComponent>([this, scene](SpotLightComponent* lightComponent) {
                ViewMatrix lightView;
                lightView.Position = lightComponent->Position;
                lightView.Front = lightComponent->Direction;
                lightView.Up = glm::vec3(0, 0, 1);
                glm::mat4x4 lightMatrix = LightMatrixUpdate(lightComponent->Projection, lightView);

                scene->EachComponent<ModelComponent>([this, &lightMatrix](ModelComponent* component)
                {
                     if (!component->EmbeddedShadow && component->Visible && component->Model.Get() != nullptr) {
                         auto& model = *component->Model;
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
                     }
                });
            });
        }

    }

}