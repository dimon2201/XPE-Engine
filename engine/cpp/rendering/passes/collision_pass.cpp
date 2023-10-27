#include <rendering/passes/collision_pass.h>
#include <physics/collision_manager.h>

#include <ecs/scenes.hpp>

namespace xpe {

    namespace render {

        CollisionPass::CollisionPass(
            const vector<RenderPassBinding>& bindings,
            RenderTarget* output,
            MaterialStorage* materialStorage
        ) : InstancingPass(bindings, output, materialStorage) // need to delete materialStorage from this
        {
            m_Pipeline->InputLayout.Format = Vertex3D::Format;
            m_Pipeline->Rasterizer.FillMode = eFillMode::WIREFRAME;

            context::CreatePipeline(*m_Pipeline);
        }

        CollisionPass::~CollisionPass() {}

        void CollisionPass::Update(Scene* scene) {
            GeometryManager::BindVertexBuffer3D();
        };

        void CollisionPass::Draw(Scene* scene) {
            
            scene->EachComponent<DynamicCollisionComponent>([this](DynamicCollisionComponent* component) {
                if (!component->Shapes.empty()) {
                    for (auto shape : component->Shapes) {

                        Transform transform;
                        transform.Position = { 
                            shape->PosisionX.load(std::memory_order_relaxed),
                            shape->PosisionY.load(std::memory_order_relaxed),
                            shape->PosisionZ.load(std::memory_order_relaxed) 
                        };

                        transform.Rotation = {
                            shape->RotationX.load(std::memory_order_relaxed),
                            shape->RotationY.load(std::memory_order_relaxed),
                            shape->RotationZ.load(std::memory_order_relaxed)
                        };

                        transform.Scale = {
                            shape->ScaleX.load(std::memory_order_relaxed),
                            shape->ScaleY.load(std::memory_order_relaxed),
                            shape->ScaleZ.load(std::memory_order_relaxed)
                        };

                        auto& geometry = shape->Geometry;

                        DrawInstanced(
                            geometry->PrimitiveTopology,
                            geometry->VertexOffset,
                            geometry->Vertices.size(),
                            geometry->IndexOffset,
                            geometry->Indices.size(),
                            transform
                        );
                    }
                }
            });
        }

        void CollisionPass::DrawInstanced(
            ePrimitiveTopology primitiveTopology,
            usize vertexOffset,
            usize vertexCount,
            usize indexOffset,
            usize indexCount,
            Transform transform
        ) {
            DrawSingle(primitiveTopology, vertexOffset, vertexCount, indexOffset, indexCount, transform);
        }

        void CollisionPass::DrawSingle(
            ePrimitiveTopology primitiveTopology,
            usize vertexOffset,
            usize vertexCount,
            usize indexOffset,
            usize indexCount,
            Transform transform
        ) {
            m_InstanceBuffer.Clear();
            m_TransformBuffer.Clear();

            RenderInstance instance;
            instance.CameraIndex = 0;
            instance.TransformIndex = 0;

            m_InstanceBuffer.Add(instance);
            m_TransformBuffer.AddTransform(transform);

            m_InstanceBuffer.Flush();
            m_TransformBuffer.Flush();

            context::BindPrimitiveTopology(primitiveTopology);
            context::BindVSBuffer(m_InstanceBuffer);
            context::BindVSBuffer(m_TransformBuffer);

            if (indexCount == 0) {
                context::DrawVertexed(vertexCount, 1, vertexOffset);
            }
            else {
                context::DrawIndexed(indexCount, 1, vertexOffset, indexOffset);
            }

            context::UnbindVSBuffer(m_InstanceBuffer);
            context::UnbindVSBuffer(m_TransformBuffer);
        }

    }

}