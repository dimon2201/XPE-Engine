#pragma once

#include <bindings.hpp>

namespace xpe::ecs
{
    class cScene;
    class cEntity;
    class CMaterial;
    class CMaterialInfo;
}

namespace xpe
{
    namespace render
    {
        struct ENGINE_API sMaterialFilepath final
        {
            string AlbedoFilepath;
            string BumpFilepath;
            string ParallaxFilepath;
            string MetallicFilepath;
            string RoughnessFilepath;
            string AOFilepath;
            string EmissionFilepath;
        };

        struct ENGINE_API sMaterialFormat final
        {
            eTextureFormat Format;
            usize Width;
            usize Height;
            u32 Slot;
        };

        class ENGINE_API MMaterial final
        {

        public:
            static void Init();
            static void Free();

            static CMaterialInfo Add(const CMaterial& material);
            static CMaterial Load(const aiScene* scene, aiMesh* mesh, const hstring& directory);
            static CMaterial Load(const sMaterialFilepath& filepath);

            static void Clear();
            static void AddLayer(cTexture& texture, cTextureLayer* layer);
            static void SetLayer(cTexture& texture, cTextureLayer* layer, u32 layerIndex);

        private:
            static void InitSampler();
            static cTexture* InitTextureArray(const sMaterialFormat& materialFormat);
        };

    }
}