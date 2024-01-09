#pragma once

#include <rendering/material/material.hpp>
#include <rendering/bindings.hpp>

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

            static void AddLayer(cTexture& texture, sTextureLayer* layer);

            static void SetLayer(cTexture& texture, sTextureLayer* layer, u32 layerIndex);

        private:
            static void InitSampler();
            static cTexture* InitTextureArray(const sMaterialFormat& materialFormat);
        };

        template<typename... Args>
        sMaterial cMaterialManager::AddMaterial(Args &&... args)
        {
            sMaterial material(std::forward<Args>(args)...);

            usize& counter = Buffers::Material->GetCounterRef();
            material.Index = counter;
            Buffers::Material->GetList()[counter] = material;
            counter += 1;

            AddLayer(*Textures::AlbedoAtlas, &material.AlbedoMap);
            AddLayer(*Textures::NormalAtlas, &material.NormalMap);
            AddLayer(*Textures::ParallaxAtlas, &material.ParallaxMap);
            AddLayer(*Textures::MetalAtlas, &material.MetalMap);
            AddLayer(*Textures::RoughnessAtlas, &material.RoughnessMap);
            AddLayer(*Textures::AOAtlas, &material.AOMap);
            AddLayer(*Textures::EmissionAtlas, &material.EmissionMap);

            Buffers::Material->Flush();

            return material;
        }

    }

}