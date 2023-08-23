#include <rendering/draw/instance_drawer.h>
#include <rendering/buffers/light_buffers.h>

#include <ecs/scenes.hpp>

namespace xpe {

    namespace render {

        InstanceDrawer::InstanceDrawer(
            CameraBuffer* cameraBuffer,
            Shader* shader,
            GeometryStorage* geometryStorage,
            MaterialStorage* materialStorage,
            DirectLightBuffer* directLightBuffer,
            PointLightBuffer* pointLightBuffer,
            SpotLightBuffer* spotLightBuffer
        ) : Drawer(cameraBuffer, shader),
        m_GeometryStorage(geometryStorage),
        m_MaterialStorage(materialStorage)

        {
            m_InstanceBuffer.Reserve(1000);
            m_TransformBuffer.Reserve(1000);

            m_Pipeline->InputLayout.Format = Vertex3D::Format;

            materialStorage->BindPipeline(*m_Pipeline);

            m_Pipeline->PSBuffers.emplace_back(directLightBuffer);
            m_Pipeline->PSBuffers.emplace_back(pointLightBuffer);
            m_Pipeline->PSBuffers.emplace_back(spotLightBuffer);

            Init();
        }

        InstanceDrawer::~InstanceDrawer() {}

        void InstanceDrawer::Draw(Scene* scene)
        {
            scene->EachComponent<GeometryVertexed3DComponent>([this](GeometryVertexed3DComponent* component)
            {
                DrawGeometryVertexed(component->Geometry, component->Instance);
            });

            scene->EachComponent<GeometryIndexed3DComponent>([this](GeometryIndexed3DComponent* component)
            {
                DrawGeometryIndexed(component->Geometry, component->Instance);
            });

            scene->EachComponent<GeometryVertexed3DListComponent>([this](GeometryVertexed3DListComponent* component)
            {
                DrawGeometryVertexedList(component->Geometry, component->Instances);
            });

            scene->EachComponent<GeometryIndexed3DListComponent>([this](GeometryIndexed3DListComponent* component)
            {
                DrawGeometryIndexedList(component->Geometry, component->Instances);
            });

            scene->EachComponent<MeshComponent>([this](MeshComponent* component)
            {
                DrawMesh(component->Mesh, component->Transform);
            });

            scene->EachComponent<MeshListComponent>([this](MeshListComponent* component)
            {
                DrawMeshList(component->Mesh, component->Transforms);
            });

            scene->EachComponent<ModelComponent>([this](ModelComponent* component)
            {
                DrawModel(component->Model, component->Transform);
            });

            scene->EachComponent<ModelListComponent>([this](ModelListComponent* component)
            {
                DrawModelList(component->Model, component->Transforms);
            });
        }

        void InstanceDrawer::DrawGeometryVertexed(
                const Ref<GeometryVertexed<Vertex3D>> &geometry,
                const MaterialInstance& materialInstance
        ) {
            if (geometry.Get() == nullptr) return;

            RenderInstance instance;
            instance.TransformIndex = 0;
            instance.CameraIndex = 0;
            instance.MaterialIndex = materialInstance.Material->Index;

            m_InstanceBuffer.Clear();
            m_InstanceBuffer.Add(instance);
            m_InstanceBuffer.Flush();

            m_TransformBuffer.Clear();
            m_TransformBuffer.AddTransform(materialInstance.Transform);
            m_TransformBuffer.Flush();

            context::BindPrimitiveTopology(geometry->PrimitiveTopology);
            context::BindVertexBuffer(geometry->Vertices);
            context::BindVSBuffer(m_InstanceBuffer);
            context::BindVSBuffer(m_TransformBuffer);
            context::DrawVertexed(0, geometry->Vertices.NumElements, 1);
            context::UnbindVSBuffer(m_InstanceBuffer);
            context::UnbindVSBuffer(m_TransformBuffer);
        }

