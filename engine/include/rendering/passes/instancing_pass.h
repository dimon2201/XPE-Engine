#pragma once

#include <rendering/passes/render_pass.h>
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

        class ENGINE_API InstancingPass : public RenderPass
        {

        public:
            InstancingPass(
                const core::vector<RenderPassBinding>& bindings,
                RenderTarget* output,
                MaterialStorage* materialStorage
            );
            ~InstancingPass() override;

            virtual void Update(Scene* scene) override final;
            virtual void Draw(Scene* scene) override final;

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