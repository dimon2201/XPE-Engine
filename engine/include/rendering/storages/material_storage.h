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

            inline Ref<Material>& Get(const string& name) { return m_Materials[name]; }

            void Remove(const string& name);

            void Clear();

            void BindPipeline(Pipeline& pipeline);

        private:
            void InitSampler();

            Ref<Texture> InitTextureArray(const MaterialFormat& materialFormat);

            unordered_map<string, Ref<Material>> m_Materials;
            Ref<MaterialDataBuffer> m_DataBuffer;
        };

        template<typename... Args>
        Ref<Material> MaterialStorage::Add(const string &name, Args &&... args)
        {
            Ref<Material> materialRef;

            materialRef.Create(std::forward<Args>(args)...);
            materialRef->Index = m_DataBuffer->Size();
            materialRef->Buffer = m_DataBuffer;
            materialRef->AlbedoArray = AlbedoArray;
            materialRef->BumpArray = BumpArray;
            materialRef->ParallaxArray = ParallaxArray;
            materialRef->MetallicArray = MetallicArray;
            materialRef->RoughnessArray = RoughnessArray;
            materialRef->AOArray = AOArray;
            materialRef->EmissionArray = EmissionArray;

            m_Materials[name] = materialRef;

            Material& material = *materialRef;

            m_DataBuffer->Add(material);

            material.AddLayer(*material.AlbedoArray, material.Albedo);
            material.AddLayer(*material.BumpArray, material.Bumping);
            material.AddLayer(*material.ParallaxArray, material.Parallax);
            material.AddLayer(*material.MetallicArray, material.Metallic);
            material.AddLayer(*material.RoughnessArray, material.Roughness);
            material.AddLayer(*material.AOArray, material.AO);
            material.AddLayer(*material.EmissionArray, material.Emission);

            m_DataBuffer->Flush();

            return materialRef;
        }

    }

}