#pragma once

#include <rendering/materials/material.h>

namespace xpe {

    namespace render {

        struct ENGINE_API MaterialStorage : public Object
        {
            TextureSampler Sampler;
            Ref<Texture> AlbedoArray;
            Ref<Texture> BumpArray;
            Ref<Texture> ParallaxArray;
            Ref<Texture> MetallicArray;
            Ref<Texture> RoughnessArray;
            Ref<Texture> AOArray;
            Ref<Texture> EmissionArray;

            MaterialStorage();
            ~MaterialStorage();

            template<typename... Args>
            Ref<Material> Add(const string& name, Args&&... args);

            template<typename... Args>
            Ref<Material> Set(const string& name, Args&&... args);

            inline Ref<Material>& Get(const string& name) { return m_Materials[name]; }

            void Remove(const string& name);

            void Clear();

            void BindPipeline(Pipeline& pipeline);

        private:
            void InitSampler();

            Ref<Texture> InitTextureArray(const MaterialFormat& materialFormat);

            void AddLayer(Texture& texture, TextureLayer& layer);
            void SetLayer(Texture& texture, TextureLayer& layer, u32 layerIndex);

            unordered_map<string, Ref<Material>> m_Materials;
            MaterialDataBuffer m_DataBuffer;
        };

        template<typename... Args>
        Ref<Material> MaterialStorage::Add(const string &name, Args &&... args)
        {
            if (m_Materials.find(name) != m_Materials.end()) {
                return Set(name, std::forward<Args>(args)...);
            }

            Ref<Material> materialRef;
            materialRef.Create(std::forward<Args>(args)...);
            materialRef->Index = m_DataBuffer.Size();

            m_Materials[name] = materialRef;

            Material& material = *materialRef;

            m_DataBuffer.Add(material);
            m_DataBuffer.Flush();

            AddLayer(*AlbedoArray, material.Albedo);
            AddLayer(*BumpArray, material.Bumping);
            AddLayer(*ParallaxArray, material.Parallax);
            AddLayer(*MetallicArray, material.Metallic);
            AddLayer(*RoughnessArray, material.Roughness);
            AddLayer(*AOArray, material.AO);
            AddLayer(*EmissionArray, material.Emission);

            return materialRef;
        }

        template<typename... Args>
        Ref<Material> MaterialStorage::Set(const string &name, Args &&... args)
        {
            auto& materialRef = Get(name);
            Material& material = *materialRef;
            material = Material(std::forward<Args>(args)...);

            m_DataBuffer.FlushItem(material.Index, material);

            SetLayer(*AlbedoArray, material.Albedo, material.Index);
            SetLayer(*BumpArray, material.Bumping, material.Index);
            SetLayer(*ParallaxArray, material.Parallax, material.Index);
            SetLayer(*MetallicArray, material.Metallic, material.Index);
            SetLayer(*RoughnessArray, material.Roughness, material.Index);
            SetLayer(*AOArray, material.AO, material.Index);
            SetLayer(*EmissionArray, material.Emission, material.Index);

            return materialRef;
        }

    }

}