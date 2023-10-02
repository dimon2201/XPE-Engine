#pragma once

#include <rendering/materials/material.h>

namespace xpe {

    namespace render {

        struct ENGINE_API MaterialStorage : public Object
        {
            TextureSampler Sampler;
            Ref<Texture> AlbedoAtlas;
            Ref<Texture> NormalAtlas;
            Ref<Texture> ParallaxAtlas;
            Ref<Texture> MetalAtlas;
            Ref<Texture> RoughnessAtlas;
            Ref<Texture> AOAtlas;
            Ref<Texture> EmissionAtlas;

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
            materialRef->AlbedoAtlas = AlbedoAtlas;
            materialRef->NormalAtlas = NormalAtlas;
            materialRef->ParallaxAtlas = ParallaxAtlas;
            materialRef->MetalAtlas = MetalAtlas;
            materialRef->RoughnessAtlas = RoughnessAtlas;
            materialRef->AOAtlas = AOAtlas;
            materialRef->EmissionAtlas = EmissionAtlas;

            m_Materials[name] = materialRef;

            Material& material = *materialRef;

            m_DataBuffer->Add(material);

            material.AddLayer(*material.AlbedoAtlas, material.AlbedoMap);
            material.AddLayer(*material.NormalAtlas, material.NormalMap);
            material.AddLayer(*material.ParallaxAtlas, material.ParallaxMap);
            material.AddLayer(*material.MetalAtlas, material.MetalMap);
            material.AddLayer(*material.RoughnessAtlas, material.RoughnessMap);
            material.AddLayer(*material.AOAtlas, material.AOMap);
            material.AddLayer(*material.EmissionAtlas, material.EmissionMap);

            m_DataBuffer->Flush();

            return materialRef;
        }

    }

}