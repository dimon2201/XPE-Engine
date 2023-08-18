#include <anim/storages/skin_storage.h>

namespace xpe {

    namespace anim {

        Ref<Skin> SkinStorage::AddSkin(const string &name, const Skin &skin)
        {
            Ref<Skin> meshRef;
            meshRef.Create(skin);
            meshRef->Vertices.Flush();
            meshRef->Indices.Flush();
            m_Skins.insert({ name, meshRef });
            return meshRef;
        }

        Ref<SkinModel> SkinStorage::AddModel(const string &name, const SkinModel &model)
        {
            Ref<SkinModel> newModel;
            newModel.Create(model);
            newModel->Vertices.Reserve(100000);
            newModel->Indices.Reserve(100000);
            m_Models.insert({ name, newModel });

            u32 modelVertexCount = 0;
            for (auto& mesh : model.Skins)
            {
                for (auto& vertex : mesh.Vertices.List)
                {
                    newModel->Vertices.List.emplace_back(vertex);
                }

                for (auto& index : mesh.Indices.List)
                {
                    newModel->Indices.List.emplace_back(index + modelVertexCount);
                }

                modelVertexCount += mesh.Vertices.List.size();
            }

            newModel->Vertices.Flush();
            newModel->Indices.Flush();

            return newModel;
        }

        void SkinStorage::RemoveSkin(const string &name)
        {
            auto it = m_Skins.find(name);
            if (it != m_Skins.end()) {
                m_Skins.erase(it);
            }
        }

        void SkinStorage::RemoveModel(const string &name)
        {
            auto it = m_Models.find(name);
            if (it != m_Models.end()) {
                m_Models.erase(it);
            }
        }

        void SkinStorage::Clear()
        {
            m_Skins.clear();
            m_Models.clear();
        }

    }

}