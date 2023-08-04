struct RenderInstance
{
    uint TransformIndex;
    uint CameraIndex;
    uint MaterialIndex;
};

StructuredBuffer<RenderInstance> Instances : K_SLOT_INSTANCES;

struct RenderInstance2D
{
    uint TransformIndex;
    uint CameraIndex;
};

StructuredBuffer<RenderInstance2D> Instances2D : K_SLOT_INSTANCES2D;

struct TextGlyphInstance
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

StructuredBuffer<TextGlyphInstance> TextGlyphs : K_SLOT_TEXT_GLYPH_INSTANCES;