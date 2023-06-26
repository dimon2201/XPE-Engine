#include "canvas.hpp"

xpe::core::Canvas::Canvas(s32 width, s32 height, RenderingContext_Interface* context)
{
    _size.x = width;
    _size.y = height;
    _context = context;
    _rt = _context->CreateRenderTarget(glm::ivec2(width, height), nullptr, nullptr, nullptr, nullptr);

    char* vertexStr = 
        "struct VSOut\
        { float4 positionClip : SV_POSITION; float2 texcoord : XPE_TEXCOORD; };\
        VSOut vs_main(uint vertexIndex : SV_VertexID)\
        {\
        VSOut vsOut;\
        if (vertexIndex == 0) { vsOut.positionClip = float4(-1.0, -1.0, 0.0, 1.0); vsOut.texcoord = float2(0.0, 1.0); }\
        if (vertexIndex == 1) { vsOut.positionClip = float4(-1.0, 1.0, 0.0, 1.0); vsOut.texcoord = float2(0.0, 0.0);  }\
        if (vertexIndex == 2) { vsOut.positionClip = float4(1.0, -1.0, 0.0, 1.0); vsOut.texcoord = float2(1.0, 1.0);  }\
        if (vertexIndex == 3) { vsOut.positionClip = float4(1.0, 1.0, 0.0, 1.0); vsOut.texcoord = float2(1.0, 0.0);  }\
        return vsOut;\
        }\
        ";
    char* pixelStr =
        "struct VSOut\
        { float4 positionClip : SV_POSITION; float2 texcoord : XPE_TEXCOORD; };\
        \
        Texture2D<float4> color : register(t0);\
        SamplerState colorSampler : register(s0);\
        \
        float4 ps_main(VSOut psIn) : SV_TARGET\
        {\
            uint textureWidth = 0;\
            uint textureHeight = 0;\
            color.GetDimensions(textureWidth, textureHeight);\
            float3 texcoordWorld = float3(float(textureWidth) * psIn.texcoord.x, float(textureHeight) * psIn.texcoord.y, 0);\
            return color.Load(int3(texcoordWorld));\
        }";

    _quad.PrimitiveTopology = ePrimitiveTopology::TRIANGLE_STRIP;
    _quad.Type = xShader::eType::VERTEX_PIXEL;
    _quad.Sources[0] = vertexStr;
    _quad.SourceEntryPoints[0] = "vs_main";
    _quad.SourceProfiles[0] = "vs_4_0";
    _quad.SourceFlags[0] = 0;
    _quad.Sources[1] = pixelStr;
    _quad.SourceEntryPoints[1] = "ps_main";
    _quad.SourceProfiles[1] = "ps_4_0";
    _quad.SourceFlags[1] = 0;
    _context->CreateShaderFromString(_quad);
    _context->CreateShaderFromString(_quad);
}

xpe::core::Canvas::Canvas(const glm::ivec2& size, RenderingContext_Interface* context) :
    xpe::core::Canvas(size.x, size.y, context)
{
}

xpe::core::Canvas::~Canvas()
{
    _context->FreeRenderTarget(_rt);
    _context->FreeShader(_quad);
}

void xpe::core::Canvas::Clear(const glm::vec4& color)
{
    _context->BindViewport(glm::ivec4(0.0f, 0.0f, _rt.Dimensions.x, _rt.Dimensions.y), 0.0f, 1.0f);
    _context->BindTexture(nullptr, xShader::eType::PIXEL, 0);
    _context->BindRenderTarget(&_rt);
    _context->ClearRenderTarget(color, 1.0f);
}

void xpe::core::Canvas::Present()
{
    glm::ivec2 swapChainDimensions = _context->GetSwapChainDimensions();
    _context->BindRenderTarget(nullptr);
    _context->BindViewport(glm::ivec4(0.0f, 0.0f, swapChainDimensions.x, swapChainDimensions.y), 0.0f, 1.0f);
    _context->BindShader(&_quad);
    _context->BindTexture(_rt.ColorTexture, xShader::eType::PIXEL, 0);
    _context->ClearRenderTarget(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), 1.0f);
    _context->DrawQuad();
    _context->Present();
}