#pragma once

#include <rendering/material/material.h>

namespace xpe {

    namespace render {

        class ENGINE_API cMaterialManager final
        {

        public:
            static sSampler Sampler;
            static sTexture* AlbedoAtlas;
            static sTexture* NormalAtlas;
            static sTexture* ParallaxAtlas;
            static sTexture* MetalAtlas;
            static sTexture* RoughnessAtlas;
            static sTexture* AOAtlas;
            static sTexture* EmissionAtlas;

            static void Init();
            static void Free();

            template<typename... Args>
            static sMaterial AddMaterial(Args&&... args);

            static void Clear();

            static void Bind(sPipeline& pipeline);

            static void Flush(const sMaterial& material);

            static void AddLayer(sTexture& texture, sTextureLayer& layer);

            static void SetLayer(sTexture& texture, sTextureLayer& layer, u32 layerIndex);

        private:
            static void InitSampler();

            static sTexture* InitTextureArray(const sMaterialFormat& materialFormat);

            static sMaterialDataBuffer* s_DataBuffer;
        };

        template<typename... Args>
        sMaterial cMaterialManager::AddMaterial(Args &&... args)
        {
            sMaterial material(std::forward<Args>(args)...);

            material.Index = s_DataBuffer->Size();
            s_DataBuffer->Add(material);

            AddLayer(*AlbedoAtlas, material.AlbedoMap);
            AddLayer(*NormalAtlas, material.NormalMap);
            AddLayer(*ParallaxAtlas, material.ParallaxMap);
            AddLayer(*MetalAtlas, material.MetalMap);
            AddLayer(*RoughnessAtlas, material.RoughnessMap);
            AddLayer(*AOAtlas, material.AOMap);
            AddLayer(*EmissionAtlas, material.EmissionMap);

            s_DataBuffer->Flush();

            return material;
        }

    }

}