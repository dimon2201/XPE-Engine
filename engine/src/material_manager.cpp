#include <material_manager.hpp>
#include <texture_manager.hpp>
#include <components.hpp>
#include <render_manager.hpp>

namespace xpe
{
    namespace render
    {
        void MMaterial::Init()
        {
            Buffers::Material = new sMaterialDataBuffer();
            InitSampler();
            Textures::AlbedoAtlas = InitTextureArray(CMaterial::k_AlbedoFormat);
            Textures::NormalAtlas = InitTextureArray(CMaterial::k_NormalFormat);
            Textures::ParallaxAtlas = InitTextureArray(CMaterial::k_ParallaxFormat);
            Textures::MetalAtlas = InitTextureArray(CMaterial::k_MetalFormat);
            Textures::RoughnessAtlas = InitTextureArray(CMaterial::k_RoughnessFormat);
            Textures::AOAtlas = InitTextureArray(CMaterial::k_AoFormat);
            Textures::EmissionAtlas = InitTextureArray(CMaterial::k_EmissionFormat);
        }

        void MMaterial::Free()
        {
            delete Buffers::Material;
            delete Textures::AlbedoAtlas;
            delete Textures::NormalAtlas;
            delete Textures::ParallaxAtlas;
            delete Textures::MetalAtlas;
            delete Textures::RoughnessAtlas;
            delete Textures::AOAtlas;
            delete Textures::EmissionAtlas;
            delete Samplers::Material;
        }

        void MMaterial::InitSampler()
        {
            Samplers::Material                  = new cSampler();
            Samplers::Material->Slot            = K_SLOT_MATERIAL_SAMPLER;
            Samplers::Material->Filter          = cSampler::eFilter::ANISOTROPIC;
            Samplers::Material->AnisotropyLevel = MRender::AnisotropyLevel;
            Samplers::Material->AddressU        = cSampler::eAddress::WRAP;
            Samplers::Material->AddressV        = cSampler::eAddress::WRAP;
            Samplers::Material->AddressW        = cSampler::eAddress::WRAP;
            Samplers::Material->Init();
        }

        cTexture* MMaterial::InitTextureArray(const sMaterialFormat& materialFormat)
        {
            cTexture* texture = new cTexture();
            texture->InitializeData = true;
            texture->Type = cTexture::eType::TEXTURE_2D_ARRAY;
            texture->Usage = cTexture::eUsage::DEFAULT;
            texture->Format = materialFormat.Format;
            texture->Width = materialFormat.Width;
            texture->Height = materialFormat.Height;
            texture->Slot = materialFormat.Slot;
            texture->Channels = cTexture::k_ChannelTable.at(materialFormat.Format);
            texture->Layers.reserve(MHardware::GpuInfo.MaxTexture2dArray);
            texture->EnableMipmapping = true;
            return texture;
        }

        void MMaterial::Clear()
        {
            Buffers::Material->Clear();
            Buffers::Material->Flush();

            Textures::AlbedoAtlas->Layers.clear();
            Textures::AlbedoAtlas->Flush();

            Textures::NormalAtlas->Layers.clear();
            Textures::NormalAtlas->Flush();

            Textures::ParallaxAtlas->Layers.clear();
            Textures::ParallaxAtlas->Flush();

            Textures::MetalAtlas->Layers.clear();
            Textures::MetalAtlas->Flush();

            Textures::RoughnessAtlas->Layers.clear();
            Textures::RoughnessAtlas->Flush();

            Textures::AOAtlas->Layers.clear();
            Textures::AOAtlas->Flush();

            Textures::EmissionAtlas->Layers.clear();
            Textures::EmissionAtlas->Flush();
        }

        void MMaterial::AddLayer(cTexture &texture, cTextureLayer* layer)
        {
            if (layer == nullptr || layer->Pixels == nullptr)
            {
                texture.Layers.emplace_back(texture.CreateLayer());
            }
            else
            {
                layer->Resize(texture.Format, texture.Width, texture.Height);
                texture.Layers.emplace_back(*layer);
            }
            texture.Flush();
        }

