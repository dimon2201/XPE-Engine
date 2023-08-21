struct Character
{
    uint TransformIndex;
    uint CameraIndex;
    float GlyphSize;
    float Width;
    float Height;
    float Left;
    float Top;
    float AdvanceX;
    float AdvanceY;
    float AtlasXOffset;
    float AtlasYOffset;
};

StructuredBuffer<Character> Text : K_SLOT_TEXT_CHARS;