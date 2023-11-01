struct Character
{
    float GlyphSize;
    float Width;
    float Height;
    float Left;
    float Top;
    float AdvanceX;
    float AdvanceY;
    float AtlasXOffset;
    float AtlasYOffset;
    float4x4 ModelMatrix;
};

StructuredBuffer<Character> Text : K_SLOT_TEXT_CHARS;