        void InstanceDrawer::DrawGeometryIndexed(
                const Ref<GeometryIndexed<Vertex3D>> &geometry,
                const MaterialInstance& materialInstance
        ) {
            if (geometry.Get() == nullptr) return;

            RenderInstance instance;
            instance.TransformIndex = 0;
            instance.CameraIndex = 0;
            instance.MaterialIndex = materialInstance.Material->Index;

            m_InstanceBuffer.Clear();
            m_InstanceBuffer.Add(instance);
            m_InstanceBuffer.Flush();

            m_TransformBuffer.Clear();
            m_TransformBuffer.AddTransform(materialInstance.Transform);
            m_TransformBuffer.Flush();

            context::BindPrimitiveTopology(geometry->PrimitiveTopology);
            context::BindVertexBuffer(geometry->Vertices);
            context::BindIndexBuffer(geometry->Indices);
            context::BindVSBuffer(m_InstanceBuffer);
            context::BindVSBuffer(m_TransformBuffer);
            context::DrawIndexed(0, 0, geometry->Indices.NumElements, 1);
            context::UnbindVSBuffer(m_InstanceBuffer);
            context::UnbindVSBuffer(m_TransformBuffer);
        }

        void InstanceDrawer::DrawGeometryVertexedList(
                const Ref<GeometryVertexed<Vertex3D>> &geometry,
                const vector<MaterialInstance>& instances
        ) {
            if (geometry.Get() == nullptr) return;

            usize instanceCount = instances.size();
            m_InstanceBuffer.Clear();
            m_TransformBuffer.Clear();
            for (usize i = 0 ; i < instanceCount ; i++)
            {
                auto& materialInstance = instances[i];

                RenderInstance instance;
                instance.TransformIndex = i;
                instance.CameraIndex = 0;
                instance.MaterialIndex = materialInstance.Material->Index;

                m_InstanceBuffer.Add(instance);
                m_TransformBuffer.AddTransform(materialInstance.Transform);
            }
            m_InstanceBuffer.Flush();
            m_TransformBuffer.Flush();

            context::BindPrimitiveTopology(geometry->PrimitiveTopology);
            context::BindVertexBuffer(geometry->Vertices);
            context::BindVSBuffer(m_InstanceBuffer);
            context::BindVSBuffer(m_TransformBuffer);
            context::DrawVertexed(0, geometry->Vertices.NumElements, instanceCount);
            context::UnbindVSBuffer(m_InstanceBuffer);
            context::UnbindVSBuffer(m_TransformBuffer);
        }

        void InstanceDrawer::DrawGeometryIndexedList(
                const Ref<GeometryIndexed<Vertex3D>> &geometry,
                const vector<MaterialInstance>& instances
        ) {
            if (geometry.Get() == nullptr) return;

            usize instanceCount = instances.size();
            m_InstanceBuffer.Clear();
            m_TransformBuffer.Clear();
            for (usize i = 0 ; i < instanceCount ; i++)
            {
                auto& materialInstance = instances[i];

                RenderInstance instance;
                instance.TransformIndex = i;
                instance.CameraIndex = 0;
                instance.MaterialIndex = materialInstance.Material->Index;

                m_InstanceBuffer.Add(instance);
                m_TransformBuffer.AddTransform(materialInstance.Transform);
            }
            m_InstanceBuffer.Flush();
            m_TransformBuffer.Flush();

            context::BindPrimitiveTopology(geometry->PrimitiveTopology);
            context::BindVertexBuffer(geometry->Vertices);
            context::BindIndexBuffer(geometry->Indices);
            context::BindVSBuffer(m_InstanceBuffer);
            context::BindVSBuffer(m_TransformBuffer);
            context::DrawIndexed(0, 0, geometry->Indices.NumElements, instanceCount);
            context::UnbindVSBuffer(m_InstanceBuffer);
            context::UnbindVSBuffer(m_TransformBuffer);
        }

        void InstanceDrawer::DrawMesh(const Ref<Mesh> &mesh, const Transform &transform)
        {
            if (mesh.Get() == nullptr) return;

            RenderInstance instance;
            instance.TransformIndex = 0;
            instance.CameraIndex = 0;
            instance.MaterialIndex = mesh->Material->Index;

            m_InstanceBuffer.Clear();
            m_InstanceBuffer.Add(instance);
            m_InstanceBuffer.Flush();

            m_TransformBuffer.Clear();
            m_TransformBuffer.AddTransform(transform);
            m_TransformBuffer.Flush();

            context::BindPrimitiveTopology(mesh->PrimitiveTopology);
            context::BindVertexBuffer(mesh->Vertices);
            context::BindIndexBuffer(mesh->Indices);
            context::BindVSBuffer(m_InstanceBuffer);
            context::BindVSBuffer(m_TransformBuffer);
            context::DrawIndexed(0, 0, mesh->Indices.NumElements, 1);
            context::UnbindVSBuffer(m_InstanceBuffer);
            context::UnbindVSBuffer(m_TransformBuffer);
        }

