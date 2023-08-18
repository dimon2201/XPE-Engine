#include <rendering/draw/instance_drawer.h>

#include <rendering/buffers/light_buffers.h>

#include <ecs/scenes.hpp>

namespace xpe {

    namespace render {

        InstanceDrawer::InstanceDrawer(
            CameraBuffer* cameraBuffer,
            Shader* shader,
            const VertexFormat& vertexFormat,
            GeometryStorage* geometryStorage,
            MaterialStorage* materialStorage,
            DirectLightBuffer* directLightBuffer,
            PointLightBuffer* pointLightBuffer,
            SpotLightBuffer* spotLightBuffer,
            SkeletStorage* skeletStorage,
            SkinStorage* skinStorage
        ) : Drawer(cameraBuffer, shader),
        m_GeometryStorage(geometryStorage),
        m_MaterialStorage(materialStorage),
        m_SkeletStorage(skeletStorage),
        m_SkinStorage(skinStorage)

        {
            m_InstanceBuffer.Reserve(1000);
            m_TransformBuffer.Reserve(1000);

            m_Pipeline->InputLayout.Format = vertexFormat;
            m_Pipeline->VSBuffers.emplace_back(&m_InstanceBuffer);
            m_Pipeline->VSBuffers.emplace_back(&m_TransformBuffer);

            materialStorage->BindPipeline(*m_Pipeline);

            m_Pipeline->PSBuffers.emplace_back(directLightBuffer);
            m_Pipeline->PSBuffers.emplace_back(pointLightBuffer);
            m_Pipeline->PSBuffers.emplace_back(spotLightBuffer);

            m_Pipeline->DepthStencilState.UseDepthTest = K_TRUE;
            m_Pipeline->BlendState.UseBlending = K_TRUE;
            m_Pipeline->Rasterizer.CullMode = eCullMode::NONE;

            Init();
        }

        InstanceDrawer::~InstanceDrawer() {}

