#pragma once

#include <rendering/material/material.h>

namespace xpe {

    namespace render {

        class ENGINE_API MaterialManager final
        {

        public:
            static TextureSampler Sampler;
            static Texture* AlbedoAtlas;
            static Texture* NormalAtlas;
            static Texture* ParallaxAtlas;
            static Texture* MetalAtlas;
            static Texture* RoughnessAtlas;
            static Texture* AOAtlas;
            static Texture* EmissionAtlas;

            static void Init();
            static void Free();

            template<typename... Args>
            static Material AddMaterial(Args&&... args);

            static void Clear();

            static void Bind(Pipeline& pipeline);

            static void Flush(const Material& material);

            static void AddLayer(Texture& texture, TextureLayer& layer);

            static void SetLayer(Texture& texture, TextureLayer& layer, u32 layerIndex);

        private:
            static void InitSampler();

            static Texture* InitTextureArray(const MaterialFormat& materialFormat);

            static MaterialDataBuffer* s_DataBuffer;
        };

        template<typename... Args>
        Material MaterialManager::AddMaterial(Args &&... args)
        {
            Material material(std::forward<Args>(args)...);

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