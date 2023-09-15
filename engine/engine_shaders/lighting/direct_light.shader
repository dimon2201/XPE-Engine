struct DirectLight
{
    float3 Position;
    float3 Color;
};

StructuredBuffer<DirectLight> DirectLights : K_SLOT_DIRECT_LIGHTS;

float Attenuation(DirectLight directLight) {
    float d = length(directLight.Position - W);
    return 1.0 / (d * d);
}