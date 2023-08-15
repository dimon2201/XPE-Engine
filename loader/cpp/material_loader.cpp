#include <material_loader.h>
#include <texture_loader.h>

#include <rendering/storages/material_storage.h>

#include <assimp/Importer.hpp>
#include <assimp/Exporter.hpp>
#include <assimp/scene.h>

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

        Ref<Material> MaterialLoader::Load(const aiScene* scene, aiMesh* mesh, const hstring& directory, u32 flags)
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

        Ref<Material> MaterialLoader::Load(const MaterialFilepath &filepath)
        {
            Ref<Material> material = m_Storage->Add(filepath.Name, Material());

            int w, h, c;
            material->Albedo = TextureLoader::LoadLayer(
                    filepath.AlbedoFilepath.c_str(),
                    render::Material::K_ALBEDO_FORMAT.Format,
                    w, h, c
            );
            material->EnableAlbedo = material->Albedo.Pixels != nullptr;

            material->Bumping = TextureLoader::LoadLayer(
                    filepath.BumpFilepath.c_str(),
                    render::Material::K_BUMP_FORMAT.Format,
                    w, h, c
            );
            material->EnableBumping = material->Bumping.Pixels != nullptr;

            material->Parallax = TextureLoader::LoadLayer(
                    filepath.ParallaxFilepath.c_str(),
                    render::Material::K_PARALLAX_FORMAT.Format,
                    w, h, c
            );
            material->EnableParallax = material->Parallax.Pixels != nullptr;

            material->Metallic = TextureLoader::LoadLayer(
                    filepath.MetallicFilepath.c_str(),
                    render::Material::K_METALLIC_FORMAT.Format,
                    w, h, c
            );
            material->EnableMetallic = material->Metallic.Pixels != nullptr;

            material->Roughness = TextureLoader::LoadLayer(
                    filepath.RoughnessFilepath.c_str(),
                    render::Material::K_ROUGHNESS_FORMAT.Format,
                    w, h, c
            );
            material->EnableRoughness = material->Roughness.Pixels != nullptr;

            material->AO = TextureLoader::LoadLayer(
                    filepath.AOFilepath.c_str(),
                    render::Material::K_AO_FORMAT.Format,
                    w, h, c
            );
            material->EnableAO = material->AO.Pixels != nullptr;

            material->Emission = TextureLoader::LoadLayer(
                    filepath.EmissionFilepath.c_str(),
                    render::Material::K_EMISSION_FORMAT.Format,
                    w, h, c
            );
            material->EnableEmission = material->Emission.Pixels != nullptr;

            m_Storage->Set(filepath.Name, *material);

            return material;
        }

    }

}