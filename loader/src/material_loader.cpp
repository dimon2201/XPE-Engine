#include <material_loader.hpp>
#include <texture_loader.hpp>
#include <assimp_manager.hpp>

namespace xpe {

    namespace res {

        static hstring GetMaterialPath(
                aiMaterial* material,
                aiTextureType type,
                const hstring& directory
        ) {
            aiString textureFile;
            material->Get(AI_MATKEY_TEXTURE(type, 0), textureFile);
            hstringstream ss;
            ss << directory << "/" << textureFile.data;
            return ss.str();
        }

        sMaterial cMaterialLoader::Load(const aiScene* scene, aiMesh* mesh, const hstring& directory)
        {
            if (mesh->mMaterialIndex >= 0) {
                aiMaterial* assimpMaterial = scene->mMaterials[mesh->mMaterialIndex];
                sMaterialFilepath materialFilepath;

                materialFilepath.Name = directory;
                materialFilepath.AlbedoFilepath = GetMaterialPath(assimpMaterial, aiTextureType_BASE_COLOR, directory);
                materialFilepath.BumpFilepath = GetMaterialPath(assimpMaterial, aiTextureType_NORMALS, directory);
                materialFilepath.ParallaxFilepath = GetMaterialPath(assimpMaterial, aiTextureType_DISPLACEMENT, directory);
                materialFilepath.MetallicFilepath = GetMaterialPath(assimpMaterial, aiTextureType_METALNESS, directory);
                materialFilepath.RoughnessFilepath = GetMaterialPath(assimpMaterial, aiTextureType_DIFFUSE_ROUGHNESS, directory);
                materialFilepath.AOFilepath = GetMaterialPath(assimpMaterial, aiTextureType_AMBIENT_OCCLUSION, directory);
                materialFilepath.EmissionFilepath = GetMaterialPath(assimpMaterial, aiTextureType_EMISSION_COLOR, directory);

                return Load(materialFilepath);
            }

            return {};
        }

        sMaterial cMaterialLoader::Load(const sMaterialFilepath &filepath)
        {
            sMaterial material;
            int w, h, c;

            material.AlbedoMap = cTextureLoader::LoadLayer(
                    filepath.AlbedoFilepath.c_str(),
                    render::sMaterial::k_AlbedoFormat.Format,
                    w, h, c
            );
            material.EnableAlbedoMap = material.AlbedoMap->Pixels != nullptr;

            material.NormalMap = cTextureLoader::LoadLayer(
                    filepath.BumpFilepath.c_str(),
                    render::sMaterial::k_NormalFormat.Format,
                    w, h, c
            );
            material.EnableNormalMap = material.NormalMap->Pixels != nullptr;

            material.ParallaxMap = cTextureLoader::LoadLayer(
                    filepath.ParallaxFilepath.c_str(),
                    render::sMaterial::k_ParallaxFormat.Format,
                    w, h, c
            );
            material.EnableParallaxMap = material.ParallaxMap->Pixels != nullptr;

            material.MetalMap = cTextureLoader::LoadLayer(
                    filepath.MetallicFilepath.c_str(),
                    render::sMaterial::k_MetalFormat.Format,
                    w, h, c
            );
            material.EnableMetalMap = material.MetalMap->Pixels != nullptr;

            material.RoughnessMap = cTextureLoader::LoadLayer(
                    filepath.RoughnessFilepath.c_str(),
                    render::sMaterial::k_RoughnessFormat.Format,
                    w, h, c
            );
            material.EnableRoughnessMap = material.RoughnessMap->Pixels != nullptr;

            material.AOMap = cTextureLoader::LoadLayer(
                    filepath.AOFilepath.c_str(),
                    render::sMaterial::k_AoFormat.Format,
                    w, h, c
            );
            material.EnableAOMap = material.AOMap->Pixels != nullptr;

            material.EmissionMap = cTextureLoader::LoadLayer(
                    filepath.EmissionFilepath.c_str(),
                    render::sMaterial::k_EmissionFormat.Format,
                    w, h, c
            );
            material.EnableEmissionMap = material.EmissionMap->Pixels != nullptr;

            return cMaterialManager::AddMaterial(material);
        }

    }

}