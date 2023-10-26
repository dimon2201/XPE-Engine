// ---------- For Structure Buffer --------------- //

// ---------- For Vertex Stage --------------- //

#define K_SLOT_INSTANCES               register(t0)
#define K_SLOT_INSTANCES2D             register(t1)
#define K_SLOT_TRANSFORMS              register(t2)
#define K_SLOT_TRANSFORMS2D            register(t3)
#define K_SLOT_CAMERAS                 register(t4)
#define K_SLOT_TEXT_CHARS              register(t5)
#define K_SLOT_VIEWPORT                register(t6)
#define K_SLOT_BONES                   register(t7)

// ---------- For Pixel Stage --------------- //

// core
#define K_SLOT_MONITOR                 register(t0)

// materials
#define K_SLOT_MATERIALS               register(t1)
#define K_SLOT_ALBEDO                  register(t2)
#define K_SLOT_BUMPING                 register(t3)
#define K_SLOT_PARALLAX                register(t4)
#define K_SLOT_METALLIC                register(t5)
#define K_SLOT_ROUGHNESS               register(t6)
#define K_SLOT_AO                      register(t7)
#define K_SLOT_EMISSION                register(t8)

// lights
#define K_SLOT_DIRECT_LIGHTS           register(t9)
#define K_SLOT_POINT_LIGHTS            register(t10)
#define K_SLOT_SPOT_LIGHTS             register(t11)

// shadows
#define K_SLOT_SHADOW_FILTER           register(t12)
#define K_SLOT_SHADOW_MAP              register(t13)
#define K_SLOT_SHADOW_COORDS           register(t14)

// ------------ For Samplers ----------------- //

// materials
#define K_SLOT_MATERIAL_SAMPLER        register(s0)

// shadows
#define K_SLOT_SHADOW_SAMPLER          register(s1)

// ---------- For Constant Buffer --------------- //


// ----------------- Constants ----------------- //

#define K_PI							3.14159265359
#define K_EPSILON						0.000001

// ----------------- Globals ------------------ //

static float3 V = float3(0, 0, 0);  // view direction vector. from pixel surface to camera view position
static float3 R = float3(0, 0, 0);  // reflection direction vector. reflected vector between light vector and surface normal
static float2 UV = float3(0, 0, 0); // texture coordinates.
static float3 W = float3(0, 0, 0);  // world position vector of pixel
static float3 N = float3(0, 0, 0);  // normal direction vector of pixel surface
static float3 L = float3(0, 0, 0);  // light direction vector. from pixel surface to light position
