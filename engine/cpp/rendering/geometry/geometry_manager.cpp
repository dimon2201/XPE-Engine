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

        void GeometryManager::BindVertexBuffer()
        {
            context::BindVertexBuffer(*s_VertexBuffer);
        }

        void GeometryManager::BindIndexBuffer()
        {
            context::BindIndexBuffer(*s_IndexBuffer);
        }

        Ref<Model> GeometryManager::AddModel(const Model &model)
        {
            if (model.Meshes.empty()) {
                return { nullptr };
            }

            Ref<Model> modelRef;
            modelRef.Create(model);

            modelRef->VertexOffset = s_VertexBuffer->AddVertices(modelRef->Meshes[0].Vertices);
            for (auto& index : modelRef->Meshes[0].Indices)
            {
                modelRef->Indices.emplace_back(index + modelRef->VertexCount);
            }
            modelRef->VertexCount += modelRef->Meshes[0].Vertices.size();

            for (u32 i = 1 ; i < modelRef->Meshes.size() ; i++)
            {
                auto& mesh = modelRef->Meshes[i];

                for (auto& index : mesh.Indices)
                {
                    modelRef->Indices.emplace_back(index + modelRef->VertexCount);
                }
                modelRef->VertexCount += mesh.Vertices.size();

                s_VertexBuffer->AddVertices(mesh.Vertices);
            }

            modelRef->IndexOffset = s_IndexBuffer->AddIndices(modelRef->Indices);

            s_VertexBuffer->Flush();
            s_IndexBuffer->Flush();

            return modelRef;
        }

        usize GeometryManager::AddIndices(const vector<u32>& indices)
        {
            usize indexOffset = s_IndexBuffer->AddIndices(indices);
            s_IndexBuffer->Flush();
            return indexOffset;
        }

    }

}