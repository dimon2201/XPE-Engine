#include <material_loader.h>
#include <texture_loader.h>

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
                string materialPath = GetMaterialPath(assimpMaterial, aiTextureType_BASE_COLOR, directory).c_str();
                Ref<Material> material = m_Storage->Add(materialPath, Material());

                int w, h, c;
                material->Albedo = TextureLoader::LoadLayer(
                        materialPath.c_str(),
                        render::Material::K_ALBEDO_FORMAT.Format,
                        w, h, c
                );
                material->EnableAlbedo = material->Albedo.Pixels != nullptr;

                material->Bumping = TextureLoader::LoadLayer(
                        GetMaterialPath(assimpMaterial, aiTextureType_NORMALS, directory).c_str(),
                        render::Material::K_BUMP_FORMAT.Format,
                        w, h, c
                );
                material->EnableBumping = material->Bumping.Pixels != nullptr;

                material->Parallax = TextureLoader::LoadLayer(
                        GetMaterialPath(assimpMaterial, aiTextureType_DISPLACEMENT, directory).c_str(),
                        render::Material::K_PARALLAX_FORMAT.Format,
                        w, h, c
                );
                material->EnableParallax = material->Parallax.Pixels != nullptr;

                material->Metallic = TextureLoader::LoadLayer(
                        GetMaterialPath(assimpMaterial, aiTextureType_METALNESS, directory).c_str(),
                        render::Material::K_METALLIC_FORMAT.Format,
                        w, h, c
                );
                material->EnableMetallic = material->Metallic.Pixels != nullptr;

                material->Roughness = TextureLoader::LoadLayer(
                        GetMaterialPath(assimpMaterial, aiTextureType_DIFFUSE_ROUGHNESS, directory).c_str(),
                        render::Material::K_ROUGHNESS_FORMAT.Format,
                        w, h, c
                );
                material->EnableRoughness = material->Roughness.Pixels != nullptr;

                material->AO = TextureLoader::LoadLayer(
                        GetMaterialPath(assimpMaterial, aiTextureType_AMBIENT_OCCLUSION, directory).c_str(),
                        render::Material::K_AO_FORMAT.Format,
                        w, h, c
                );
                material->EnableAO = material->AO.Pixels != nullptr;

                material->Emission = TextureLoader::LoadLayer(
                        GetMaterialPath(assimpMaterial, aiTextureType_EMISSION_COLOR, directory).c_str(),
                        render::Material::K_EMISSION_FORMAT.Format,
                        w, h, c
                );
                material->EnableEmission = material->Emission.Pixels != nullptr;

                m_Storage->Set(materialPath, *material);

                return material;
            }

            return {};
        }

    }

}