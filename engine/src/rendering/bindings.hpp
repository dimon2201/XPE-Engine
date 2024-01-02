#pragma once

#include <rendering/buffers.hpp>

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
            static cVertexBuffer*               Vertex;
            static cIndexBuffer*                Index;
            static cCameraBuffer*               Camera;
            static cDirectLightBuffer*          DirectLight;
            static cPointLightBuffer*           PointLight;
            static cSpotLightBuffer*            SpotLight;
            static cMaterialDataBuffer*         Material;
            static cShadowPcfBuffer*            ShadowPCF;
            static cSkeletonBuffer*             Skeleton;
            static cSsaoBuffer*                 SSAO;
            static cParticleBuffer*             Particle;
            static cParticleEmitterBuffer*      ParticleEmitter;
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