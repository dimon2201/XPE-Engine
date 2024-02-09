#pragma once

#define K_DEPTH_INVALID -1.0f

// ---------- Math --------------------------//

#define K_PI 3.14159265359f

// ---------- Memory measure ----------------//

#define K_MEMORY_KB 1024
#define K_MEMORY_MB (1024 * K_MEMORY_KB)
#define K_MEMORY_GB (1024 * K_MEMORY_MB)

// ---------- Shader Profiles ---------------//

#define K_PROFILE_VERTEX   "vs_5_0"
#define K_PROFILE_PIXEL    "ps_5_0"
#define K_PROFILE_GEOMETRY "gs_5_0"
#define K_PROFILE_COMPUTE  "cs_5_0"

// ---------- Shader Entry Points ----------//

#define K_ENTRY_POINT_VERTEX   "vs_main"
#define K_ENTRY_POINT_PIXEL    "ps_main"
#define K_ENTRY_POINT_GEOMETRY "gs_main"
#define K_ENTRY_POINT_COMPUTE  "cs_main"

#define K_SLOT_DEFAULT UINT32_MAX

// ---------- Structure Buffers ---------- //

// ---------- Vertex Stage --------------- //

// UI
#define K_SLOT_WIDGETS 0
#define K_SLOT_CHARS 0
#define K_SLOT_TEXTS 1
#define K_SLOT_WIDGET_SAMPLER 0
#define K_SLOT_WIDGET_ATLAS 0
#define K_SLOT_FONT_SAMPLER 0
#define K_SLOT_FONT_ATLAS 0

// Scene
#define K_SLOT_INSTANCES 0
#define K_SLOT_SKELETONS 1

// ---------- Pixel Stage --------------- //

// materials
#define K_SLOT_MATERIAL_SAMPLER 0
#define K_SLOT_MATERIALS 0
#define K_SLOT_ALBEDO 1
#define K_SLOT_BUMPING 2
#define K_SLOT_PARALLAX 3
#define K_SLOT_METALLIC 4
#define K_SLOT_ROUGHNESS 5
#define K_SLOT_AO 6
#define K_SLOT_EMISSION 7

// lights
#define K_SLOT_DIRECT_LIGHTS 8
#define K_SLOT_POINT_LIGHTS 9
#define K_SLOT_SPOT_LIGHTS 10

// shadows
#define K_SLOT_SHADOW_PCF 0
#define K_SLOT_SHADOW_ATLAS 11
#define K_SLOT_SHADOW_SAMPLER 1
#define K_SLOT_SHADOW_RANDOM_TEXTURE 12
#define K_SLOT_SHADOW_CIRCLE_SAMPLER 2
#define K_SLOT_SHADOW_CIRCLE_FILTER 13

// ---------- Constant Buffers --------------- //

// ---------- Vertex Stage --------------- //

#define K_SLOT_CAMERA 0

// ---------- Pixel Stage --------------- //

#define K_SLOT_SSAO 0