#pragma once

#include <rendering/material/material.h>
#include <rendering/bindings.h>

namespace xpe {

    namespace render {

        class ENGINE_API cMaterialManager final
        {
        public:
            static void Init();
            static void Free();

            template<typename... Args>
            static sMaterial AddMaterial(Args&&... args);

            static void Clear();

            static void Flush(const sMaterial& material);

            static void AddLayer(sTexture& texture, sTextureLayer* layer);

            static void SetLayer(sTexture& texture, sTextureLayer* layer, u32 layerIndex);

        private:
            static void InitSampler();
            static sTexture* InitTextureArray(const sMaterialFormat& materialFormat);
        };

        template<typename... Args>
        sMaterial cMaterialManager::AddMaterial(Args &&... args)
        {
            sMaterial material(std::forward<Args>(args)...);

            material.Index = Buffers::Material->Size();
            Buffers::Material->Add(material);

            AddLayer(*Textures::AlbedoAtlas, material.AlbedoMap);
            AddLayer(*Textures::NormalAtlas, material.NormalMap);
            AddLayer(*Textures::ParallaxAtlas, material.ParallaxMap);
            AddLayer(*Textures::MetalAtlas, material.MetalMap);
            AddLayer(*Textures::RoughnessAtlas, material.RoughnessMap);
            AddLayer(*Textures::AOAtlas, material.AOMap);
            AddLayer(*Textures::EmissionAtlas, material.EmissionMap);

            Buffers::Material->Flush();

            return material;
        }

    }

}