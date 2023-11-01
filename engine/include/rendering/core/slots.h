#pragma once

#define K_SLOT_DEFAULT UINT32_MAX

// ---------- Structure Buffers --------------- //

// ---------- sVertex Stage --------------- //

#define K_SLOT_INSTANCES 0
#define K_SLOT_TEXT_CHARS 1
#define K_SLOT_SKELETONS 2
#define K_SLOT_UI 3

// ---------- Pixel Stage --------------- //

// materials
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
#define K_SLOT_SHADOW_ATLAS 11

// ---------- Samplers --------------- //

#define K_SLOT_MATERIAL_SAMPLER 0
#define K_SLOT_SHADOW_SAMPLER 1

// ---------- Constant Buffers --------------- //

// ---------- Vertex Stage --------------- //

#define K_SLOT_CAMERA 0

// ---------- Pixel Stage --------------- //

#define K_SLOT_SSAO 0
#define K_SLOT_SHADOW 0