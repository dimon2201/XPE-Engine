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

            u32 GetIndex(Material* material);

            void Remove(const string& name);

            void Clear();

            void BindPipeline(Pipeline& pipeline);

        private:
            void InitSampler();

            Ref<Texture> InitTextureArray(const MaterialFormat& materialFormat);

            void AddLayer(Texture& texture, TextureLayer& layer);
            void SetLayer(Texture& texture, TextureLayer& layer, u32 layerIndex);

            unordered_map<string, Ref<Material>> m_Materials;
            unordered_map<Material*, u32> m_MaterialIndices;
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

            m_Materials.insert({ name, materialRef });
            m_MaterialIndices.insert({ materialRef.Get(), m_MaterialIndices.size() });

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
            u32 materialIndex = m_MaterialIndices[materialRef.Get()];

            m_DataBuffer.FlushItem(materialIndex, material);
//            *m_DataBuffer[materialIndex] = material;
//            m_DataBuffer.Flush();

            SetLayer(*AlbedoArray, material.Albedo, materialIndex);
            SetLayer(*BumpArray, material.Bumping, materialIndex);
            SetLayer(*ParallaxArray, material.Parallax, materialIndex);
            SetLayer(*MetallicArray, material.Metallic, materialIndex);
            SetLayer(*RoughnessArray, material.Roughness, materialIndex);
            SetLayer(*AOArray, material.AO, materialIndex);
            SetLayer(*EmissionArray, material.Emission, materialIndex);

            return materialRef;
        }

    }

}