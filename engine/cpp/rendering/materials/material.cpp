#include <rendering/materials/material.h>
#include <rendering/shader.h>

namespace xpe {

    namespace render {

        Context* MaterialManager::s_Context = nullptr;
        MaterialStorage* MaterialManager::s_Storage = nullptr;

        MaterialStorage::MaterialStorage(Context *context, usize count) : m_Context(context)
        {
            Buffer = MaterialBuffer(context, count);
            InitMaterialTextures();
        }

        void MaterialStorage::Bind()
        {
            auto& textures = Textures;

            m_Context->BindSampler(&textures.Sampler);

            m_Context->BindTexture(&textures.AlbedoArray);
            m_Context->BindTexture(&textures.BumpArray);
            m_Context->BindTexture(&textures.ParallaxArray);
            m_Context->BindTexture(&textures.MetallicArray);
            m_Context->BindTexture(&textures.RoughnessArray);
            m_Context->BindTexture(&textures.AOArray);
            m_Context->BindTexture(&textures.EmissionArray);
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

            m_Context->CreateSampler(Textures.Sampler);
        }

        void MaterialStorage::FreeMaterialTextures()
        {
            auto& textures = Textures;

            Buffer.Free();

            m_Context->FreeSampler(&textures.Sampler);

            m_Context->FreeTexture(&textures.AlbedoArray);
            m_Context->FreeTexture(&textures.BumpArray);
            m_Context->FreeTexture(&textures.ParallaxArray);
            m_Context->FreeTexture(&textures.MetallicArray);
            m_Context->FreeTexture(&textures.RoughnessArray);
            m_Context->FreeTexture(&textures.AOArray);
            m_Context->FreeTexture(&textures.EmissionArray);
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

        void MaterialManager::Init(Context* context)
        {
            LogInfo("MaterialManager::Init()");

            s_Context = context;
            s_Storage = new MaterialStorage(s_Context, K_MATERIALS_COUNT);

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
            material.Textures = &s_Storage->Textures;
            material.Data = s_Storage->Buffer.GetItem(material.Index);
        }

        void MaterialManager::BindMaterials()
        {
            s_Storage->Bind();
        }

        void MaterialManager::UpdateMaterials()
        {
            s_Storage->Buffer.Flush();
        }

        void MaterialManager::UpdateMaterial(Material &material)
        {
            s_Storage->Buffer.FlushItem(material.Index, *material.Data);
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
            return s_Storage->Buffer.GetItem(index);
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
                s_Context->FreeTexture(&textureArray);
                s_Context->CreateTexture(textureArray);
            }
            textureArray.Layers[textureLayerIndex] = textureLayer;

//            TextureManager::WriteTexture(textureArray, textureLayerIndex);

            return &textureLayer;
        }

    }

}