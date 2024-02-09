#pragma once

#include <buffers.hpp>

namespace xpe
{
    namespace render
    {
        struct ENGINE_API Viewports final
        {
            static sViewport*           Canvas;
            static sViewport*           DirectionalShadow;
            static sViewport*           PointShadow;
            static sViewport*           SpotShadow;
            static sViewport*           SSAO;
        };

        struct ENGINE_API RenderTargets final
        {
            static cRenderTarget*       Scene;
            static cRenderTarget*       Opaque;
            static cRenderTarget*       Transparent;
            static cRenderTarget*       UI;
            static cRenderTarget*       Final;
            static cRenderTarget*       DirectionalShadow;
            static cRenderTarget*       PointShadow;
            static cRenderTarget*       SpotShadow;
            static cRenderTarget*       SSAO;
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
            static cTexture*            Canvas;
            static cTexture*            SharedDepth;
            static cTexture*            AlbedoAtlas;
            static cTexture*            NormalAtlas;
            static cTexture*            ParallaxAtlas;
            static cTexture*            MetalAtlas;
            static cTexture*            RoughnessAtlas;
            static cTexture*            AOAtlas;
            static cTexture*            EmissionAtlas;
            static cAtlas*              WidgetAtlas;
            static cCircleFilter3D*     ShadowCircle;
        };

        struct ENGINE_API Samplers final
        {
            static cSampler*            Material;
            static cSampler*            Shadow;
            static cSampler*            Widget;
            static cSampler*            Skybox;
            static cSampler*            Font;
            static cSampler*            ShadowCircle;
        };
    }
}