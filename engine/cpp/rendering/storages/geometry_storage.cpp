#include <rendering/storages/geometry_storage.h>

namespace xpe {

    namespace render {

        Ref<GeometryVertexed<Vertex2D>>
        GeometryStorage::AddGeometryVertexed2D(const string &name, const GeometryVertexed<Vertex2D> &geometry)
        {
            Ref<GeometryVertexed<Vertex2D>> newGeometry;
            newGeometry.Create(geometry);
            m_GeometryVertexed2D.insert({ name, newGeometry });

            Ref<VertexBuffer<Vertex2D>> vertexBuffer;
            vertexBuffer.Create(geometry.Vertices);
            m_VertexBuffers2D.insert({ newGeometry.Get(), vertexBuffer });

            return newGeometry;
        }

        Ref<GeometryVertexed<Vertex3D>>
        GeometryStorage::AddGeometryVertexed3D(const string &name, const GeometryVertexed<Vertex3D> &geometry)
        {
            Ref<GeometryVertexed<Vertex3D>> newGeometry;
            newGeometry.Create(geometry);
            m_GeometryVertexed3D.insert({ name, newGeometry });

            Ref<VertexBuffer<Vertex3D>> vertexBuffer;
            vertexBuffer.Create(geometry.Vertices);
            m_VertexBuffers3D.insert({ newGeometry.Get(), vertexBuffer });

            return newGeometry;
        }

        Ref<GeometryIndexed<Vertex2D>>
        GeometryStorage::AddGeometryIndexed2D(const string &name, const GeometryIndexed<Vertex2D> &geometry)
        {
            Ref<GeometryIndexed<Vertex2D>> newGeometry;
            newGeometry.Create(geometry);
            m_GeometryIndexed2D.insert({ name, newGeometry });

            Ref<VertexBuffer<Vertex2D>> vertexBuffer;
            vertexBuffer.Create(geometry.Vertices);
            m_VertexBuffers2D.insert({ newGeometry.Get(), vertexBuffer });

            Ref<IndexBuffer> indexBuffer;
            indexBuffer.Create(geometry.Indices);
            m_IndexBuffers.insert({ newGeometry.Get(), indexBuffer });

            return newGeometry;
        }

        Ref<GeometryIndexed<Vertex3D>>
        GeometryStorage::AddGeometryIndexed3D(const string &name, const GeometryIndexed<Vertex3D> &geometry)
        {
            Ref<GeometryIndexed<Vertex3D>> newGeometry;
            newGeometry.Create(geometry);
            m_GeometryIndexed3D.insert({ name, newGeometry });

            Ref<VertexBuffer<Vertex3D>> vertexBuffer;
            vertexBuffer.Create(geometry.Vertices);
            m_VertexBuffers3D.insert({ newGeometry.Get(), vertexBuffer });

            Ref<IndexBuffer> indexBuffer;
            indexBuffer.Create(geometry.Indices);
            m_IndexBuffers.insert({ newGeometry.Get(), indexBuffer });

            return newGeometry;
        }

        Ref<Mesh> GeometryStorage::AddMesh(const string &name, const Mesh &mesh)
        {
            Ref<Mesh> meshRef;
            meshRef.Create(mesh);
            m_Meshes.insert({ name, meshRef });

            Ref<VertexBuffer<Vertex3D>> vertexBuffer;
            vertexBuffer.Create(mesh.Vertices);
            m_VertexBuffers3D.insert({ meshRef.Get(), vertexBuffer });

            Ref<IndexBuffer> indexBuffer;
            indexBuffer.Create(mesh.Indices);
            m_IndexBuffers.insert({ meshRef.Get(), indexBuffer });

            return meshRef;
        }

        Ref<Model3D> GeometryStorage::AddModel(const string &name, const Model3D &model)
        {
            Ref<Model3D> newModel;
            newModel.Create(model);
            m_Models.insert({ name, newModel });

            VertexArray<Vertex3D> vertexArray;
            vertexArray.Reserve(100000);
            IndexArray indexArray;
            indexArray.Reserve(100000);

            u32 modelVertexCount = 0;
            for (auto& mesh : model.Meshes)
            {
                for (auto& vertex : mesh.Vertices.Data)
                {
                    vertexArray.Data.emplace_back(vertex);
                }

                for (auto& index : mesh.Indices.Data)
                {
                    indexArray.Data.emplace_back(index + modelVertexCount);
                }

                modelVertexCount += mesh.Vertices.Count();
            }

            Ref<VertexBuffer<Vertex3D>> vertexBuffer;
            vertexBuffer.Create(vertexArray);
            m_VertexBuffers3D.insert({ newModel.Get(), vertexBuffer });

            Ref<IndexBuffer> indexBuffer;
            indexBuffer.Create(indexArray);
            m_IndexBuffers.insert({ newModel.Get(), indexBuffer });

            return newModel;
        }

        void GeometryStorage::RemoveGeometryVertexed2D(const string &name)
        {
            auto it = m_GeometryVertexed2D.find(name);
            if (it != m_GeometryVertexed2D.end()) {
                m_VertexBuffers2D.erase(it->second.Get());
                m_GeometryVertexed2D.erase(it);
            }
        }

        void GeometryStorage::RemoveGeometryVertexed3D(const string &name)
        {
            auto it = m_GeometryVertexed3D.find(name);
            if (it != m_GeometryVertexed3D.end()) {
                m_VertexBuffers3D.erase(it->second.Get());
                m_GeometryVertexed3D.erase(it);
            }
        }

        void GeometryStorage::RemoveGeometryIndexed2D(const string &name)
        {
            auto it = m_GeometryIndexed2D.find(name);
            if (it != m_GeometryIndexed2D.end()) {
                m_VertexBuffers2D.erase(it->second.Get());
                m_IndexBuffers.erase(it->second.Get());
                m_GeometryIndexed2D.erase(it);
            }
        }

        void GeometryStorage::RemoveGeometryIndexed3D(const string &name)
        {
            auto it = m_GeometryIndexed3D.find(name);
            if (it != m_GeometryIndexed3D.end()) {
                m_VertexBuffers3D.erase(it->second.Get());
                m_IndexBuffers.erase(it->second.Get());
                m_GeometryIndexed3D.erase(it);
            }
        }

        void GeometryStorage::RemoveMesh(const string &name)
        {
            auto it = m_Meshes.find(name);
            if (it != m_Meshes.end()) {
                m_VertexBuffers3D.erase(it->second.Get());
                m_IndexBuffers.erase(it->second.Get());
                m_Meshes.erase(it);
            }
        }

        void GeometryStorage::RemoveModel(const string &name)
        {
            auto it = m_Models.find(name);
            if (it != m_Models.end()) {
                m_VertexBuffers3D.erase(it->second.Get());
                m_IndexBuffers.erase(it->second.Get());
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

            m_VertexBuffers2D.clear();
            m_VertexBuffers3D.clear();
            m_IndexBuffers.clear();
        }

    }

}