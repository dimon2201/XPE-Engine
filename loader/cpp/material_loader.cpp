#include <material_loader.h>
#include <texture_loader.h>
#include <assimp_manager.h>

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

        Material MaterialLoader::Load(const aiScene* scene, aiMesh* mesh, const hstring& directory)
        {
            if (mesh->mMaterialIndex >= 0) {
                aiMaterial* assimpMaterial = scene->mMaterials[mesh->mMaterialIndex];
                MaterialFilepath materialFilepath;

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

        Material MaterialLoader::Load(const MaterialFilepath &filepath)
        {
            Material material;
            int w, h, c;

            material.AlbedoMap = TextureLoader::LoadLayer(
                    filepath.AlbedoFilepath.c_str(),
                    render::Material::K_ALBEDO_FORMAT.Format,
                    w, h, c
            );
            material.EnableAlbedoMap = material.AlbedoMap.Pixels != nullptr;

            material.NormalMap = TextureLoader::LoadLayer(
                    filepath.BumpFilepath.c_str(),
                    render::Material::K_BUMP_FORMAT.Format,
                    w, h, c
            );
            material.EnableNormalMap = material.NormalMap.Pixels != nullptr;

            material.ParallaxMap = TextureLoader::LoadLayer(
                    filepath.ParallaxFilepath.c_str(),
                    render::Material::K_PARALLAX_FORMAT.Format,
                    w, h, c
            );
            material.EnableParallaxMap = material.ParallaxMap.Pixels != nullptr;

            material.MetalMap = TextureLoader::LoadLayer(
                    filepath.MetallicFilepath.c_str(),
                    render::Material::K_METALLIC_FORMAT.Format,
                    w, h, c
            );
            material.EnableMetalMap = material.MetalMap.Pixels != nullptr;

            material.RoughnessMap = TextureLoader::LoadLayer(
                    filepath.RoughnessFilepath.c_str(),
                    render::Material::K_ROUGHNESS_FORMAT.Format,
                    w, h, c
            );
            material.EnableRoughnessMap = material.RoughnessMap.Pixels != nullptr;

            material.AOMap = TextureLoader::LoadLayer(
                    filepath.AOFilepath.c_str(),
                    render::Material::K_AO_FORMAT.Format,
                    w, h, c
            );
            material.EnableAOMap = material.AOMap.Pixels != nullptr;

            material.EmissionMap = TextureLoader::LoadLayer(
                    filepath.EmissionFilepath.c_str(),
                    render::Material::K_EMISSION_FORMAT.Format,
                    w, h, c
            );
            material.EnableEmissionMap = material.EmissionMap.Pixels != nullptr;

            return MaterialManager::AddMaterial(material);
        }

    }

}