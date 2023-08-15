#pragma once

#include <build.h>

class aiScene;
class aiMesh;

namespace xpe {

    namespace render
    {
        class MaterialStorage;
        class Material;
    }

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

            Ref<Material> Load(const aiScene* scene, aiMesh* mesh, const hstring& directory, u32 flags);

            Ref<Material> Load(const MaterialFilepath& filepath);

        private:
            MaterialStorage* m_Storage = nullptr;
        };

    }

}