#pragma once

#include <geometry/geometries.h>

namespace xpe {

    namespace render {

        struct ENGINE_API GeometryStorage : public Object
        {
            Ref<GeometryVertexed<Vertex2D>> AddGeometryVertexed2D(const string& name, const GeometryVertexed<Vertex2D>& geometry);
            Ref<GeometryVertexed<Vertex3D>> AddGeometryVertexed3D(const string& name, const GeometryVertexed<Vertex3D>& geometry);
            Ref<GeometryIndexed<Vertex2D>> AddGeometryIndexed2D(const string& name, const GeometryIndexed<Vertex2D>& geometry);
            Ref<GeometryIndexed<Vertex3D>> AddGeometryIndexed3D(const string& name, const GeometryIndexed<Vertex3D>& geometry);

            Ref<Mesh> AddMesh(const string& name, const Mesh& mesh);
            Ref<Model3D> AddModel(const string& name, const Model3D& model);

            void RemoveGeometryVertexed2D(const string& name);
            void RemoveGeometryVertexed3D(const string& name);
            void RemoveGeometryIndexed2D(const string& name);
            void RemoveGeometryIndexed3D(const string& name);

            void RemoveMesh(const string& name);
            void RemoveModel(const string& name);

            inline Ref<GeometryVertexed<Vertex2D>>& GetGeometryVertexed2D(const string& name) { return m_GeometryVertexed2D[name]; }
            inline Ref<GeometryVertexed<Vertex3D>>& GetGeometryVertexed3D(const string& name) { return m_GeometryVertexed3D[name]; }
            inline Ref<GeometryIndexed<Vertex2D>>& GetGeometryIndexed2D(const string& name) { return m_GeometryIndexed2D[name]; }
            inline Ref<GeometryIndexed<Vertex3D>>& GetGeometryIndexed3D(const string& name) { return m_GeometryIndexed3D[name]; }

            inline Ref<Mesh>& GetMesh(const string& name) { return m_Meshes[name]; }
            inline Ref<Model3D>& GetModel(const string& name) { return m_Models[name]; }

            void Clear();

        private:
            unordered_map<string, Ref<GeometryVertexed<Vertex2D>>> m_GeometryVertexed2D;
            unordered_map<string, Ref<GeometryVertexed<Vertex3D>>> m_GeometryVertexed3D;
            unordered_map<string, Ref<GeometryIndexed<Vertex2D>>> m_GeometryIndexed2D;
            unordered_map<string, Ref<GeometryIndexed<Vertex3D>>> m_GeometryIndexed3D;

            unordered_map<string, Ref<Mesh>> m_Meshes;
            unordered_map<string, Ref<Model3D>> m_Models;
        };

    }

}