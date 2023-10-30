#include <rendering/passes/main_pass.h>
#include <rendering/material/material_manager.h>

#include <ecs/components.hpp>

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
                if (!component->Transparent && component->Visible && !component->HasSkeleton) {
                    auto& geometry = *component;
                    DrawInstanced(
                            geometry.PrimitiveTopology,
                            geometry.VertexOffset,
                            geometry.Vertices.size(),
                            geometry.IndexOffset,
                            geometry.Indices.size(),
                            component->Entity,
                            component->Entities,
                            [](Entity* entity, RenderInstance& instance) {
                                auto* materialComponent = entity->Get<MaterialComponent>();
                                if (materialComponent != nullptr) {
                                    instance.MaterialIndex = materialComponent->Index;
                                }
                            }
                    );
                }
            });

            scene->EachComponent<ModelComponent>([this](ModelComponent* component)
            {
                if (!component->Transparent && component->Visible && !component->HasSkeleton) {
                    auto& model = *component;
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
                }
            });
        }

        void MainPass::DrawTransparent(Scene* scene) {
            scene->EachComponent<GeometryComponent>([this](GeometryComponent* component)
            {
                if (component->Transparent && component->Visible && !component->HasSkeleton) {
                    auto& geometry = *component;
                    DrawInstanced(
                            geometry.PrimitiveTopology,
                            geometry.VertexOffset,
                            geometry.Vertices.size(),
                            geometry.IndexOffset,
                            geometry.Indices.size(),
                            component->Entity,
                            component->Entities,
                            [](Entity* entity, RenderInstance& instance) {
                                auto* materialComponent = entity->Get<MaterialComponent>();
                                if (materialComponent != nullptr) {
                                    instance.MaterialIndex = materialComponent->Index;
                                }
                            }
                    );
                }
            });

            scene->EachComponent<ModelComponent>([this](ModelComponent* component)
            {
                 if (component->Transparent && component->Visible && !component->HasSkeleton) {
                     auto& model = *component;
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
                 }
            });
        }

        void MainPass::DrawShadow(Scene* scene)
        {
            scene->EachComponent<DirectLightComponent>([this, scene](DirectLightComponent* lightComponent) {
                ViewMatrix lightView;
                lightView.Position = lightComponent->Position;
                lightView.Front = glm::vec3(0, 0, 0);
                lightView.Up = glm::vec3(0, 1, 0);
                glm::mat4x4 lightMatrix = LightMatrixUpdate(*lightComponent, lightView);

                scene->EachComponent<GeometryComponent>([this, &lightMatrix](GeometryComponent* component)
                {
                    if (component->CastShadow && component->Visible && !component->HasSkeleton) {
                        auto& geometry = *component;
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
                lightView.Front = glm::vec3(0, 0, 0);
                lightView.Up = glm::vec3(0, 1, 0);
                glm::mat4x4 lightMatrix = LightMatrixUpdate(*lightComponent, lightView);

                scene->EachComponent<GeometryComponent>([this, &lightMatrix](GeometryComponent* component)
                {
                    if (component->CastShadow && component->Visible && !component->HasSkeleton) {
                        auto& geometry = *component;
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
                lightView.Front = glm::vec3(0, 0, 0);
                lightView.Up = glm::vec3(0, 1, 0);
                glm::mat4x4 lightMatrix = LightMatrixUpdate(*lightComponent, lightView);

                scene->EachComponent<ModelComponent>([this, &lightMatrix](ModelComponent* component)
                {
                     if (component->CastShadow && component->Visible && !component->HasSkeleton) {
                         auto& model = *component;
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
                lightView.Front = glm::vec3(0, 0, 0);
                lightView.Up = glm::vec3(0, 1, 0);
                glm::mat4x4 lightMatrix = LightMatrixUpdate(*lightComponent, lightView);

                scene->EachComponent<ModelComponent>([this, &lightMatrix](ModelComponent* component)
                {
                     if (component->CastShadow && component->Visible && !component->HasSkeleton) {
                         auto& model = *component;
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