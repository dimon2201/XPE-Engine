#include ../types.shader
#include ../particle.shader
#include ../camera.shader

struct VSOut
{
	float4 clipPosition : SV_POSITION;
	float3 worldPosition : XPE_POSITION_WORLD;
    float3 normal        : XPE_NORMAL_WORLD;
};

VSOut vs_main(uint vertexIndex : SV_VertexID, uint instanceIndex : SV_InstanceID)
{
	VSOut vsOut = (VSOut)0;

	float3 localPosition = 0;
	if (vertexIndex == 0) { localPosition = float3(-0.05, -0.05, 0.0); }
	if (vertexIndex == 1) { localPosition = float3(-0.05, 0.05, 0.0); }
	if (vertexIndex == 2) { localPosition = float3(0.05, -0.05, 0.0); }
	if (vertexIndex == 3) { localPosition = float3(0.05, 0.05, 0.0); }

	float3 cameraUp = float3(CameraView[1][0], CameraView[1][1], CameraView[1][2]);
	float3 cameraRight = float3(CameraView[0][0], CameraView[0][1], CameraView[0][2]);
	float3 localBillboardPosition = (cameraRight * localPosition.x) + (cameraUp * localPosition.y);
	float3 worldPosition = localBillboardPosition + ParticlesSRV[instanceIndex].WorldPosition;
	worldPosition += ParticlesSRV[instanceIndex].Velocity * ParticlesSRV[instanceIndex].Lifetime;

	float4 viewPosition  = mul(CameraView, float4(worldPosition.xyz, 1.0));
    
	vsOut.clipPosition = mul(CameraProjection, float4(viewPosition.xyz, 1.0));
	vsOut.worldPosition = worldPosition;
	vsOut.normal = float3(0.0, 1.0, 0.0);

	return vsOut;
}