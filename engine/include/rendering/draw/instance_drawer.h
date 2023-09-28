#pragma once

#include <rendering/draw/drawer.h>
#include <rendering/buffers/instance_buffer.h>
#include <rendering/buffers/transform_buffer.h>
#include <rendering/storages/geometry_storage.h>
#include <rendering/storages/material_storage.h>

namespace xpe {

    namespace ecs
    {
        class Scene;
    }

    namespace render {

        using namespace core;
        using namespace math;
        using namespace ecs;

        class DirectLightBuffer;
        class PointLightBuffer;
        class SpotLightBuffer;

        class ENGINE_API InstanceDrawer : public Drawer
        {

        public:
            InstanceDrawer(
                Shader* shader,
                RenderTarget* renderTarget,
                MaterialStorage* materialStorage,
                const vector<Buffer*>& VSBuffers = {},
                const vector<Buffer*>& PSBuffers = {}
            );

            ~InstanceDrawer() override;

            void Draw(Scene* scene) override;

        private:
            void DrawGeometryVertexed(const Ref<GeometryVertexed<Vertex3D>>& geometry, const MaterialInstance& instance);
            void DrawGeometryIndexed(const Ref<GeometryIndexed<Vertex3D>>& geometry, const MaterialInstance& instance);

            void DrawGeometryVertexedList(const Ref<GeometryVertexed<Vertex3D>>& geometry, const vector<MaterialInstance>& instances);
            void DrawGeometryIndexedList(const Ref<GeometryIndexed<Vertex3D>>& geometry, const vector<MaterialInstance>& instances);

            void DrawMesh(const Ref<Mesh>& mesh, const Transform& transform);
            void DrawMeshList(const Ref<Mesh>& mesh, const vector<Transform>& transforms);

            void DrawModel(const Ref<Model3D>& model, const Transform& transform);
            void DrawModelList(const Ref<Model3D>& model, const vector<Transform>& transforms);

        protected:
            InstanceBuffer m_InstanceBuffer;
            TransformBuffer m_TransformBuffer;
        };

    }

}