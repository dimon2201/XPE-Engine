#pragma once

#include <rendering/buffers.h>

namespace xpe {

    namespace render {

        struct ENGINE_API Viewports final
        {
            static sViewport*           Shadow;
            static sViewport*           SSAO;
        };

        struct ENGINE_API RenderTargets final
        {
            static sRenderTarget*       Scene;
            static sRenderTarget*       Opaque;
            static sRenderTarget*       Transparent;
            static sRenderTarget*       UI;
            static sRenderTarget*       Final;
            static sRenderTarget*       Shadow;
            static sRenderTarget*       SSAO;
        };

        struct ENGINE_API Buffers final
        {
            static sVertexBuffer*       Vertex;
            static sIndexBuffer*        Index;
            static sCameraBuffer*       Camera;
            static sDirectLightBuffer*  DirectLight;
            static sPointLightBuffer*   PointLight;
            static sSpotLightBuffer*    SpotLight;
            static sMaterialDataBuffer* Material;
            static sShadowPcfBuffer*    ShadowPCF;
            static sSkeletonBuffer*     Skeleton;
            static sSsaoBuffer*         SSAO;
        };

        struct ENGINE_API Textures final
        {
            static sTexture*            Canvas;
            static sTexture*            SharedDepth;
            static sTexture*            AlbedoAtlas;
            static sTexture*            NormalAtlas;
            static sTexture*            ParallaxAtlas;
            static sTexture*            MetalAtlas;
            static sTexture*            RoughnessAtlas;
            static sTexture*            AOAtlas;
            static sTexture*            EmissionAtlas;
            static sAtlas*              WidgetAtlas;
        };

        struct ENGINE_API Samplers final
        {
            static sSampler*            Material;
            static sSampler*            Shadow;
            static sSampler*            Widget;
            static sSampler*            Skybox;
            static sSampler*            Font;
        };

    }

}