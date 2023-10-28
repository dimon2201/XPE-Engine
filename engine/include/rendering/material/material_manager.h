#pragma once

#include <rendering/material/material.h>

namespace xpe {

    namespace render {

        class ENGINE_API MaterialManager final
        {

        public:
            static TextureSampler Sampler;
            static Ref<Texture> AlbedoAtlas;
            static Ref<Texture> NormalAtlas;
            static Ref<Texture> ParallaxAtlas;
            static Ref<Texture> MetalAtlas;
            static Ref<Texture> RoughnessAtlas;
            static Ref<Texture> AOAtlas;
            static Ref<Texture> EmissionAtlas;

            static void Init();
            static void Free();

            template<typename... Args>
            static Ref<Material> AddMaterial(const string& name, Args&&... args);

            static Ref<Material>& GetMaterial(const string& name) { return s_Map->at(name); }

            static void Remove(const string& name);

            static void Clear();

            static void Bind(Pipeline& pipeline);

            static bool Has(const string& name) { return s_Map->find(name) != s_Map->end(); }

        private:
            static void InitSampler();

            static Ref<Texture> InitTextureArray(const MaterialFormat& materialFormat);

            static unordered_map<string, Ref<Material>>* s_Map;
            static Ref<MaterialDataBuffer> s_DataBuffer;
        };

        template<typename... Args>
        Ref<Material> MaterialManager::AddMaterial(const string& name, Args &&... args)
        {
            Ref<Material> materialRef;

            materialRef.Create(std::forward<Args>(args)...);
            materialRef->Index = s_DataBuffer->Size();
            materialRef->Buffer = s_DataBuffer;
            materialRef->AlbedoAtlas = AlbedoAtlas;
            materialRef->NormalAtlas = NormalAtlas;
            materialRef->ParallaxAtlas = ParallaxAtlas;
            materialRef->MetalAtlas = MetalAtlas;
            materialRef->RoughnessAtlas = RoughnessAtlas;
            materialRef->AOAtlas = AOAtlas;
            materialRef->EmissionAtlas = EmissionAtlas;

            s_Map->operator[](name) = materialRef;

            Material& material = *materialRef;

            s_DataBuffer->Add(material);

            material.AddLayer(*material.AlbedoAtlas, material.AlbedoMap);
            material.AddLayer(*material.NormalAtlas, material.NormalMap);
            material.AddLayer(*material.ParallaxAtlas, material.ParallaxMap);
            material.AddLayer(*material.MetalAtlas, material.MetalMap);
            material.AddLayer(*material.RoughnessAtlas, material.RoughnessMap);
            material.AddLayer(*material.AOAtlas, material.AOMap);
            material.AddLayer(*material.EmissionAtlas, material.EmissionMap);

            s_DataBuffer->Flush();

            return materialRef;
        }

    }

}