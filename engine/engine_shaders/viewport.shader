struct Viewport {
    float TopLeftX;
    float TopLeftY;
    float Width;
    float Height;
    float MinDepth;
    float MaxDepth;
};

StructuredBuffer<Viewport> Viewports : K_SLOT_VIEWPORT;