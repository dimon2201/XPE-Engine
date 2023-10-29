#include <rendering/geometry/geometry_manager.h>

namespace xpe {

    namespace render {

        VertexBuffer* GeometryManager::s_VertexBuffer = nullptr;
        IndexBuffer* GeometryManager::s_IndexBuffer = nullptr;

        void GeometryManager::Init()
        {
            s_VertexBuffer = new VertexBuffer();
            s_IndexBuffer = new IndexBuffer();
        }

        void GeometryManager::Free()
        {
            delete s_VertexBuffer;
            delete s_IndexBuffer;
        }

        void GeometryManager::Bind()
        {
            context::BindVertexBuffer(*s_VertexBuffer);
            context::BindIndexBuffer(*s_IndexBuffer);
        }

        usize GeometryManager::AddIndices(const vector<u32>& indices)
        {
            usize indexOffset = s_IndexBuffer->AddIndices(indices);
            s_IndexBuffer->Flush();
            return indexOffset;
        }

        Geometry GeometryManager::AddGeometry(const Geometry& _geometry)
        {
            Geometry geometry(_geometry);

            geometry.IndexOffset = AddIndices(geometry.Indices);
            geometry.VertexOffset = s_VertexBuffer->AddVertices(geometry.Vertices);
            s_VertexBuffer->Flush();

            return geometry;
        }

        Model GeometryManager::AddModel(const Model& _model)
        {
            Model model(_model);

            model.VertexOffset = s_VertexBuffer->AddVertices(model.Meshes[0].Vertices);
            for (auto& index : model.Meshes[0].Indices)
            {
                model.Indices.emplace_back(index + model.VertexCount);
            }
            model.VertexCount += model.Meshes[0].Vertices.size();

            for (u32 i = 1 ; i < model.Meshes.size() ; i++)
            {
                auto& mesh = model.Meshes[i];

                for (auto& index : mesh.Indices)
                {
                    model.Indices.emplace_back(index + model.VertexCount);
                }
                model.VertexCount += mesh.Vertices.size();

                s_VertexBuffer->AddVertices(mesh.Vertices);
            }

            model.IndexOffset = s_IndexBuffer->AddIndices(model.Indices);

            s_VertexBuffer->Flush();
            s_IndexBuffer->Flush();

            return model;
        }

    }

}