#pragma once

#include <build.h>

#include <rendering/storages/material_storage.h>

#include <assimp/Importer.hpp>
#include <assimp/Exporter.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace xpe {

    namespace res {

        using namespace core;
        using namespace render;

        class LOADER_API MaterialLoader : public Object
        {

        public:
            MaterialLoader(MaterialStorage* materialStorage) : m_Storage(materialStorage) {}
            Ref<Material> Load(const aiScene* scene, aiMesh* mesh, const hstring& directory, u32 flags);

        private:
            render::MaterialStorage* m_Storage = nullptr;
        };

    }

}