        void InstanceDrawer::Draw(Scene* scene, RenderTarget* renderTarget)
        {
            m_Pipeline->RenderTarget = renderTarget;
            Bind();

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

            scene->EachComponent<SkinComponent>([this](SkinComponent* component)
            {
                DrawSkin(component->Skin, component->Skelet, component->Transform);
            });

            scene->EachComponent<SkinListComponent>([this](SkinListComponent* component)
            {
                DrawSkinList(component->Skin, component->Skelet, component->Transforms);
            });

            scene->EachComponent<SkinModelComponent>([this](SkinModelComponent* component)
            {
                DrawSkinModel(component->Model, component->Skelet, component->Transform);
            });

            scene->EachComponent<SkinModelListComponent>([this](SkinModelListComponent* component)
            {
                DrawSkinModelList(component->Model, component->Skelet, component->Transforms);
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
            instance.MaterialIndex = m_MaterialStorage->GetIndex(materialInstance.Material.Get());

            m_InstanceBuffer.Clear();
            m_InstanceBuffer.Add(instance);
            m_InstanceBuffer.Flush();

            m_TransformBuffer.Clear();
            m_TransformBuffer.AddTransform(materialInstance.Transform);
            m_TransformBuffer.Flush();

            auto& vertexBuffer = m_GeometryStorage->GetVertexBuffer3D(geometry.Get());

            context::BindPrimitiveTopology(geometry->PrimitiveTopology);
            context::BindVertexBuffer(&vertexBuffer);
            context::BindVSBuffer(&m_InstanceBuffer);
            context::BindVSBuffer(&m_TransformBuffer);
            context::DrawVertexed(0, vertexBuffer.NumElements, 1);
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

            m_InstanceBuffer.Clear();
            m_InstanceBuffer.Add(instance);
            m_InstanceBuffer.Flush();

            m_TransformBuffer.Clear();
            m_TransformBuffer.AddTransform(materialInstance.Transform);
            m_TransformBuffer.Flush();

            auto& vertexBuffer = m_GeometryStorage->GetVertexBuffer3D(geometry.Get());
            auto& indexBuffer = m_GeometryStorage->GetIndexBuffer(geometry.Get());

            context::BindPrimitiveTopology(geometry->PrimitiveTopology);
            context::BindVertexBuffer(&vertexBuffer);
            context::BindIndexBuffer(&indexBuffer);
            context::BindVSBuffer(&m_InstanceBuffer);
            context::BindVSBuffer(&m_TransformBuffer);
            context::DrawIndexed(0, 0, indexBuffer.NumElements, 1);
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

            auto& vertexBuffer = m_GeometryStorage->GetVertexBuffer3D(geometry.Get());

            context::BindPrimitiveTopology(geometry->PrimitiveTopology);
            context::BindVertexBuffer(&vertexBuffer);
            context::BindVSBuffer(&m_InstanceBuffer);
            context::BindVSBuffer(&m_TransformBuffer);
            context::DrawVertexed(0, vertexBuffer.NumElements, instanceCount);
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

            auto& vertexBuffer = m_GeometryStorage->GetVertexBuffer3D(geometry.Get());
            auto& indexBuffer = m_GeometryStorage->GetIndexBuffer(geometry.Get());

            context::BindPrimitiveTopology(geometry->PrimitiveTopology);
            context::BindVertexBuffer(&vertexBuffer);
            context::BindIndexBuffer(&indexBuffer);
            context::BindVSBuffer(&m_InstanceBuffer);
            context::BindVSBuffer(&m_TransformBuffer);
            context::DrawIndexed(0, 0, indexBuffer.NumElements, instanceCount);
        }

        void InstanceDrawer::DrawMesh(const Ref<Mesh> &mesh, const Transform &transform)
        {
            if (mesh.Get() == nullptr) return;

            RenderInstance instance;
            instance.TransformIndex = 0;
            instance.CameraIndex = 0;
            instance.MaterialIndex = m_MaterialStorage->GetIndex(mesh->Material.Get());

            m_InstanceBuffer.Clear();
            m_InstanceBuffer.Add(instance);
            m_InstanceBuffer.Flush();

            m_TransformBuffer.Clear();
            m_TransformBuffer.AddTransform(transform);
            m_TransformBuffer.Flush();

            auto& vertexBuffer = m_GeometryStorage->GetVertexBuffer3D(mesh.Get());
            auto& indexBuffer = m_GeometryStorage->GetIndexBuffer(mesh.Get());

            context::BindPrimitiveTopology(mesh->PrimitiveTopology);
            context::BindVertexBuffer(&vertexBuffer);
            context::BindIndexBuffer(&indexBuffer);
            context::BindVSBuffer(&m_InstanceBuffer);
            context::BindVSBuffer(&m_TransformBuffer);
            context::DrawIndexed(0, 0, indexBuffer.NumElements, 1);
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

            auto& vertexBuffer = m_GeometryStorage->GetVertexBuffer3D(mesh.Get());
            auto& indexBuffer = m_GeometryStorage->GetIndexBuffer(mesh.Get());

            context::BindPrimitiveTopology(mesh->PrimitiveTopology);
            context::BindVertexBuffer(&vertexBuffer);
            context::BindIndexBuffer(&indexBuffer);
            context::BindVSBuffer(&m_InstanceBuffer);
            context::BindVSBuffer(&m_TransformBuffer);
            context::DrawIndexed(0, 0, indexBuffer.NumElements, instanceCount);
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

            auto& vertexBuffer = m_GeometryStorage->GetVertexBuffer3D(model.Get());
            auto& indexBuffer = m_GeometryStorage->GetIndexBuffer(model.Get());

            context::BindPrimitiveTopology(model->PrimitiveTopology);
            context::BindVertexBuffer(&vertexBuffer);
            context::BindIndexBuffer(&indexBuffer);
            context::BindVSBuffer(&m_InstanceBuffer);
            context::BindVSBuffer(&m_TransformBuffer);
            context::DrawIndexed(0, 0, indexBuffer.NumElements, instanceCount);
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
                    instance.MaterialIndex = m_MaterialStorage->GetIndex(model->Meshes[j].Material.Get());
                    m_InstanceBuffer.Add(instance);
                }

                m_TransformBuffer.AddTransform(transforms[i]);
            }
            m_InstanceBuffer.Flush();
            m_TransformBuffer.Flush();

            auto& vertexBuffer = m_GeometryStorage->GetVertexBuffer3D(model.Get());
            auto& indexBuffer = m_GeometryStorage->GetIndexBuffer(model.Get());

            context::BindPrimitiveTopology(model->PrimitiveTopology);
            context::BindVertexBuffer(&vertexBuffer);
            context::BindIndexBuffer(&indexBuffer);
            context::BindVSBuffer(&m_InstanceBuffer);
            context::BindVSBuffer(&m_TransformBuffer);
            context::DrawIndexed(0, 0, indexBuffer.NumElements, instanceCount);
        }

        void InstanceDrawer::DrawSkin(const Ref<Skin> &skin, const Ref<Skelet>& skelet, const Transform &transform)
        {
            if (skin.Get() == nullptr) return;

            RenderInstance instance;
            instance.TransformIndex = 0;
            instance.CameraIndex = 0;
            instance.MaterialIndex = m_MaterialStorage->GetIndex(skin->Material.Get());

            m_InstanceBuffer.Clear();
            m_InstanceBuffer.Add(instance);
            m_InstanceBuffer.Flush();

            m_TransformBuffer.Clear();
            m_TransformBuffer.AddTransform(transform);
            m_TransformBuffer.Flush();

            auto& vertexBuffer = m_SkinStorage->GetVertexBuffer(skin.Get());
            auto& indexBuffer = m_SkinStorage->GetIndexBuffer(skin.Get());

            if (skelet.Get() != nullptr) {
                auto& boneBuffer = m_SkeletStorage->GetBoneBuffer(skelet.Get());
                context::BindVSBuffer(&boneBuffer);
            }

            context::BindPrimitiveTopology(skin->PrimitiveTopology);
            context::BindVertexBuffer(&vertexBuffer);
            context::BindIndexBuffer(&indexBuffer);
            context::BindVSBuffer(&m_InstanceBuffer);
            context::BindVSBuffer(&m_TransformBuffer);
            context::DrawIndexed(0, 0, indexBuffer.NumElements, 1);
        }

