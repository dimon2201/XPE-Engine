#ifndef _H_MATERIAL_
#define _H_MATERIAL_

struct Material
{
    float4 DiffuseColor;
    float4 DiffuseTextureOffsets;
};

StructuredBuffer<Material> Materials : K_SLOT_BUFFER_MATERIALS;

SamplerState MaterialSampler : K_SLOT_MATERIAL_SAMPLER;

Texture2D<float4> TextureAtlas : K_SLOT_TEXTURE_ATLAS;

uint2 GetTextureSize(in Texture2D<float4> tex)
{
    uint width = 0;
    uint height = 0;
    tex.GetDimensions(width, height);

    return uint2(width, height);
}

float4 AtlasLoad(in Texture2D<float4> atlas, float4 textureOffsets, float2 uv)
{
    float2 atlasSize = float2(GetTextureSize(atlas));
    float2 bottomLeft = float2(textureOffsets.x, textureOffsets.y) / atlasSize;
    float2 topRight = float2(textureOffsets.z, textureOffsets.w) / atlasSize;
    float2 textureSize = topRight - bottomLeft;

    uv.x = (uv.x * textureSize.x) + bottomLeft.x;
    uv.y = (uv.y * textureSize.y) + bottomLeft.y;

    return atlas.Load(int3(float2(atlasSize.x, atlasSize.y) * uv.xy, 0));
}

#endif