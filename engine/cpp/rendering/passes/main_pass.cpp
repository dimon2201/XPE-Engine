#include <rendering/passes/main_pass.h>
#include <rendering/material/material_manager.h>

#include <ecs/scenes.hpp>

namespace xpe {

    namespace render {

        MainPass::MainPass(const vector<RenderPassBinding> &bindings) : InstancingPass(bindings)
        {
            MaterialManager::Bind(*m_Pipeline);
        }

        void MainPass::Draw(Scene* scene)
        {
            scene->EachComponent<GeometryComponent>([this](GeometryComponent* component)
            {
                if (component->Geometry.Get() != nullptr) {
                    auto& geometry = *component->Geometry;
                    DrawInstanced(
                        geometry.PrimitiveTopology,
                        geometry.VertexOffset,
                        geometry.Vertices.size(),
                        geometry.IndexOffset,
                        geometry.Indices.size(),
                        component->Entity,
                        component->Entities
                    );
                }
            });

            scene->EachComponent<ModelComponent>([this](ModelComponent* component)
            {
                 if (component->Model.Get() != nullptr) {
                     auto& model = *component->Model;
                     DrawInstanced(
                             model.PrimitiveTopology,
                             model.VertexOffset,
                             model.VertexCount,
                             model.IndexOffset,
                             model.Indices.size(),
                             component->Entity,
                             component->Entities
                     );
                 }
            });
        }

    }

}