        void MMaterial::SetLayer(cTexture &texture, cTextureLayer* layer, u32 layerIndex)
        {
            if (layer == nullptr || layer->Pixels == nullptr)
            {
                texture.Layers[layerIndex] = texture.CreateLayer();
            }
            else
            {
                texture.Layers[layerIndex] = *layer;
            }
            texture.Flush();
        }

        static hstring GetMaterialPath(
                aiMaterial* material,
                aiTextureType type,
                const hstring& directory
        )
        {
            aiString textureFile;
            material->Get(AI_MATKEY_TEXTURE(type, 0), textureFile);
            hstringstream ss;
            ss << directory << "/" << textureFile.data;
            return ss.str();
        }

        CMaterialInfo MMaterial::Add(const CMaterial &material)
        {
            CMaterialInfo materialInfo;
            materialInfo.MaterialIndex = Buffers::Material->Size();
            Buffers::Material->Add(material);
            AddLayer(*Textures::AlbedoAtlas, material.AlbedoMap);
            AddLayer(*Textures::NormalAtlas, material.NormalMap);
            AddLayer(*Textures::ParallaxAtlas, material.ParallaxMap);
            AddLayer(*Textures::MetalAtlas, material.MetalMap);
            AddLayer(*Textures::RoughnessAtlas, material.RoughnessMap);
            AddLayer(*Textures::AOAtlas, material.AOMap);
            AddLayer(*Textures::EmissionAtlas, material.EmissionMap);
            Buffers::Material->Flush();
            return materialInfo;
        }

        CMaterial MMaterial::Load(const aiScene* scene, aiMesh* mesh, const hstring& directory)
        {
            if (mesh->mMaterialIndex >= 0)
            {
                aiMaterial* assimpMaterial = scene->mMaterials[mesh->mMaterialIndex];
                sMaterialFilepath materialFilepath;

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

        CMaterial MMaterial::Load(const sMaterialFilepath &filepath)
        {
            CMaterial material;
            int w, h, c;

            material.AlbedoMap = MTexture::LoadLayer(
                    filepath.AlbedoFilepath.c_str(),
                    CMaterial::k_AlbedoFormat.Format,
                    w, h, c
            );
            material.EnableAlbedoMap = material.AlbedoMap != nullptr && material.AlbedoMap->Pixels != nullptr;

            material.NormalMap = MTexture::LoadLayer(
                    filepath.BumpFilepath.c_str(),
                    CMaterial::k_NormalFormat.Format,
                    w, h, c
            );
            material.EnableNormalMap = material.NormalMap != nullptr && material.NormalMap->Pixels != nullptr;

            material.ParallaxMap = MTexture::LoadLayer(
                    filepath.ParallaxFilepath.c_str(),
                    CMaterial::k_ParallaxFormat.Format,
                    w, h, c
            );
            material.EnableParallaxMap = material.ParallaxMap != nullptr && material.ParallaxMap->Pixels != nullptr;

            material.MetalMap = MTexture::LoadLayer(
                    filepath.MetallicFilepath.c_str(),
                    CMaterial::k_MetalFormat.Format,
                    w, h, c
            );
            material.EnableMetalMap = material.MetalMap != nullptr && material.MetalMap->Pixels != nullptr;

            material.RoughnessMap = MTexture::LoadLayer(
                    filepath.RoughnessFilepath.c_str(),
                    CMaterial::k_RoughnessFormat.Format,
                    w, h, c
            );
            material.EnableRoughnessMap = material.RoughnessMap != nullptr && material.RoughnessMap->Pixels != nullptr;

            material.AOMap = MTexture::LoadLayer(
                    filepath.AOFilepath.c_str(),
                    CMaterial::k_AoFormat.Format,
                    w, h, c
            );
            material.EnableAOMap = material.AOMap != nullptr && material.AOMap->Pixels != nullptr;

            material.EmissionMap = MTexture::LoadLayer(
                    filepath.EmissionFilepath.c_str(),
                    CMaterial::k_EmissionFormat.Format,
                    w, h, c
            );
            material.EnableEmissionMap = material.EmissionMap != nullptr && material.EmissionMap->Pixels != nullptr;

            return material;
        }
    }
}