#include <rendering/materials/material.h>
#include <rendering/core/shader.h>

namespace xpe {

    namespace render {

        MaterialStorage* MaterialManager::s_Storage = nullptr;

        MaterialStorage::MaterialStorage(usize count)
        {
            Buffer = MaterialBuffer(0);
            Buffer.Reserve(count);
            InitMaterialTextures();
        }

        void MaterialStorage::AddMaterial(const string &name, const Material &material)
        {
            Table[name] = material;
        }

        void MaterialStorage::RemoveMaterial(const string &name)
        {
            auto it = Table.find(name);
            if (it != Table.end()) {
                Table.erase(it);
            }
        }

        Material* MaterialStorage::GetMaterial(const string &name)
        {
            return &Table[name];
        }

        void MaterialStorage::InitMaterialTextures()
        {
            InitMaterialSampler();
            InitTextureArray(Textures.AlbedoArray, Texture::eFormat::RGBA8, 512, 512, K_SLOT_ALBEDO);
            InitTextureArray(Textures.BumpArray, Texture::eFormat::RGB8, 512, 512, K_SLOT_BUMPING);
            InitTextureArray(Textures.ParallaxArray, Texture::eFormat::R32, 512, 512, K_SLOT_PARALLAX);
            InitTextureArray(Textures.MetallicArray, Texture::eFormat::R32, 512, 512, K_SLOT_METALLIC);
            InitTextureArray(Textures.RoughnessArray, Texture::eFormat::R32, 512, 512, K_SLOT_ROUGHNESS);
            InitTextureArray(Textures.AOArray, Texture::eFormat::R32, 512, 512, K_SLOT_AO);
            InitTextureArray(Textures.EmissionArray, Texture::eFormat::RGB8, 512, 512, K_SLOT_EMISSION);
        }

        void MaterialStorage::InitMaterialSampler() {
            TextureSampler& materialSampler = Textures.Sampler;
            materialSampler.Filter          = TextureSampler::eFilter::ANISOTROPIC;
            materialSampler.AnisotropyLevel = HardwareConfig::K_ANISOTROPY_LEVEL_MAX;
            materialSampler.AddressU        = TextureSampler::eAddress::WRAP;
            materialSampler.AddressV        = TextureSampler::eAddress::WRAP;
            materialSampler.AddressW        = TextureSampler::eAddress::WRAP;

            context::CreateSampler(Textures.Sampler);
        }

        void MaterialStorage::FreeMaterialTextures()
        {
            auto& textures = Textures;

            Buffer.Free();

            context::FreeSampler(&textures.Sampler);

            context::FreeTexture(textures.AlbedoArray);
            context::FreeTexture(textures.BumpArray);
            context::FreeTexture(textures.ParallaxArray);
            context::FreeTexture(textures.MetallicArray);
            context::FreeTexture(textures.RoughnessArray);
            context::FreeTexture(textures.AOArray);
            context::FreeTexture(textures.EmissionArray);
        }

        void MaterialStorage::InitTextureArray(Texture &textureArray, const Texture::eFormat& format, usize width, usize height, u32 slot)
        {
            textureArray.InitializeData = true;
            textureArray.Type = Texture::eType::TEXTURE_2D_ARRAY;
            textureArray.Usage = Texture::eUsage::DEFAULT;
            textureArray.Format = format;
            textureArray.Width = width;
            textureArray.Height = height;
            textureArray.Slot = slot;
            textureArray.Channels = TextureManager::ChannelTable.at(format);
            textureArray.Layers.reserve(HardwareConfig::K_TEXTURE_2D_ARRAY_SIZE_MAX);
//            for (auto& layer : textureArray.Layers) {
//                layer.Pixels = MemoryPoolManager::Allocate(textureArray.WinWidth * textureArray.WinHeight * textureArray.Channels);
//            }
        }

        MaterialStorage::~MaterialStorage()
        {
            Table.clear();
            FreeMaterialTextures();
        }

        void MaterialManager::Init()
        {
            LogInfo("MaterialManager::Init()");
            s_Storage = new MaterialStorage(K_MATERIALS_COUNT);
            LogInfo("MaterialManager initialized");
        }

