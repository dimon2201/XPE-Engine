#include <rendering/passes/geometry_pass.h>
#include <rendering/material/material_manager.h>

#include <anim/skeleton_manager.h>

#include <ecs/components.hpp>

namespace xpe {

    namespace render {

        cGeometryPass::cGeometryPass(eType type, const vector<sRenderPassBinding> &bindings) : cInstancingPass(type, bindings)
        {
            for (auto* stage : m_Pipeline->Shader->Stages)
            {
                if (stage->Type == sShaderStage::eType::VERTEX) {
                    stage->Buffers.emplace_back(cSkeletonManager::GetBuffer());
                }
            }
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

        void cGeometryPass::DrawOpaque(cScene* scene)
        {
            // Draw geometry
            {
                auto components = scene->GetComponents<COpaque, CGeometryInfo>();
                for (auto [entity, opaque, geometryInfo]: components.each()) {
                    DrawInstanced(
                            scene,
                            geometryInfo,
                            [scene](EntityID entityId, sRenderInstance &instance) {
                                instance.MaterialIndex = scene->GetComponent<CMaterial>(entityId).Index;
                            }
                    );
                }
            }
            // Draw skeletons
            {
                auto components = scene->GetComponents<COpaque, CSkeletonInfo>();
                for (auto [entity, opaque, skeletonInfo]: components.each()) {
                    DrawInstanced(
                            scene,
                            skeletonInfo.GeometryInfo,
                            [scene, &skeletonInfo](EntityID entityId, sRenderInstance &instance) {
                                instance.MaterialIndex = scene->GetComponent<CMaterial>(entityId).Index;
                                instance.SkeletonIndex = skeletonInfo.SkeletonIndex;
                            }
                    );
                }
            }
        }

        void cGeometryPass::DrawTransparent(cScene* scene)
        {
            // Draw geometry
            {
                auto components = scene->GetComponents<CTransparent, CGeometryInfo>();
                for (auto [entity, transparent, geometryInfo]: components.each()) {
                    DrawInstanced(
                            scene,
                            geometryInfo,
                            [scene](EntityID entityId, sRenderInstance &instance) {
                                instance.MaterialIndex = scene->GetComponent<CMaterial>(entityId).Index;
                            }
                    );
                }
            }
            // Draw skeletons
            {
                auto components = scene->GetComponents<CTransparent, CSkeletonInfo>();
                for (auto [entity, transparent, skeletonInfo]: components.each()) {
                    DrawInstanced(
                            scene,
                            skeletonInfo.GeometryInfo,
                            [scene, &skeletonInfo](EntityID entityId, sRenderInstance &instance) {
                                instance.MaterialIndex = scene->GetComponent<CMaterial>(entityId).Index;
                                instance.SkeletonIndex = skeletonInfo.SkeletonIndex;
                            }
                    );
                }
            }
        }

        void cGeometryPass::DrawShadow(cScene* scene)
        {
            // Draw geometry
            {
                auto components = scene->GetComponents<CHasShadow, CGeometryInfo>();
                for (auto [entity, hasShadow, geometryInfo]: components.each()) {
                    DrawInstanced(
                            scene,
                            geometryInfo
                    );
                }
            }
            // Draw skeletons
            {
                auto components = scene->GetComponents<CHasShadow, CSkeletonInfo>();
                for (auto [entity, hasShadow, skeletonInfo]: components.each()) {
                    DrawInstanced(
                            scene,
                            skeletonInfo.GeometryInfo,
                            [&skeletonInfo](EntityID entityId, sRenderInstance &instance) {
                                instance.SkeletonIndex = skeletonInfo.SkeletonIndex;
                            }
                    );
                }
            }
        }

    }

}