#include <rendering/materials/material.h>
#include <rendering/shader.h>

namespace xpe {

    namespace render {

        Material* MaterialBuilder::Build(const string& name) {
            MaterialManager::AddMaterial(name, *m_Material);
            return MaterialManager::GetMaterial(name);
        }

        MaterialBuilder& MaterialBuilder::AddAlbedoFromFile(const char *filepath) {
            m_Material->Albedo = AddTextureFromFile(filepath, MaterialManager::Textures().AlbedoArray);
            return *this;
        }

        MaterialBuilder& MaterialBuilder::AddBumpFromFile(const char *filepath) {
            m_Material->Bumping = AddTextureFromFile(filepath, MaterialManager::Textures().BumpArray);
            return *this;
        }

        MaterialBuilder& MaterialBuilder::AddParallaxFromFile(const char *filepath) {
            m_Material->Parallax = AddTextureFromFile(filepath, MaterialManager::Textures().ParallaxArray);
            return *this;
        }

        MaterialBuilder& MaterialBuilder::AddMetallicFromFile(const char *filepath) {
            m_Material->Metallic = AddTextureFromFile(filepath, MaterialManager::Textures().MetallicArray);
            return *this;
        }

        MaterialBuilder& MaterialBuilder::AddRoughnessFromFile(const char *filepath) {
            m_Material->Roughness = AddTextureFromFile(filepath, MaterialManager::Textures().RoughnessArray);
            return *this;
        }

        MaterialBuilder& MaterialBuilder::AddAOFromFile(const char *filepath) {
            m_Material->AO = AddTextureFromFile(filepath, MaterialManager::Textures().AOArray);
            return *this;
        }

        MaterialBuilder& MaterialBuilder::AddEmissionFromFile(const char *filepath) {
            m_Material->Emission = AddTextureFromFile(filepath, MaterialManager::Textures().EmissionArray);
            return *this;
        }

        TextureLayer* MaterialBuilder::AddTextureFromFile(const char *filepath, Texture &textureArray)
        {
            u32 materialIndex = m_Material->Index;

            Texture* texture = TextureManager::ReadTextureFile(filepath, textureArray.Format);
            TextureLayer& textureLayer = texture->Layers.front();

            textureArray.Layers[materialIndex] = textureLayer;
            TextureManager::WriteTexture(textureArray, materialIndex);

            return &textureLayer;
        }

        Context* MaterialManager::s_Context = nullptr;
        MaterialStorage* MaterialManager::s_Storage = nullptr;
        MaterialBuilder MaterialManager::s_MaterialBuilder;
        Material MaterialManager::s_TempMaterial;

        MaterialStorage::MaterialStorage(Context *context, usize count) {
            Buffer = MaterialBuffer(context, count);

            context->CreateSampler(Textures.Sampler);

            InitTextureArray(context, Textures.AlbedoArray, Texture::eFormat::RGBA8, 389, 600, K_SLOT_ALBEDO);
            InitTextureArray(context, Textures.BumpArray, Texture::eFormat::RGB8, 389, 600, K_SLOT_BUMPING);
            InitTextureArray(context, Textures.ParallaxArray, Texture::eFormat::R32, 389, 600, K_SLOT_PARALLAX);
            InitTextureArray(context, Textures.MetallicArray, Texture::eFormat::R32, 389, 600, K_SLOT_METALLIC);
            InitTextureArray(context, Textures.RoughnessArray, Texture::eFormat::R32, 389, 600, K_SLOT_ROUGHNESS);
            InitTextureArray(context, Textures.AOArray, Texture::eFormat::R32, 389, 600, K_SLOT_AO);
            InitTextureArray(context, Textures.EmissionArray, Texture::eFormat::RGB8, 389, 600, K_SLOT_EMISSION);
        }

