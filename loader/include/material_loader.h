#pragma once

#include <build.h>
#include <rendering/storages/material_storage.h>

class aiScene;
class aiMesh;

namespace xpe {

    namespace res {

        using namespace core;
        using namespace render;

        struct LOADER_API MaterialFilepath final
        {
            string Name;
            string AlbedoFilepath;
            string BumpFilepath;
            string ParallaxFilepath;
            string MetallicFilepath;
            string RoughnessFilepath;
            string AOFilepath;
            string EmissionFilepath;
        };

        class LOADER_API MaterialLoader : public Object
        {

        public:
            MaterialLoader(MaterialStorage* materialStorage) : m_Storage(materialStorage) {}

            Ref<Material> Load(const aiScene* scene, aiMesh* mesh, const hstring& directory);

            Ref<Material> Load(const MaterialFilepath& filepath);

        private:
            MaterialStorage* m_Storage;
        };

    }

}