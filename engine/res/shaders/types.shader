// ---------- For Structure Buffer --------------- //

// ---------- Vertex Stage --------------- //

// UI
#define K_SLOT_WIDGETS                 register(t0)
#define K_SLOT_CHARS                   register(t0)
#define K_SLOT_TEXTS                   register(t1)
#define K_SLOT_WIDGET_SAMPLER          register(s0)
#define K_SLOT_WIDGET_ATLAS            register(t0)
#define K_SLOT_FONT_SAMPLER            register(s0)
#define K_SLOT_FONT_ATLAS              register(t0)

// Scene
#define K_SLOT_INSTANCES               register(t0)
#define K_SLOT_SKELETONS               register(t1)

// ---------- Pixel Stage --------------- //

// materials
#define K_SLOT_MATERIAL_SAMPLER        register(s0)
#define K_SLOT_MATERIALS               register(t0)
#define K_SLOT_ALBEDO                  register(t1)
#define K_SLOT_BUMPING                 register(t2)
#define K_SLOT_PARALLAX                register(t3)
#define K_SLOT_METALLIC                register(t4)
#define K_SLOT_ROUGHNESS               register(t5)
#define K_SLOT_AO                      register(t6)
#define K_SLOT_EMISSION                register(t7)

// lights
#define K_SLOT_DIRECT_LIGHTS           register(t8)
#define K_SLOT_POINT_LIGHTS            register(t9)
#define K_SLOT_SPOT_LIGHTS             register(t10)

// shadows
#define K_SLOT_SHADOW_ATLAS            register(t11)
#define K_SLOT_SHADOW_SAMPLER          register(s1)
#define K_SLOT_SHADOW_PCF              register(b0)

// ---------- Constant Buffer --------------- //

// ---------- Vertex Stage --------------- //

#define K_SLOT_CAMERA                  register(b0)

// ---------- Pixel Stage --------------- //

#define K_SLOT_SSAO                    register(b0)

// ----------------- Constants ----------------- //

#define K_PI							3.14159265359
#define K_EPSILON						0.000001

// ----------------- Globals ------------------ //

static float3 V = float3(0, 0, 0);  // view direction vector. from pixel surface to camera view position
static float3 R = float3(0, 0, 0);  // reflection direction vector. reflected vector between light vector and surface normal
static float2 UV = float2(0, 0);    // texture coordinates.
static float3 W = float3(0, 0, 0);  // world position vector of pixel
static float3 N = float3(0, 0, 0);  // normal direction vector of pixel surface
static float3 L = float3(0, 0, 0);  // light direction vector. from pixel surface to light position

float LinearizeDepth(float depth, float near, float far)
{
    float z = depth * 2.0 - 1.0; // Back to NDC
    z = (2.0 * near * far) / (far + near - z * (far - near));
    z /= far;
    return z;
}

float3 ToLightSpace(float3 v, float4x4 m)
{
    float4 l = mul(m, float4(v.xyz, 1.0));
    l = float4(l.xyz / l.w, 1.0) * 0.5 + 0.5; // transform into [0, 1]
    l.y = 1.0 - l.y;                          // HOTFIX: flip Y coordinate for correct UV projection of shadow
    l.z = l.z > 1.0 ? 1.0 : l.z;              // clamp depth value to 1
    return l.xyz;
}