        void MaterialManager::Free()
        {
            LogInfo("MaterialManager::Free()");
            delete s_Storage;
        }

        Material* MaterialManager::CreateMaterial(const string& usid)
        {
            Material material;
            CreateMaterial(material);
            s_Storage->AddMaterial(usid, material);
            return s_Storage->GetMaterial(usid);
        }

        void MaterialManager::CreateMaterial(Material &material)
        {
            material.Index = s_Storage->Buffer.Add();
            material.Data = s_Storage->Buffer[material.Index];
            material.Textures = &s_Storage->Textures;
        }

        u32 MaterialManager::AddMaterial(const string& usid, const Material &material)
        {
            s_Storage->AddMaterial(usid, material);
            return s_Storage->Buffer.Add(*material.Data);
        }

        void MaterialManager::RemoveMaterial(const string& usid)
        {
            auto* material = s_Storage->GetMaterial(usid);
            if (material != nullptr) {
                s_Storage->RemoveMaterial(usid);
                s_Storage->Buffer.RemoveAt(material->Index);
            }
        }

        void MaterialManager::FlushMaterial(Material &material)
        {
            s_Storage->Buffer.FlushItem(material.Index, *material.Data);
        }

        void MaterialManager::FlushMaterials()
        {
            s_Storage->Buffer.Flush();
        }

        MaterialBuffer* MaterialManager::GetBuffer()
        {
            return &s_Storage->Buffer;
        }

        MaterialStorage* MaterialManager::GetStorage() {
            return s_Storage;
        }

        MaterialBufferData* MaterialManager::GetMaterialData(u32 index)
        {
            return s_Storage->Buffer[index];
        }

        void MaterialManager::AddAlbedoFromFile(Material& material, const char *filepath)
        {
            material.Albedo = AddTextureFromFile(filepath, material.Textures->AlbedoArray, material.AlbedoIndex);
        }

        void MaterialManager::AddBumpFromFile(Material& material, const char *filepath)
        {
            material.Bumping = AddTextureFromFile(filepath, material.Textures->BumpArray, material.BumpingIndex);
        }

        void MaterialManager::AddParallaxFromFile(Material& material, const char *filepath)
        {
            material.Parallax = AddTextureFromFile(filepath, material.Textures->ParallaxArray, material.ParallaxIndex);
        }

        void MaterialManager::AddMetallicFromFile(Material& material, const char *filepath)
        {
            material.Metallic = AddTextureFromFile(filepath, material.Textures->MetallicArray, material.MetallicIndex);
        }

        void MaterialManager::AddRoughnessFromFile(Material& material, const char *filepath)
        {
            material.Roughness = AddTextureFromFile(filepath, material.Textures->RoughnessArray, material.RoughnessIndex);
        }

        void MaterialManager::AddAOFromFile(Material& material, const char *filepath)
        {
            material.AO = AddTextureFromFile(filepath, material.Textures->AOArray, material.AOIndex);
        }

        void MaterialManager::AddEmissionFromFile(Material& material, const char *filepath)
        {
            material.Emission = AddTextureFromFile(filepath, material.Textures->EmissionArray, material.EmissionIndex);
        }

        TextureLayer* MaterialManager::AddTextureFromFile(const char *filepath, Texture& textureArray, u32 textureLayerIndex)
        {
            Texture* texture = TextureManager::ReadTextureFile(filepath, textureArray.Format);
            TextureLayer& textureLayer = texture->Layers.front();

            textureArray.Width = texture->Width;
            textureArray.Height = texture->Height;

            if (textureLayer.Mips.empty()) {
                TextureManager::GenerateMips(textureLayer, textureArray.Width, textureArray.Height, textureArray.Format);
            }

            if (textureLayerIndex >= textureArray.Layers.size()) {
                textureArray.Layers.resize(textureLayerIndex + 1);
                textureArray.Layers[textureLayerIndex] = textureLayer;
                context::FreeTexture(textureArray);
                context::CreateTexture(textureArray);
            }
            textureArray.Layers[textureLayerIndex] = textureLayer;

//            TextureManager::WriteTexture(textureArray, textureLayerIndex);

            return &textureLayer;
        }

    }

}