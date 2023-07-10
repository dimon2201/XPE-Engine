// ---------- For Structure Buffer --------------- //

#define K_SLOT_INSTANCES               register(t0)
#define K_SLOT_CAMERAS                 register(t1)
#define K_SLOT_MATERIALS               register(t2)
#define K_SLOT_DIRECT_LIGHTS           register(t3)
#define K_SLOT_POINT_LIGHTS            register(t4)
#define K_SLOT_SPOT_LIGHTS             register(t5)
#define K_SLOT_ENV_LIGHTS              register(t6)

// ---------- For Constant Buffer --------------- //


// ----------------- Constants ----------------- //

#define PI 3.14159265359

// ----------------- Globals ------------------ //

static float3 V = float3(0, 0, 0);  // view direction vector. from pixel surface to camera view position
static float3 R = float3(0, 0, 0);  // reflection direction vector. reflected vector between light vector and surface normal
static float2 UV = float3(0, 0, 0); // texture coordinates.
static float3 W = float3(0, 0, 0);  // world position vector of pixel
static float3 N = float3(0, 0, 0);  // normal direction vector of pixel surface
static float3 L = float3(0, 0, 0);  // light direction vector. from pixel surface to light position