        void InstanceDrawer::DrawSkinList(const Ref<Skin> &skin, const Ref<Skelet>& skelet, const vector<Transform> &transforms)
        {
            if (skin.Get() == nullptr) return;

            usize instanceCount = transforms.size();
            m_InstanceBuffer.Clear();
            m_TransformBuffer.Clear();
            for (usize i = 0 ; i < instanceCount ; i++)
            {
                RenderInstance instance;
                instance.TransformIndex = i;
                instance.CameraIndex = 0;
                instance.MaterialIndex = m_MaterialStorage->GetIndex(skin->Material.Get());

                m_InstanceBuffer.Add(instance);
                m_TransformBuffer.AddTransform(transforms[i]);
            }
            m_InstanceBuffer.Flush();
            m_TransformBuffer.Flush();

            auto& vertexBuffer = m_SkinStorage->GetVertexBuffer(skin.Get());
            auto& indexBuffer = m_SkinStorage->GetIndexBuffer(skin.Get());

            if (skelet.Get() != nullptr) {
                auto& boneBuffer = m_SkeletStorage->GetBoneBuffer(skelet.Get());
                context::BindVSBuffer(&boneBuffer);
            }

            context::BindPrimitiveTopology(skin->PrimitiveTopology);
            context::BindVertexBuffer(&vertexBuffer);
            context::BindIndexBuffer(&indexBuffer);
            context::BindVSBuffer(&m_InstanceBuffer);
            context::BindVSBuffer(&m_TransformBuffer);
            context::DrawIndexed(0, 0, indexBuffer.NumElements, instanceCount);
        }

        void InstanceDrawer::DrawSkinModel(const Ref<SkinModel> &model, const Ref<Skelet>& skelet, const Transform &transform)
        {
            if (model.Get() == nullptr) return;

            u32 instanceCount = model->Skins.size();
            m_InstanceBuffer.Clear();
            m_TransformBuffer.Clear();
            for (usize i = 0 ; i < instanceCount ; i++)
            {
                RenderInstance instance;
                instance.TransformIndex = i;
                instance.CameraIndex = 0;
                instance.MaterialIndex = m_MaterialStorage->GetIndex(model->Skins[i].Material.Get());

                m_InstanceBuffer.Add(instance);
                m_TransformBuffer.AddTransform(transform);
            }
            m_InstanceBuffer.Flush();
            m_TransformBuffer.Flush();

            auto& vertexBuffer = m_SkinStorage->GetVertexBuffer(model.Get());
            auto& indexBuffer = m_SkinStorage->GetIndexBuffer(model.Get());

            if (skelet.Get() != nullptr) {
                auto& boneBuffer = m_SkeletStorage->GetBoneBuffer(skelet.Get());
                context::BindVSBuffer(&boneBuffer);
            }

            context::BindPrimitiveTopology(model->PrimitiveTopology);
            context::BindVertexBuffer(&vertexBuffer);
            context::BindIndexBuffer(&indexBuffer);
            context::BindVSBuffer(&m_InstanceBuffer);
            context::BindVSBuffer(&m_TransformBuffer);
            context::DrawIndexed(0, 0, indexBuffer.NumElements, instanceCount);
        }

        void InstanceDrawer::DrawSkinModelList(const Ref<SkinModel> &model, const Ref<Skelet>& skelet, const vector<Transform> &transforms)
        {
            if (model.Get() == nullptr) return;

            u32 skinCount = model->Skins.size();
            u32 transformCount = transforms.size();
            u32 instanceCount = skinCount * transformCount;
            m_InstanceBuffer.Clear();
            m_TransformBuffer.Clear();
            for (usize i = 0 ; i < transformCount ; i++)
            {
                RenderInstance instance;
                instance.TransformIndex = i;

                for (usize j = 0 ; j < skinCount ; j++)
                {
                    instance.CameraIndex = 0;
                    instance.MaterialIndex = m_MaterialStorage->GetIndex(model->Skins[j].Material.Get());
                    m_InstanceBuffer.Add(instance);
                }

                m_TransformBuffer.AddTransform(transforms[i]);
            }
            m_InstanceBuffer.Flush();
            m_TransformBuffer.Flush();

            auto& vertexBuffer = m_SkinStorage->GetVertexBuffer(model.Get());
            auto& indexBuffer = m_SkinStorage->GetIndexBuffer(model.Get());
            if (skelet.Get() != nullptr) {
                auto& boneBuffer = m_SkeletStorage->GetBoneBuffer(skelet.Get());
                context::BindVSBuffer(&boneBuffer);
            }

            context::BindPrimitiveTopology(model->PrimitiveTopology);
            context::BindVertexBuffer(&vertexBuffer);
            context::BindIndexBuffer(&indexBuffer);
            context::BindVSBuffer(&m_InstanceBuffer);
            context::BindVSBuffer(&m_TransformBuffer);
            context::DrawIndexed(0, 0, indexBuffer.NumElements, instanceCount);
        }

    }

}