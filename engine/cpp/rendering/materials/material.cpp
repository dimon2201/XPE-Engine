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
        MaterialBuilder MaterialManager::s_MaterialBuilder;
        unordered_map<string, Material> MaterialManager::s_MaterialTable;
        Material MaterialManager::s_TempMaterial;
        MaterialBuffer MaterialManager::s_MaterialBuffer;
        MaterialTextures MaterialManager::s_MaterialTextures;

        void MaterialManager::Init(Context* context) {
            LogInfo("MaterialManager::Init()");

            s_Context = context;
            s_MaterialBuilder = { s_Context, &s_TempMaterial };
            InitMaterialList();

            LogInfo("MaterialManager initialized");
        }

        void MaterialManager::Free() {
            LogInfo("MaterialManager::Free()");

            for (auto& material : s_MaterialTable) {
                FreeMaterial(material.second);
            }
            s_MaterialTable.clear();

            FreeMaterialList();
        }

        void MaterialManager::InitMaterialList() {
            s_MaterialBuffer = MaterialBuffer(s_Context, K_MATERIALS_COUNT);

            s_Context->CreateSampler(s_MaterialTextures.Sampler);

            InitTextureArray(s_MaterialTextures.AlbedoArray, Texture::eFormat::RGBA8, 389, 600, K_SLOT_ALBEDO);
            InitTextureArray(s_MaterialTextures.BumpArray, Texture::eFormat::RGB8, 389, 600, K_SLOT_BUMPING);
            InitTextureArray(s_MaterialTextures.ParallaxArray, Texture::eFormat::R32, 389, 600, K_SLOT_PARALLAX);
            InitTextureArray(s_MaterialTextures.MetallicArray, Texture::eFormat::R32, 389, 600, K_SLOT_METALLIC);
            InitTextureArray(s_MaterialTextures.RoughnessArray, Texture::eFormat::R32, 389, 600, K_SLOT_ROUGHNESS);
            InitTextureArray(s_MaterialTextures.AOArray, Texture::eFormat::R32, 389, 600, K_SLOT_AO);
            InitTextureArray(s_MaterialTextures.EmissionArray, Texture::eFormat::RGB8, 389, 600, K_SLOT_EMISSION);
        }

        void MaterialManager::InitTextureArray(Texture &textureArray, const Texture::eFormat& format, usize width, usize height, u32 slot) {
            textureArray.InitializeData = false;
            textureArray.Type = Texture::eType::TEXTURE_2D_ARRAY;
            textureArray.Usage = Texture::eUsage::DEFAULT;
            textureArray.Format = format;
            textureArray.Width = width;
            textureArray.Height = height;
            textureArray.Slot = slot;
            textureArray.ChannelCount = TextureManager::ChannelTable.at(format);
            textureArray.Layers.resize(K_MATERIALS_COUNT);
//            for (auto& layer : textureArray.Layers) {
//                layer.Pixels = MemoryPoolManager::Allocate(textureArray.WinWidth * textureArray.WinHeight * textureArray.ChannelCount);
//            }
            s_Context->CreateTexture(textureArray, nullptr);
        }

        void MaterialManager::FreeMaterialList() {
            s_MaterialBuffer.Free();

            s_Context->FreeSampler(&s_MaterialTextures.Sampler);

            s_Context->FreeTexture(&s_MaterialTextures.AlbedoArray);
            s_Context->FreeTexture(&s_MaterialTextures.BumpArray);
            s_Context->FreeTexture(&s_MaterialTextures.ParallaxArray);
            s_Context->FreeTexture(&s_MaterialTextures.MetallicArray);
            s_Context->FreeTexture(&s_MaterialTextures.RoughnessArray);
            s_Context->FreeTexture(&s_MaterialTextures.AOArray);
            s_Context->FreeTexture(&s_MaterialTextures.EmissionArray);
        }

        MaterialBuilder& MaterialManager::Builder() {
            s_TempMaterial = {};
            InitMaterial(s_TempMaterial);
            s_MaterialBuilder = { s_Context, &s_TempMaterial };
            return s_MaterialBuilder;
        }

        MaterialTextures& MaterialManager::Textures() {
            return s_MaterialTextures;
        }

        vector<MaterialBufferData>& MaterialManager::List() {
            return s_MaterialBuffer.GetList();
        }

        void MaterialManager::InitMaterial(Material &material) {
            material.Textures = &s_MaterialTextures;
            material.Data = s_MaterialBuffer.GetItem(material.Index);
        }

        void MaterialManager::FreeMaterial(Material &material) {
            material.Textures = nullptr;
            material.Data = nullptr;
        }

        void MaterialManager::BindMaterials() {
            s_Context->BindSampler(&s_MaterialTextures.Sampler);

            s_Context->BindTexture(&s_MaterialTextures.AlbedoArray);
            s_Context->BindTexture(&s_MaterialTextures.BumpArray);
            s_Context->BindTexture(&s_MaterialTextures.ParallaxArray);
            s_Context->BindTexture(&s_MaterialTextures.MetallicArray);
            s_Context->BindTexture(&s_MaterialTextures.RoughnessArray);
            s_Context->BindTexture(&s_MaterialTextures.AOArray);
            s_Context->BindTexture(&s_MaterialTextures.EmissionArray);
        }

        void MaterialManager::UpdateMaterials() {
            s_MaterialBuffer.Flush();
        }

        void MaterialManager::UpdateMaterial(Material &material) {
            s_MaterialBuffer.FlushItem(material.Index, *material.Data);
        }

        MaterialBuffer* MaterialManager::GetBuffer() {
            return &s_MaterialBuffer;
        }

        void MaterialManager::AddMaterial(const string &name, const Material &material) {
            s_MaterialTable[name] = material;
        }

        void MaterialManager::RemoveMaterial(const string &name) {
            auto it = s_MaterialTable.find(name);
            if (it != s_MaterialTable.end()) {
                FreeMaterial(it->second);
                s_MaterialTable.erase(it);
            }
        }

        Material* MaterialManager::GetMaterial(const string &name) {
            return &s_MaterialTable[name];
        }

        MaterialBufferData* MaterialManager::GetMaterialData(u32 index) {
            return s_MaterialBuffer.GetItem(index);
        }

    }

}