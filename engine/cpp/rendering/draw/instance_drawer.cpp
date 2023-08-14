#include <rendering/draw/instance_drawer.h>

#include <ecs/scenes.hpp>

namespace xpe {

    namespace render {

        InstanceDrawer::InstanceDrawer(
            CameraBuffer* cameraBuffer,
            Shader* shader,
            GeometryStorage* geometryStorage,
            MaterialStorage* materialStorage
        ) : Drawer(cameraBuffer, shader), m_GeometryStorage(geometryStorage), m_MaterialStorage(materialStorage)
        {
            m_InstanceBuffer.Reserve(1000);
            m_TransformBuffer.Reserve(1000);

            m_Pipeline->InputLayout.Format = Vertex3D::Format;
            m_Pipeline->VSBuffers.emplace_back(&m_InstanceBuffer);
            m_Pipeline->VSBuffers.emplace_back(&m_TransformBuffer);

            materialStorage->BindPipeline(*m_Pipeline);

            m_DirectLightBuffer.Reserve(1000);
            m_PointLightBuffer.Reserve(1000);
            m_SpotLightBuffer.Reserve(1000);
            m_Pipeline->PSBuffers.emplace_back(&m_DirectLightBuffer);
            m_Pipeline->PSBuffers.emplace_back(&m_PointLightBuffer);
            m_Pipeline->PSBuffers.emplace_back(&m_SpotLightBuffer);

            m_Pipeline->DepthStencilState.UseDepthTest = K_TRUE;
            m_Pipeline->BlendState.UseBlending = K_TRUE;

            Init();
        }

        InstanceDrawer::~InstanceDrawer() {}

        void InstanceDrawer::Draw(Scene* scene, RenderTarget* renderTarget)
        {
            m_Pipeline->RenderTarget = renderTarget;

            FlushLights(scene);

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

        void InstanceDrawer::FlushLights(Scene* scene)
        {
            m_DirectLightBuffer.Clear();
            scene->EachComponent<DirectLightComponent>([this](DirectLightComponent* component) {
                m_DirectLightBuffer.AddComponent(*component);
            });
            m_DirectLightBuffer.Flush();

            m_PointLightBuffer.Clear();
            scene->EachComponent<PointLightComponent>([this](PointLightComponent* component) {
                m_PointLightBuffer.AddComponent(*component);
            });
            m_PointLightBuffer.Flush();

            m_SpotLightBuffer.Clear();
            scene->EachComponent<SpotLightComponent>([this](SpotLightComponent* component) {
                m_SpotLightBuffer.AddComponent(*component);
            });
            m_SpotLightBuffer.Flush();
        }

        void InstanceDrawer::DrawGeometryVertexed(
                const Ref<GeometryVertexed<Vertex3D>> &geometry,
                const MaterialInstance& materialInstance
        ) {
            if (geometry.Get() == nullptr) return;

            RenderInstance instance;
            instance.TransformIndex = 0;
            instance.CameraIndex = 0;
            instance.MaterialIndex = m_MaterialStorage->GetIndex(materialInstance.Material.Get());

            m_Pipeline->PrimitiveTopology = geometry->PrimitiveTopology;
            m_Pipeline->VertexBuffer = &m_GeometryStorage->GetVertexBuffer3D(geometry.Get());
            m_Pipeline->IndexBuffer = nullptr;

            m_InstanceBuffer.Clear();
            m_InstanceBuffer.Add(instance);
            m_InstanceBuffer.Flush();

            m_TransformBuffer.Clear();
            m_TransformBuffer.AddTransform(materialInstance.Transform);
            m_TransformBuffer.Flush();

            Bind();
            context::DrawVertexed(0, m_Pipeline->VertexBuffer->NumElements, 1);
        }

        void InstanceDrawer::DrawGeometryIndexed(
                const Ref<GeometryIndexed<Vertex3D>> &geometry,
                const MaterialInstance& materialInstance
        ) {
            if (geometry.Get() == nullptr) return;

            RenderInstance instance;
            instance.TransformIndex = 0;
            instance.CameraIndex = 0;
            instance.MaterialIndex = m_MaterialStorage->GetIndex(materialInstance.Material.Get());

            m_Pipeline->PrimitiveTopology = geometry->PrimitiveTopology;
            m_Pipeline->VertexBuffer = &m_GeometryStorage->GetVertexBuffer3D(geometry.Get());
            m_Pipeline->IndexBuffer = &m_GeometryStorage->GetIndexBuffer(geometry.Get());

            m_InstanceBuffer.Clear();
            m_InstanceBuffer.Add(instance);
            m_InstanceBuffer.Flush();

            m_TransformBuffer.Clear();
            m_TransformBuffer.AddTransform(materialInstance.Transform);
            m_TransformBuffer.Flush();

            Bind();
            context::DrawIndexed(0, 0, m_Pipeline->IndexBuffer->NumElements, 1);
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
                instance.MaterialIndex = m_MaterialStorage->GetIndex(materialInstance.Material.Get());

                m_InstanceBuffer.Add(instance);
                m_TransformBuffer.AddTransform(materialInstance.Transform);
            }
            m_InstanceBuffer.Flush();
            m_TransformBuffer.Flush();

            m_Pipeline->PrimitiveTopology = geometry->PrimitiveTopology;
            m_Pipeline->VertexBuffer = &m_GeometryStorage->GetVertexBuffer3D(geometry.Get());
            m_Pipeline->IndexBuffer = nullptr;

            Bind();
            context::DrawVertexed(0, m_Pipeline->VertexBuffer->NumElements, instanceCount);
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
                instance.MaterialIndex = m_MaterialStorage->GetIndex(materialInstance.Material.Get());

                m_InstanceBuffer.Add(instance);
                m_TransformBuffer.AddTransform(materialInstance.Transform);
            }
            m_InstanceBuffer.Flush();
            m_TransformBuffer.Flush();

