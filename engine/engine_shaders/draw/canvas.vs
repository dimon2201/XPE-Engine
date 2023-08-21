struct VSOut {
    float4 positionClip : SV_POSITION;
    float2 texcoord : XPE_UV;
};

VSOut vs_main(uint vertexIndex : SV_VertexID) {
    VSOut vsOut;

    if (vertexIndex == 0) { vsOut.positionClip = float4(-1.0, -1.0, 0.0, 1.0); vsOut.texcoord = float2(0.0, 1.0); }
    if (vertexIndex == 1) { vsOut.positionClip = float4(-1.0, 1.0, 0.0, 1.0); vsOut.texcoord = float2(0.0, 0.0);  }
    if (vertexIndex == 2) { vsOut.positionClip = float4(1.0, -1.0, 0.0, 1.0); vsOut.texcoord = float2(1.0, 1.0);  }
    if (vertexIndex == 3) { vsOut.positionClip = float4(1.0, 1.0, 0.0, 1.0); vsOut.texcoord = float2(1.0, 0.0);  }

    return vsOut;
}