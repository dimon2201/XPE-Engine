#pragma once

#include <rendering/draw/drawer.h>

#include <rendering/buffers/instance_buffer.h>
#include <rendering/buffers/transform_buffer.h>

#include <rendering/storages/geometry_storage.h>
#include <rendering/storages/material_storage.h>

#include <anim/storages/skelet_storage.h>
#include <anim/storages/skin_storage.h>

namespace xpe {

    namespace ecs
    {
        class Scene;
    }

    namespace render {

        using namespace core;
        using namespace math;
        using namespace ecs;
        using namespace anim;

        class DirectLightBuffer;
        class PointLightBuffer;
        class SpotLightBuffer;

        class ENGINE_API InstanceDrawer : public Drawer
        {

        public:
            InstanceDrawer(
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
            );

            ~InstanceDrawer() override;

            void Draw(Scene* scene, RenderTarget* renderTarget) override;

        private:
            void DrawGeometryVertexed(const Ref<GeometryVertexed<Vertex3D>>& geometry, const MaterialInstance& instance);
            void DrawGeometryIndexed(const Ref<GeometryIndexed<Vertex3D>>& geometry, const MaterialInstance& instance);

            void DrawGeometryVertexedList(const Ref<GeometryVertexed<Vertex3D>>& geometry, const vector<MaterialInstance>& instances);
            void DrawGeometryIndexedList(const Ref<GeometryIndexed<Vertex3D>>& geometry, const vector<MaterialInstance>& instances);

            void DrawMesh(const Ref<Mesh>& mesh, const Transform& transform);
            void DrawMeshList(const Ref<Mesh>& mesh, const vector<Transform>& transforms);

            void DrawModel(const Ref<Model3D>& model, const Transform& transform);
            void DrawModelList(const Ref<Model3D>& model, const vector<Transform>& transforms);

            void DrawSkin(const Ref<Skin>& skin, const Ref<Skelet>& skelet, const Transform& transform);
            void DrawSkinList(const Ref<Skin>& skin, const Ref<Skelet>& skelet, const vector<Transform>& transforms);

            void DrawSkinModel(const Ref<SkinModel>& model, const Ref<Skelet>& skelet, const Transform& transform);
            void DrawSkinModelList(const Ref<SkinModel>& model, const Ref<Skelet>& skelet, const vector<Transform>& transforms);

        protected:
            GeometryStorage* m_GeometryStorage;
            MaterialStorage* m_MaterialStorage;
            SkinStorage* m_SkinStorage;
            SkeletStorage* m_SkeletStorage;

            InstanceBuffer m_InstanceBuffer;
            TransformBuffer m_TransformBuffer;
        };

    }

}