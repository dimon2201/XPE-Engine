#pragma once

#define K_SLOT_DEFAULT UINT32_MAX

// ---------- Structure Buffers --------------- //

// ---------- sVertex Stage --------------- //

#define K_SLOT_INSTANCES 0
#define K_SLOT_TRANSFORMS 1
#define K_SLOT_CAMERA 2
#define K_SLOT_TEXT_CHARS 3
#define K_SLOT_VIEWPORT 4
#define K_SLOT_BONES 5

// ---------- Pixel Stage --------------- //

// core
#define K_SLOT_MONITOR 0

// materials
#define K_SLOT_MATERIALS 1
#define K_SLOT_ALBEDO 2
#define K_SLOT_BUMPING 3
#define K_SLOT_PARALLAX 4
#define K_SLOT_METALLIC 5
#define K_SLOT_ROUGHNESS 6
#define K_SLOT_AO 7
#define K_SLOT_EMISSION 8

// lights
#define K_SLOT_DIRECT_LIGHTS 9
#define K_SLOT_POINT_LIGHTS 10
#define K_SLOT_SPOT_LIGHTS 11

// shadows
#define K_SLOT_SHADOW_FILTER 12
#define K_SLOT_SHADOW_MAP 13

// SSAO
#define K_SLOT_SSAO 0

// ---------- Samplers --------------- //

// materials
#define K_SLOT_MATERIAL_SAMPLER 0

// shadows
#define K_SLOT_SHADOW_SAMPLER 1

// ---------- Constant Buffers --------------- //