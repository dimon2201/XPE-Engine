#include <anim/storages/skin_storage.h>

namespace xpe {

    namespace anim {

        Ref<Skin> SkinStorage::AddSkin(const string &name, const Skin &skin)
        {
            Ref<Skin> meshRef;
            meshRef.Create(skin);
            m_Skins.insert({name, meshRef });

            Ref<VertexBuffer<SkeletalVertex>> vertexBuffer;
            vertexBuffer.Create(skin.Vertices);
            m_VertexBuffers.insert({ meshRef.Get(), vertexBuffer });

            Ref<IndexBuffer> indexBuffer;
            indexBuffer.Create(skin.Indices);
            m_IndexBuffers.insert({ meshRef.Get(), indexBuffer });

            return meshRef;
        }

        Ref<SkinModel> SkinStorage::AddModel(const string &name, const SkinModel &model)
        {
            Ref<SkinModel> newModel;
            newModel.Create(model);
            m_Models.insert({ name, newModel });

            VertexArray<SkeletalVertex> vertexArray;
            vertexArray.Reserve(100000);
            IndexArray indexArray;
            indexArray.Reserve(100000);

            u32 modelVertexCount = 0;
            for (auto& mesh : model.Skins)
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

            Ref<VertexBuffer<SkeletalVertex>> vertexBuffer;
            vertexBuffer.Create(vertexArray);
            m_VertexBuffers.insert({ newModel.Get(), vertexBuffer });

            Ref<IndexBuffer> indexBuffer;
            indexBuffer.Create(indexArray);
            m_IndexBuffers.insert({ newModel.Get(), indexBuffer });

            return newModel;
        }

        void SkinStorage::RemoveSkin(const string &name)
        {
            auto it = m_Skins.find(name);
            if (it != m_Skins.end()) {
                m_VertexBuffers.erase(it->second.Get());
                m_IndexBuffers.erase(it->second.Get());
                m_Skins.erase(it);
            }
        }

        void SkinStorage::RemoveModel(const string &name)
        {
            auto it = m_Models.find(name);
            if (it != m_Models.end()) {
                m_VertexBuffers.erase(it->second.Get());
                m_IndexBuffers.erase(it->second.Get());
                m_Models.erase(it);
            }
        }

        void SkinStorage::Clear()
        {
            m_Skins.clear();
            m_Models.clear();

            m_VertexBuffers.clear();
            m_IndexBuffers.clear();
        }

    }

}