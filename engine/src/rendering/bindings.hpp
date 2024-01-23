#pragma once

#include <rendering/buffers.hpp>

namespace xpe {

    namespace render {

        struct ENGINE_API Viewports final
        {
            static sViewport*           Shadow;
            static sViewport*           Main;
            static sViewport*           SSAO;
        };

        struct ENGINE_API RenderTargets final
        {
            static cRenderTarget*       Scene;
            static cRenderTarget*       Opaque;
            static cRenderTarget*       Transparent;
            static cRenderTarget*       UI;
            static cRenderTarget*       Final;
            static cRenderTarget*       Shadow;
            static cRenderTarget*       SSAO;
        };

        struct ENGINE_API Buffers final
        {
            static cVertexBuffer*               Vertex;
            static cIndexBuffer*                Index;
            static cCameraBuffer*               Camera;
            static cLightBuffer*                DirectLight;
            static cPointLightBuffer*           PointLight;
            static cSpotLightBuffer*            SpotLight;
            static cMaterialDataBuffer*         Material;
            static cShadowPcfBuffer*            ShadowPCF;
            static cSkeletonBuffer*             Skeleton;
            static cSsaoBuffer*                 SSAO;
            static cParticleBuffer*             Particle;
            static cParticleEmitterBuffer*      ParticleEmitter;
            static cParticlePassInfoBuffer*     ParticlePassInfo;
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
            static sSampler*            Widget;
            static sSampler*            Skybox;
            static sSampler*            Font;
        };

    }

}