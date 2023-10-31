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

        void MainPass::DrawOpaque(cScene* scene) {
            scene->EachComponent<sCGeometry>([this](sCGeometry* component)
            {
                if (component->Visible && !component->Transparent) {
                    auto& geometry = *component;
                    DrawInstanced(
                            geometry.PrimitiveTopology,
                            geometry.VertexOffset,
                            geometry.Vertices.size(),
                            geometry.IndexOffset,
                            geometry.Indices.size(),
                            component->Entity,
                            component->Entities,
                            [](cEntity* entity, RenderInstance& instance) {
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
                if (component->Visible && !component->Transparent) {
                    auto& model = *component;
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
                }
            });
        }

        void MainPass::DrawTransparent(cScene* scene) {
            scene->EachComponent<sCGeometry>([this](sCGeometry* component)
            {
                if (component->Visible && component->Transparent) {
                    auto& geometry = *component;
                    DrawInstanced(
                            geometry.PrimitiveTopology,
                            geometry.VertexOffset,
                            geometry.Vertices.size(),
                            geometry.IndexOffset,
                            geometry.Indices.size(),
                            component->Entity,
                            component->Entities,
                            [](cEntity* entity, RenderInstance& instance) {
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
                 if (component->Visible && component->Transparent) {
                     auto& model = *component;
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
                 }
            });
        }

        void MainPass::DrawShadow(cScene* scene)
        {
            scene->EachComponent<sCDirectionalLight>([this, scene](sCDirectionalLight* lightComponent) {
                ViewMatrix lightView;
                lightView.Position = lightComponent->Position;
                lightView.Front = glm::vec3(0, 0, 0);
                lightView.Up = glm::vec3(0, 1, 0);
                glm::mat4x4 lightMatrix = LightMatrixUpdate(*lightComponent, lightView);

                scene->EachComponent<sCGeometry>([this, &lightMatrix](sCGeometry* component)
                {
                    if (component->Visible && component->CastShadow) {
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

            scene->EachComponent<sCSpotLight>([this, scene](sCSpotLight* lightComponent) {
                ViewMatrix lightView;
                lightView.Position = lightComponent->Position;
                lightView.Front = glm::vec3(0, 0, 0);
                lightView.Up = glm::vec3(0, 1, 0);
                glm::mat4x4 lightMatrix = LightMatrixUpdate(*lightComponent, lightView);

                scene->EachComponent<sCGeometry>([this, &lightMatrix](sCGeometry* component)
                {
                    if (component->Visible && component->CastShadow) {
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

            scene->EachComponent<sCDirectionalLight>([this, scene](sCDirectionalLight* lightComponent) {
                ViewMatrix lightView;
                lightView.Position = lightComponent->Position;
                lightView.Front = glm::vec3(0, 0, 0);
                lightView.Up = glm::vec3(0, 1, 0);
                glm::mat4x4 lightMatrix = LightMatrixUpdate(*lightComponent, lightView);

                scene->EachComponent<sCModel>([this, &lightMatrix](sCModel* component)
                {
                     if (component->Visible && component->CastShadow) {
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

            scene->EachComponent<sCSpotLight>([this, scene](sCSpotLight* lightComponent) {
                ViewMatrix lightView;
                lightView.Position = lightComponent->Position;
                lightView.Front = glm::vec3(0, 0, 0);
                lightView.Up = glm::vec3(0, 1, 0);
                glm::mat4x4 lightMatrix = LightMatrixUpdate(*lightComponent, lightView);

                scene->EachComponent<sCModel>([this, &lightMatrix](sCModel* component)
                {
                     if (component->Visible && component->CastShadow) {
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