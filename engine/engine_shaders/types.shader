// ---------- For Structure Buffer --------------- //

// ---------- For sVertex Stage --------------- //

#define K_SLOT_INSTANCES               register(t0)
#define K_SLOT_TEXT_CHARS              register(t1)
#define K_SLOT_SKELETONS               register(t2)
#define K_SLOT_UI                      register(t3)

// ---------- For Pixel Stage --------------- //

// materials
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

// ------------ For Samplers ----------------- //

#define K_SLOT_MATERIAL_SAMPLER        register(s0)
#define K_SLOT_SHADOW_SAMPLER          register(s1)

// ---------- For Constant Buffer --------------- //

// ---------- For sVertex Stage --------------- //

#define K_SLOT_CAMERA                  register(b0)

// ---------- For Pixel Stage --------------- //

#define K_SLOT_SSAO                    register(b0)
#define K_SLOT_SHADOW                  register(b0)

// ----------------- Constants ----------------- //

#define K_PI							3.14159265359
#define K_EPSILON						0.000001

// ----------------- Globals ------------------ //

static float3 V = float3(0, 0, 0);  // view direction vector. from pixel surface to camera view position
static float3 R = float3(0, 0, 0);  // reflection direction vector. reflected vector between light vector and surface normal
static float2 UV = float2(0, 0); // texture coordinates.
static float3 W = float3(0, 0, 0);  // world position vector of pixel
static float3 N = float3(0, 0, 0);  // normal direction vector of pixel surface
static float3 L = float3(0, 0, 0);  // light direction vector. from pixel surface to light position
