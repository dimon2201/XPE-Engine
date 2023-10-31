#pragma once

#include <build.h>

#include <rendering/material/material_manager.h>

class aiScene;
class aiMesh;

namespace xpe {

    namespace res {

        using namespace core;
        using namespace render;

        struct LOADER_API sMaterialFilepath final
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

        class LOADER_API cMaterialLoader final
        {
        public:
            static sMaterial Load(const aiScene* scene, aiMesh* mesh, const hstring& directory);
            static sMaterial Load(const sMaterialFilepath& filepath);
        };

    }

}