#include "model_loader.h"

#include <assimp/Importer.hpp>
#include <assimp/Exporter.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace xpe {

    namespace res {

        static u32 LoadMaterial(
                unordered_map<u32, MaterialResource>& materials,
                const aiScene* scene, aiMesh* mesh,
                const string& directory, u32 flags
        ) {
            return 0;
        }

        ModelResource ModelLoader::Load(const char* filepath, const vector <ModelLoader::eOption> &options)
        {
            return {};
        }

    }

}