            m_Pipeline->PrimitiveTopology = geometry->PrimitiveTopology;
            m_Pipeline->VertexBuffer = &m_GeometryStorage->GetVertexBuffer3D(geometry.Get());
            m_Pipeline->IndexBuffer = &m_GeometryStorage->GetIndexBuffer(geometry.Get());

            Bind();
            context::DrawIndexed(0, 0, m_Pipeline->IndexBuffer->NumElements, instanceCount);
        }

        void InstanceDrawer::DrawMesh(const Ref<Mesh> &mesh, const Transform &transform)
        {
            if (mesh.Get() == nullptr) return;

            RenderInstance instance;
            instance.TransformIndex = 0;
            instance.CameraIndex = 0;
            instance.MaterialIndex = m_MaterialStorage->GetIndex(mesh->Material.Get());

            m_Pipeline->PrimitiveTopology = mesh->PrimitiveTopology;
            m_Pipeline->VertexBuffer = &m_GeometryStorage->GetVertexBuffer3D(mesh.Get());
            m_Pipeline->IndexBuffer = &m_GeometryStorage->GetIndexBuffer(mesh.Get());

            m_InstanceBuffer.Clear();
            m_InstanceBuffer.Add(instance);
            m_InstanceBuffer.Flush();

            m_TransformBuffer.Clear();
            m_TransformBuffer.AddTransform(transform);
            m_TransformBuffer.Flush();

            Bind();
            context::DrawIndexed(0, 0, m_Pipeline->IndexBuffer->NumElements, 1);
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
                instance.MaterialIndex = m_MaterialStorage->GetIndex(mesh->Material.Get());

                m_InstanceBuffer.Add(instance);
                m_TransformBuffer.AddTransform(transforms[i]);
            }
            m_InstanceBuffer.Flush();
            m_TransformBuffer.Flush();

            m_Pipeline->PrimitiveTopology = mesh->PrimitiveTopology;
            m_Pipeline->VertexBuffer = &m_GeometryStorage->GetVertexBuffer3D(mesh.Get());
            m_Pipeline->IndexBuffer = &m_GeometryStorage->GetIndexBuffer(mesh.Get());

            Bind();
            context::DrawIndexed(0, 0, m_Pipeline->IndexBuffer->NumElements, instanceCount);
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
                instance.MaterialIndex = m_MaterialStorage->GetIndex(model->Meshes[i].Material.Get());

                m_InstanceBuffer.Add(instance);
                m_TransformBuffer.AddTransform(transform);
            }
            m_InstanceBuffer.Flush();
            m_TransformBuffer.Flush();

            m_Pipeline->PrimitiveTopology = model->PrimitiveTopology;
            m_Pipeline->VertexBuffer = &m_GeometryStorage->GetVertexBuffer3D(model.Get());
            m_Pipeline->IndexBuffer = &m_GeometryStorage->GetIndexBuffer(model.Get());

            Bind();
            context::DrawIndexed(0, 0, m_Pipeline->IndexBuffer->NumElements, instanceCount);
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
                    instance.MaterialIndex = m_MaterialStorage->GetIndex(model->Meshes[i].Material.Get());
                    m_InstanceBuffer.Add(instance);
                }

                m_TransformBuffer.AddTransform(transforms[i]);
            }
            m_InstanceBuffer.Flush();
            m_TransformBuffer.Flush();

            m_Pipeline->PrimitiveTopology = model->PrimitiveTopology;
            m_Pipeline->VertexBuffer = &m_GeometryStorage->GetVertexBuffer3D(model.Get());
            m_Pipeline->IndexBuffer = &m_GeometryStorage->GetIndexBuffer(model.Get());

            Bind();
            context::DrawIndexed(0, 0, m_Pipeline->IndexBuffer->NumElements, instanceCount);
        }

    }

}