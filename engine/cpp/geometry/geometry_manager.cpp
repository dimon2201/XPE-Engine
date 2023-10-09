#include <geometry/geometry_manager.h>

namespace xpe {

    namespace render {

        VertexBuffer<Vertex2D>* GeometryManager::s_VertexBuffer2D = nullptr;
        VertexBuffer<Vertex3D>* GeometryManager::s_VertexBuffer3D = nullptr;
        VertexBuffer<VertexSkeletal>* GeometryManager::s_VertexBufferSkeletal = nullptr;
        IndexBuffer* GeometryManager::s_IndexBuffer = nullptr;

        void GeometryManager::Init()
        {
            s_VertexBuffer2D = new VertexBuffer<Vertex2D>();
            s_VertexBuffer3D = new VertexBuffer<Vertex3D>();
            s_VertexBufferSkeletal = new VertexBuffer<VertexSkeletal>();
            s_IndexBuffer = new IndexBuffer();
        }

        void GeometryManager::Free()
        {
            delete s_VertexBuffer2D;
            delete s_VertexBuffer3D;
            delete s_VertexBufferSkeletal;
            delete s_IndexBuffer;
        }

        void GeometryManager::BindVertexBuffer2D()
        {
            context::BindVertexBuffer(*s_VertexBuffer2D);
        }

        void GeometryManager::BindVertexBuffer3D()
        {
            context::BindVertexBuffer(*s_VertexBuffer3D);
        }

        void GeometryManager::BindVertexBufferSkeletal()
        {
            context::BindVertexBuffer(*s_VertexBufferSkeletal);
        }

        void GeometryManager::BindIndexBuffer()
        {
            context::BindIndexBuffer(*s_IndexBuffer);
        }

        Ref<Model3D> GeometryManager::AddModel(const Model3D &model)
        {
            if (model.Meshes.empty()) {
                return { nullptr };
            }

            Ref<Model3D> modelRef;
            modelRef.Create(model);

            modelRef->VertexOffset = s_VertexBuffer3D->AddVertices(modelRef->Meshes[0].Vertices);
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

                s_VertexBuffer3D->AddVertices(mesh.Vertices);
            }

            modelRef->IndexOffset = s_IndexBuffer->AddIndices(modelRef->Indices);

            s_VertexBuffer3D->Flush();
            s_IndexBuffer->Flush();

            return modelRef;
        }

        Ref<SkinModel> GeometryManager::AddSkinModel(const SkinModel &skinModel)
        {
            if (skinModel.Skins.empty()) {
                return { nullptr };
            }

            Ref<SkinModel> skinModelRef;
            skinModelRef.Create(skinModel);

            skinModelRef->VertexOffset = s_VertexBufferSkeletal->AddVertices(skinModelRef->Skins[0].Vertices);
            for (auto& index : skinModel.Skins[0].Indices)
            {
                skinModelRef->Indices.emplace_back(index + skinModelRef->VertexCount);
            }
            skinModelRef->VertexCount += skinModelRef->Skins[0].Vertices.size();

            for (u32 i = 1 ; i < skinModelRef->Skins.size() ; i++)
            {
                auto& mesh = skinModelRef->Skins[i];

                for (auto& index : mesh.Indices)
                {
                    skinModelRef->Indices.emplace_back(index + skinModelRef->VertexCount);
                }
                skinModelRef->VertexCount += mesh.Vertices.size();

                s_VertexBufferSkeletal->AddVertices(mesh.Vertices);
            }

            skinModelRef->IndexOffset = s_IndexBuffer->AddIndices(skinModelRef->Indices);

            s_VertexBufferSkeletal->Flush();
            s_IndexBuffer->Flush();

            return skinModelRef;
        }

        usize GeometryManager::AddIndices(const vector<u32>& indices)
        {
            usize indexOffset = s_IndexBuffer->AddIndices(indices);
            s_IndexBuffer->Flush();
            return indexOffset;
        }

    }

}