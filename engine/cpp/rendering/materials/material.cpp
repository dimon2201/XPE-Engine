#include <rendering/context.hpp>
#include <rendering/materials/material.h>
#include <rendering/shader.h>

namespace xpe {

    namespace render {

        void MaterialBuffer::Init(Context* context, MaterialList* list) {
            m_Context = context;
            m_List = list;
            Type = eBufferType::STRUCTURED;
            FirstElement = 0;
            NumElements = list->DataArray.size();
            StructureSize = sizeof(MaterialBufferData);
            ByteSize = StructureSize * NumElements;
            Slot = K_SLOT_MATERIALS;
            m_Context->CreateBuffer(*this, K_FALSE);
        }

        void MaterialBuffer::Free() {
            m_Context->FreeBuffer(*this);
        }

        void MaterialBuffer::Flush() {
            m_Context->WriteBuffer(*this, m_List->DataArray.data(), ByteSize);
        }

        void MaterialBuffer::Flush(Material &material) {
            m_Context->WriteBufferOffset(*this, StructureSize * material.Index, material.Data, StructureSize);
        }

        Material* MaterialBuilder::Build(const string& name) {
            MaterialManager::AddMaterial(name, *m_Material);
            return MaterialManager::GetMaterial(name);
        }

        MaterialBuilder& MaterialBuilder::AddAlbedoFromFile(const char *filepath) {
            m_Material->Albedo = AddTextureFromFile(filepath, MaterialManager::List().AlbedoArray);
            return *this;
        }

        MaterialBuilder& MaterialBuilder::AddBumpFromFile(const char *filepath) {
            m_Material->Bumping = AddTextureFromFile(filepath, MaterialManager::List().BumpArray);
            return *this;
        }

        MaterialBuilder& MaterialBuilder::AddParallaxFromFile(const char *filepath) {
            m_Material->Parallax = AddTextureFromFile(filepath, MaterialManager::List().ParallaxArray);
            return *this;
        }

        MaterialBuilder& MaterialBuilder::AddMetallicFromFile(const char *filepath) {
            m_Material->Metallic = AddTextureFromFile(filepath, MaterialManager::List().MetallicArray);
            return *this;
        }

        MaterialBuilder& MaterialBuilder::AddRoughnessFromFile(const char *filepath) {
            m_Material->Roughness = AddTextureFromFile(filepath, MaterialManager::List().RoughnessArray);
            return *this;
        }

        MaterialBuilder& MaterialBuilder::AddAOFromFile(const char *filepath) {
            m_Material->AO = AddTextureFromFile(filepath, MaterialManager::List().AOArray);
            return *this;
        }

        MaterialBuilder& MaterialBuilder::AddEmissionFromFile(const char *filepath) {
            m_Material->Emission = AddTextureFromFile(filepath, MaterialManager::List().EmissionArray);
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
        MaterialList MaterialManager::s_MaterialList;

        void MaterialManager::Init(Context* context) {
            s_Context = context;
            s_MaterialBuilder = { s_Context, &s_TempMaterial };
            InitMaterialList();
        }

        void MaterialManager::Free() {
            for (auto& material : s_MaterialTable) {
                FreeMaterial(material.second);
            }
            s_MaterialTable.clear();
            FreeMaterialList();
        }

        void MaterialManager::InitMaterialList() {
            s_MaterialList.DataArray.resize(k_MaterialsCount);
            s_MaterialBuffer.Init(s_Context, &s_MaterialList);

            s_Context->CreateSampler(s_MaterialList.Sampler);

            InitTextureArray(s_MaterialList.AlbedoArray, Texture::eFormat::RGBA8, 389, 600, 1);
            InitTextureArray(s_MaterialList.BumpArray, Texture::eFormat::RGB8, 389, 600, 2);
            InitTextureArray(s_MaterialList.ParallaxArray, Texture::eFormat::R32, 389, 600, 3);
            InitTextureArray(s_MaterialList.MetallicArray, Texture::eFormat::R32, 389, 600, 4);
            InitTextureArray(s_MaterialList.RoughnessArray, Texture::eFormat::R32, 389, 600, 5);
            InitTextureArray(s_MaterialList.AOArray, Texture::eFormat::R32, 389, 600, 6);
            InitTextureArray(s_MaterialList.EmissionArray, Texture::eFormat::RGB8, 389, 600, 7);
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
            textureArray.Layers.resize(k_MaterialsCount);
//            for (auto& layer : textureArray.Layers) {
//                layer.Pixels = MemoryPoolManager::Allocate(textureArray.Width * textureArray.Height * textureArray.ChannelCount);
//            }
            s_Context->CreateTexture(textureArray, nullptr);
        }

        void MaterialManager::FreeMaterialList() {
            s_MaterialBuffer.Free();
            s_MaterialList.DataArray.clear();

            s_Context->FreeSampler(&s_MaterialList.Sampler);

            s_Context->FreeTexture(&s_MaterialList.AlbedoArray);
            s_Context->FreeTexture(&s_MaterialList.BumpArray);
            s_Context->FreeTexture(&s_MaterialList.ParallaxArray);
            s_Context->FreeTexture(&s_MaterialList.MetallicArray);
            s_Context->FreeTexture(&s_MaterialList.RoughnessArray);
            s_Context->FreeTexture(&s_MaterialList.AOArray);
            s_Context->FreeTexture(&s_MaterialList.EmissionArray);
        }

        MaterialBuilder& MaterialManager::Builder() {
            s_TempMaterial = {};
            InitMaterial(s_TempMaterial);
            s_MaterialBuilder = { s_Context, &s_TempMaterial };
            return s_MaterialBuilder;
        }

        MaterialList& MaterialManager::List() {
            return s_MaterialList;
        }

        void MaterialManager::InitMaterial(Material &material) {
            u32 index = material.Index;
            u32 materialListSize = s_MaterialList.DataArray.size();
            material.List = &s_MaterialList;
            if (index >= materialListSize) {
                LogError("Failed to initialize material. Index out of bounds {} >= {}", index, materialListSize);
                return;
            } else {
                material.Data = &s_MaterialList.DataArray[material.Index];
            }
        }

        void MaterialManager::FreeMaterial(Material &material) {
            material.List = nullptr;
            material.Data = nullptr;
        }

        void MaterialManager::BindMaterials() {
            s_Context->BindSampler(&s_MaterialList.Sampler);

            s_Context->BindTexture(&s_MaterialList.AlbedoArray);
            s_Context->BindTexture(&s_MaterialList.BumpArray);
            s_Context->BindTexture(&s_MaterialList.ParallaxArray);
            s_Context->BindTexture(&s_MaterialList.MetallicArray);
            s_Context->BindTexture(&s_MaterialList.RoughnessArray);
            s_Context->BindTexture(&s_MaterialList.AOArray);
            s_Context->BindTexture(&s_MaterialList.EmissionArray);
        }

        void MaterialManager::UpdateMaterials() {
            s_MaterialBuffer.Flush();
        }

        void MaterialManager::UpdateMaterial(Material &material) {
            s_MaterialBuffer.Flush(material);
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

    }

}