        void InstanceDrawer::DrawMeshList(const Ref<Mesh> &mesh, const vector<Transform> &transforms)
        {
            if (mesh.Get() == nullptr) return;

            usize instanceCount = transforms.size();
            m_InstanceBuffer.Clear();
            m_TransformBuffer.Clear();
            for (usize i = 0 ; i < instanceCount ; i++)
            {
                RenderInstance instance;
                instance.TransformIndex = i;
                instance.CameraIndex = 0;
                instance.MaterialIndex = mesh->Material->Index;

                m_InstanceBuffer.Add(instance);
                m_TransformBuffer.AddTransform(transforms[i]);
            }
            m_InstanceBuffer.Flush();
            m_TransformBuffer.Flush();

            context::BindPrimitiveTopology(mesh->PrimitiveTopology);
            context::BindVertexBuffer(mesh->Vertices);
            context::BindIndexBuffer(mesh->Indices);
            context::BindVSBuffer(m_InstanceBuffer);
            context::BindVSBuffer(m_TransformBuffer);
            context::DrawIndexed(0, 0, mesh->Indices.NumElements, instanceCount);
            context::UnbindVSBuffer(m_InstanceBuffer);
            context::UnbindVSBuffer(m_TransformBuffer);
        }

        void InstanceDrawer::DrawModel(const Ref<Model3D> &model, const Transform &transform)
        {
            if (model.Get() == nullptr) return;

            u32 instanceCount = model->Meshes.size();
            m_InstanceBuffer.Clear();
            m_TransformBuffer.Clear();
            for (usize i = 0 ; i < instanceCount ; i++)
            {
                RenderInstance instance;
                instance.TransformIndex = i;
                instance.CameraIndex = 0;
                instance.MaterialIndex = model->Meshes[i].Material->Index;

                m_InstanceBuffer.Add(instance);
                m_TransformBuffer.AddTransform(transform);
            }
            m_InstanceBuffer.Flush();
            m_TransformBuffer.Flush();

            context::BindPrimitiveTopology(model->PrimitiveTopology);
            context::BindVertexBuffer(model->Vertices);
            context::BindIndexBuffer(model->Indices);
            context::BindVSBuffer(m_InstanceBuffer);
            context::BindVSBuffer(m_TransformBuffer);
            context::DrawIndexed(0, 0, model->Indices.NumElements, instanceCount);
            context::UnbindVSBuffer(m_InstanceBuffer);
            context::UnbindVSBuffer(m_TransformBuffer);
        }

        void InstanceDrawer::DrawModelList(const Ref<Model3D> &model, const vector<Transform> &transforms)
        {
            if (model.Get() == nullptr) return;

            u32 meshCount = model->Meshes.size();
            u32 transformCount = transforms.size();
            u32 instanceCount = meshCount * transformCount;
            m_InstanceBuffer.Clear();
            m_TransformBuffer.Clear();
            for (usize i = 0 ; i < transformCount ; i++)
            {
                RenderInstance instance;
                instance.TransformIndex = i;

                for (usize j = 0 ; j < meshCount ; j++)
                {
                    instance.CameraIndex = 0;
                    instance.MaterialIndex = model->Meshes[j].Material->Index;
                    m_InstanceBuffer.Add(instance);
                }

                m_TransformBuffer.AddTransform(transforms[i]);
            }
            m_InstanceBuffer.Flush();
            m_TransformBuffer.Flush();

            context::BindPrimitiveTopology(model->PrimitiveTopology);
            context::BindVertexBuffer(model->Vertices);
            context::BindIndexBuffer(model->Indices);
            context::BindVSBuffer(m_InstanceBuffer);
            context::BindVSBuffer(m_TransformBuffer);
            context::DrawIndexed(0, 0, model->Indices.NumElements, instanceCount);
            context::UnbindVSBuffer(m_InstanceBuffer);
            context::UnbindVSBuffer(m_TransformBuffer);
        }

    }

}