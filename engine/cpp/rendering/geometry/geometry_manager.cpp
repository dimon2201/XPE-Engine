#include <rendering/geometry/geometry_manager.h>

namespace xpe {

    namespace render {

        sVertexBuffer* cGeometryManager::s_VertexBuffer = nullptr;
        sIndexBuffer* cGeometryManager::s_IndexBuffer = nullptr;

        void cGeometryManager::Init()
        {
            s_VertexBuffer = new sVertexBuffer();
            s_IndexBuffer = new sIndexBuffer();
        }

        void cGeometryManager::Free()
        {
            delete s_VertexBuffer;
            delete s_IndexBuffer;
        }

        void cGeometryManager::Bind()
        {
            context::BindVertexBuffer(*s_VertexBuffer);
            context::BindIndexBuffer(*s_IndexBuffer);
        }

        usize cGeometryManager::AddIndices(const vector<u32>& indices)
        {
            usize indexOffset = s_IndexBuffer->AddIndices(indices);
            s_IndexBuffer->Flush();
            return indexOffset;
        }

        sGeometry cGeometryManager::AddGeometry(const sGeometry& _geometry)
        {
            sGeometry geometry(_geometry);

            geometry.IndexOffset = AddIndices(geometry.Indices);
            geometry.VertexOffset = s_VertexBuffer->AddVertices(geometry.Vertices);
            s_VertexBuffer->Flush();

            return geometry;
        }

        sModel cGeometryManager::AddModel(const sModel& _model)
        {
            sModel model(_model);

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