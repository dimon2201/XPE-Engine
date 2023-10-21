#include <rendering/passes/opaque_pass.h>

#include <ecs/scenes.hpp>

namespace xpe {

    namespace render {

        OpaquePass::OpaquePass(
                const vector<RenderPassBinding> &bindings,
                MaterialStorage* materialStorage
        ) : InstancingPass(bindings)
        {
            m_Pipeline->DepthStencil.DepthWriteMask = eDepthWriteMask::ALL;

            BlendTarget target;
            target.Enable = false;
            m_Pipeline->Blending.Targets.push_back(target);
            m_Pipeline->Blending.Targets.push_back(target);
            m_Pipeline->Blending.Targets.push_back(target);

            context::CreatePipeline(*m_Pipeline);

            materialStorage->BindPipeline(*m_Pipeline);
        }

        void OpaquePass::Draw(Scene* scene)
        {
            scene->EachComponent<GeometryComponent<Vertex3D>>([this](GeometryComponent<Vertex3D>* component)
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

            scene->EachComponent<MeshComponent>([this](MeshComponent* component)
            {
                if (component->Entity->GetComponent<OpaqueComponent>() != nullptr &&
                    component->Mesh.Get() != nullptr) {
                    auto& mesh = *component->Mesh;
                    DrawInstanced(
                            mesh.PrimitiveTopology,
                            mesh.VertexOffset,
                            mesh.Vertices.size(),
                            mesh.IndexOffset,
                            mesh.Indices.size(),
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