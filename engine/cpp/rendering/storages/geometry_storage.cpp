#include <rendering/storages/geometry_storage.h>

namespace xpe {

    namespace render {

        Ref<GeometryVertexed<Vertex2D>>
        GeometryStorage::AddGeometryVertexed2D(const string &name, const GeometryVertexed<Vertex2D> &geometry)
        {
            Ref<GeometryVertexed<Vertex2D>> geometryRef;
            geometryRef.Create(geometry);
            geometryRef->Vertices.Flush();
            m_GeometryVertexed2D.insert({name, geometryRef });
            return geometryRef;
        }

        Ref<GeometryVertexed<Vertex3D>>
        GeometryStorage::AddGeometryVertexed3D(const string &name, const GeometryVertexed<Vertex3D> &geometry)
        {
            Ref<GeometryVertexed<Vertex3D>> geometryRef;
            geometryRef.Create(geometry);
            geometryRef->Vertices.Flush();
            m_GeometryVertexed3D.insert({name, geometryRef });
            return geometryRef;
        }

        Ref<GeometryIndexed<Vertex2D>>
        GeometryStorage::AddGeometryIndexed2D(const string &name, const GeometryIndexed<Vertex2D> &geometry)
        {
            Ref<GeometryIndexed<Vertex2D>> geometryRef;
            geometryRef.Create(geometry);
            geometryRef->Vertices.Flush();
            geometryRef->Indices.Flush();
            m_GeometryIndexed2D.insert({ name, geometryRef });
            return geometryRef;
        }

        Ref<GeometryIndexed<Vertex3D>>
        GeometryStorage::AddGeometryIndexed3D(const string &name, const GeometryIndexed<Vertex3D> &geometry)
        {
            Ref<GeometryIndexed<Vertex3D>> geometryRef;
            geometryRef.Create(geometry);
            geometryRef->Vertices.Flush();
            geometryRef->Indices.Flush();
            m_GeometryIndexed3D.insert({ name, geometryRef });
            return geometryRef;
        }

        Ref<Mesh> GeometryStorage::AddMesh(const string &name, const Mesh &mesh)
        {
            Ref<Mesh> meshRef;
            meshRef.Create(mesh);
            meshRef->Vertices.Flush();
            meshRef->Indices.Flush();
            m_Meshes.insert({ name, meshRef });
            return meshRef;
        }

        Ref<Model3D> GeometryStorage::AddModel(const string &name, const Model3D &model)
        {
            Ref<Model3D> modelRef;
            modelRef.Create(model);
            modelRef->Vertices.Reserve(100000);
            modelRef->Indices.Reserve(100000);
            m_Models.insert({name, modelRef });

            u32 modelVertexCount = 0;
            for (auto& mesh : model.Meshes)
            {
                for (auto& vertex : mesh.Vertices.List)
                {
                    modelRef->Vertices.List.emplace_back(vertex);
                }

                for (auto& index : mesh.Indices.List)
                {
                    modelRef->Indices.List.emplace_back(index + modelVertexCount);
                }

                modelVertexCount += mesh.Vertices.List.size();
            }

            modelRef->Vertices.Flush();
            modelRef->Indices.Flush();

            return modelRef;
        }

        void GeometryStorage::RemoveGeometryVertexed2D(const string &name)
        {
            auto it = m_GeometryVertexed2D.find(name);
            if (it != m_GeometryVertexed2D.end()) {
                m_GeometryVertexed2D.erase(it);
            }
        }

        void GeometryStorage::RemoveGeometryVertexed3D(const string &name)
        {
            auto it = m_GeometryVertexed3D.find(name);
            if (it != m_GeometryVertexed3D.end()) {
                m_GeometryVertexed3D.erase(it);
            }
        }

        void GeometryStorage::RemoveGeometryIndexed2D(const string &name)
        {
            auto it = m_GeometryIndexed2D.find(name);
            if (it != m_GeometryIndexed2D.end()) {
                m_GeometryIndexed2D.erase(it);
            }
        }

        void GeometryStorage::RemoveGeometryIndexed3D(const string &name)
        {
            auto it = m_GeometryIndexed3D.find(name);
            if (it != m_GeometryIndexed3D.end()) {
                m_GeometryIndexed3D.erase(it);
            }
        }

        void GeometryStorage::RemoveMesh(const string &name)
        {
            auto it = m_Meshes.find(name);
            if (it != m_Meshes.end()) {
                m_Meshes.erase(it);
            }
        }

        void GeometryStorage::RemoveModel(const string &name)
        {
            auto it = m_Models.find(name);
            if (it != m_Models.end()) {
                m_Models.erase(it);
            }
        }

        void GeometryStorage::Clear()
        {
            m_GeometryVertexed2D.clear();
            m_GeometryVertexed3D.clear();
            m_GeometryIndexed2D.clear();
            m_GeometryIndexed3D.clear();

            m_Meshes.clear();
            m_Models.clear();
        }

    }

}