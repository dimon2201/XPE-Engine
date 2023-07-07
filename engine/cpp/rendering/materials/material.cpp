#include <rendering/context.hpp>
#include <rendering/materials/material.h>

namespace xpe {

    namespace render {

        void MaterialBuffer::Init(Context *context) {
            m_Context = context;
            Type = eBufferType::CONSTANT;
            ByteSize = sizeof(MaterialBufferData);
            Slot = 1;
            m_Context->CreateBuffer(*this, K_FALSE);
        }

        void MaterialBuffer::Free() {
            m_Context->FreeBuffer(*this);
        }

        void MaterialBuffer::Flush() {
            m_Context->WriteBuffer(*this, &m_Data, sizeof(m_Data));
        }

        void MaterialBuffer::SetMaterial(const Material &material) {
            m_Data = material.Data;
        }

        Material* MaterialBuilder::Build(const string& name) {
            MaterialManager::AddMaterial(name, *m_Material);
            return MaterialManager::GetMaterial(name);
        }

        MaterialBuilder& MaterialBuilder::AddAlbedoFromFile(const char *filepath) {
            m_Material->Albedo = TextureManager::LoadTexture(filepath, Texture::eFormat::RGBA8);
            return *this;
        }

        MaterialBuilder& MaterialBuilder::AddBumpFromFile(const char *filepath) {
            m_Material->Bumping = TextureManager::LoadTexture(filepath, Texture::eFormat::RGB8);
            return *this;
        }

        MaterialBuilder& MaterialBuilder::AddParallaxFromFile(const char *filepath) {
            m_Material->Parallax = TextureManager::LoadTexture(filepath, Texture::eFormat::R32);
            return *this;
        }

        MaterialBuilder& MaterialBuilder::AddMetallicFromFile(const char *filepath) {
            m_Material->Metallic = TextureManager::LoadTexture(filepath, Texture::eFormat::R32);
            return *this;
        }

        MaterialBuilder& MaterialBuilder::AddRoughnessFromFile(const char *filepath) {
            m_Material->Roughness = TextureManager::LoadTexture(filepath, Texture::eFormat::R32);
            return *this;
        }

        MaterialBuilder& MaterialBuilder::AddAOFromFile(const char *filepath) {
            m_Material->AO = TextureManager::LoadTexture(filepath, Texture::eFormat::R32);
            return *this;
        }

        MaterialBuilder& MaterialBuilder::AddEmissionFromFile(const char *filepath) {
            m_Material->Emission = TextureManager::LoadTexture(filepath, Texture::eFormat::RGBA8);
            return *this;
        }

        Context* MaterialManager::s_Context = nullptr;
        MaterialBuilder MaterialManager::s_MaterialBuilder;
        unordered_map<string, Material> MaterialManager::s_MaterialTable;
        Material MaterialManager::s_TempMaterial;
        MaterialBuffer MaterialManager::s_MaterialBuffer;

        void MaterialManager::Init(Context *context) {
            s_Context = context;
            s_MaterialBuilder = { s_Context, &s_TempMaterial };
            s_MaterialBuffer.Init(context);
        }

        void MaterialManager::Free() {
            for (auto& material : s_MaterialTable) {
                FreeMaterial(material.second);
            }
            s_MaterialTable.clear();
            s_MaterialBuffer.Free();
        }

        MaterialBuilder& MaterialManager::Builder() {
            s_TempMaterial = {};
            InitMaterial(s_TempMaterial);
            s_MaterialBuilder = { s_Context, &s_TempMaterial };
            return s_MaterialBuilder;
        }

        void MaterialManager::InitMaterial(Material &material) {
            s_Context->CreateSampler(material.Sampler);
        }

        void MaterialManager::FreeMaterial(Material &material) {
            s_Context->FreeSampler(&material.Sampler);

            if (material.Albedo) {
                s_Context->FreeTexture(material.Albedo);
            }

            if (material.Bumping) {
                s_Context->FreeTexture(material.Bumping);
            }

            if (material.Parallax) {
                s_Context->FreeTexture(material.Parallax);
            }

            if (material.Metallic) {
                s_Context->FreeTexture(material.Metallic);
            }

            if (material.Roughness) {
                s_Context->FreeTexture(material.Roughness);
            }

            if (material.AO) {
                s_Context->FreeTexture(material.AO);
            }

            if (material.Emission) {
                s_Context->FreeTexture(material.Emission);
            }
        }

        void MaterialManager::BindMaterial(Material &material, const eShaderType& shaderType, u32 slot) {
            s_Context->BindSampler(&material.Sampler);

            if (material.Albedo) {
                s_Context->BindTexture(material.Albedo, shaderType, slot);
            }

            if (material.Bumping) {
                s_Context->BindTexture(material.Bumping, shaderType, slot + 1);
            }

            if (material.Parallax) {
                s_Context->BindTexture(material.Parallax, shaderType, slot + 2);
            }

            if (material.Metallic) {
                s_Context->BindTexture(material.Metallic, shaderType, slot + 3);
            }

            if (material.Roughness) {
                s_Context->BindTexture(material.Roughness, shaderType, slot + 4);
            }

            if (material.AO) {
                s_Context->BindTexture(material.AO, shaderType, slot + 5);
            }

            if (material.Emission) {
                s_Context->BindTexture(material.AO, shaderType, slot + 6);
            }
        }

        void MaterialManager::UpdateMaterial(Material &material) {
            s_MaterialBuffer.SetMaterial(material);
            s_MaterialBuffer.Flush();
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