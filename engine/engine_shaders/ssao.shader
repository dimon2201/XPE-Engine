struct SSAOData {
    int DirectionCount;
    int SampleCount;
    float Intensity;
};

StructuredBuffer<SSAOData> SSAO : K_SLOT_SSAO;