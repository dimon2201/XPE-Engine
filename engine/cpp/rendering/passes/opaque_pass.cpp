#include <rendering/passes/opaque_pass.h>
#include <rendering/material/material_manager.h>

#include <ecs/scenes.hpp>

namespace xpe {

    namespace render {

        OpaquePass::OpaquePass(const vector<RenderPassBinding> &bindings) : InstancingPass(bindings)
        {
            m_Pipeline->DepthStencil.DepthWriteMask = eDepthWriteMask::ALL;

            BlendTarget target;
            target.Enable = false;
            m_Pipeline->Blending.Targets.push_back(target);
            m_Pipeline->Blending.Targets.push_back(target);
            m_Pipeline->Blending.Targets.push_back(target);
            m_Pipeline->Blending.IndependentBlendEnable = true;

            MaterialManager::Bind(*m_Pipeline);
        }

        void OpaquePass::Draw(Scene* scene)
        {
            scene->EachComponent<GeometryComponent>([this](GeometryComponent* component)
            {
                if (component->Entity->GetComponent<OpaqueComponent>("Opaque") != nullptr &&
                    component->Geometry.Get() != nullptr) {
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
                 if (component->Entity->GetComponent<OpaqueComponent>() != nullptr &&
                     component->Model.Get() != nullptr) {
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