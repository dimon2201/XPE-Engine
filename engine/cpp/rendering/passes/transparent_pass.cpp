#include <rendering/passes/transparent_pass.h>
#include <rendering/material/material_manager.h>

#include <ecs/scenes.hpp>

namespace xpe {

    namespace render {

        TransparentPass::TransparentPass(const vector<RenderPassBinding>& bindings) : InstancingPass(bindings)
        {
            m_Pipeline->DepthStencil.DepthWriteMask = eDepthWriteMask::ZERO;

            BlendTarget target;
            target.Enable = true;
            target.Src = eBlend::ONE;
            target.Dest = eBlend::ONE;
            target.BlendOp = eBlendOp::ADD;
            target.SrcAlpha = eBlend::ONE;
            target.DestAlpha = eBlend::ONE;
            target.BlendOpAlpha = eBlendOp::ADD;
            m_Pipeline->Blending.Targets.push_back(target);
            target.Enable = true;
            target.Src = eBlend::ZERO;
            target.Dest = eBlend::INV_SRC_COLOR;
            target.BlendOp = eBlendOp::ADD;
            target.SrcAlpha = eBlend::ZERO;
            target.DestAlpha = eBlend::ZERO;
            target.BlendOpAlpha = eBlendOp::ADD;
            m_Pipeline->Blending.Targets.push_back(target);
            m_Pipeline->Blending.IndependentBlendEnable = true;

            MaterialManager::Bind(*m_Pipeline);
        }

        void TransparentPass::Draw(Scene* scene)
        {
            scene->EachComponent<GeometryComponent>([this](GeometryComponent* component)
                {
                    if (component->Entity->GetComponent<TransparentComponent>("Transparent") != nullptr &&
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
                    if (component->Entity->GetComponent<TransparentComponent>("Transparent") != nullptr &&
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