        void MaterialStorage::InitTextureArray(Context* context, Texture &textureArray, const Texture::eFormat& format, usize width, usize height, u32 slot) {
            textureArray.InitializeData = false;
            textureArray.Type = Texture::eType::TEXTURE_2D_ARRAY;
            textureArray.Usage = Texture::eUsage::DEFAULT;
            textureArray.Format = format;
            textureArray.Width = width;
            textureArray.Height = height;
            textureArray.Slot = slot;
            textureArray.ChannelCount = TextureManager::ChannelTable.at(format);
            textureArray.Layers.resize(TextureManager::K_TEXTURE_ARRAY_SIZE);
//            for (auto& layer : textureArray.Layers) {
//                layer.Pixels = MemoryPoolManager::Allocate(textureArray.WinWidth * textureArray.WinHeight * textureArray.ChannelCount);
//            }
            context->CreateTexture(textureArray);
        }

        MaterialStorage::~MaterialStorage() {
            for (auto& material : Table) {
                MaterialManager::FreeMaterial(material.second);
            }
            Table.clear();
        }

        void MaterialManager::Init(Context* context) {
            LogInfo("MaterialManager::Init()");

            s_Context = context;
            s_MaterialBuilder = { s_Context, &s_TempMaterial };
            s_Storage = new MaterialStorage(s_Context, K_MATERIALS_COUNT);

            LogInfo("MaterialManager initialized");
        }

        void MaterialManager::Free() {
            LogInfo("MaterialManager::Free()");
            FreeMaterialList();
            delete s_Storage;
        }

        void MaterialManager::FreeMaterialList() {
            auto& textures = s_Storage->Textures;

            s_Storage->Buffer.Free();

            s_Context->FreeSampler(&textures.Sampler);

            s_Context->FreeTexture(&textures.AlbedoArray);
            s_Context->FreeTexture(&textures.BumpArray);
            s_Context->FreeTexture(&textures.ParallaxArray);
            s_Context->FreeTexture(&textures.MetallicArray);
            s_Context->FreeTexture(&textures.RoughnessArray);
            s_Context->FreeTexture(&textures.AOArray);
            s_Context->FreeTexture(&textures.EmissionArray);
        }

        MaterialBuilder& MaterialManager::Builder() {
            s_TempMaterial = {};
            InitMaterial(s_TempMaterial);
            s_MaterialBuilder = { s_Context, &s_TempMaterial };
            return s_MaterialBuilder;
        }

        MaterialTextures& MaterialManager::Textures() {
            return s_Storage->Textures;
        }

        vector<MaterialBufferData>& MaterialManager::List() {
            return s_Storage->Buffer.GetList();
        }

        void MaterialManager::InitMaterial(Material &material) {
            material.Textures = &s_Storage->Textures;
            material.Data = s_Storage->Buffer.GetItem(material.Index);
        }

        void MaterialManager::FreeMaterial(Material &material) {
            material.Textures = nullptr;
            material.Data = nullptr;
        }

        void MaterialManager::BindMaterials() {
            auto& textures = s_Storage->Textures;

            s_Context->BindSampler(&textures.Sampler);

            s_Context->BindTexture(&textures.AlbedoArray);
            s_Context->BindTexture(&textures.BumpArray);
            s_Context->BindTexture(&textures.ParallaxArray);
            s_Context->BindTexture(&textures.MetallicArray);
            s_Context->BindTexture(&textures.RoughnessArray);
            s_Context->BindTexture(&textures.AOArray);
            s_Context->BindTexture(&textures.EmissionArray);
        }

        void MaterialManager::UpdateMaterials() {
            s_Storage->Buffer.Flush();
        }

        void MaterialManager::UpdateMaterial(Material &material) {
            s_Storage->Buffer.FlushItem(material.Index, *material.Data);
        }

        MaterialBuffer* MaterialManager::GetBuffer() {
            return &s_Storage->Buffer;
        }

        void MaterialManager::AddMaterial(const string &name, const Material &material) {
            s_Storage->Table[name] = material;
        }

        void MaterialManager::RemoveMaterial(const string &name) {
            auto& table = s_Storage->Table;
            auto it = table.find(name);
            if (it != table.end()) {
                FreeMaterial(it->second);
                table.erase(it);
            }
        }

        Material* MaterialManager::GetMaterial(const string &name) {
            return &s_Storage->Table[name];
        }

        MaterialBufferData* MaterialManager::GetMaterialData(u32 index) {
            return s_Storage->Buffer.GetItem(index);
        }

    }

}