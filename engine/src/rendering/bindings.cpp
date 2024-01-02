#include <rendering/bindings.hpp>

namespace xpe {

    namespace render {

        sViewport*           Viewports::Shadow = nullptr;
        sViewport*           Viewports::SSAO = nullptr;

        sRenderTarget*       RenderTargets::Scene = nullptr;
        sRenderTarget*       RenderTargets::Opaque = nullptr;
        sRenderTarget*       RenderTargets::Transparent = nullptr;
        sRenderTarget*       RenderTargets::UI = nullptr;
        sRenderTarget*       RenderTargets::Final = nullptr;
        sRenderTarget*       RenderTargets::Shadow = nullptr;
        sRenderTarget*       RenderTargets::SSAO = nullptr;

        cVertexBuffer*                  Buffers::Vertex = nullptr;
        cIndexBuffer*                   Buffers::Index = nullptr;
        cCameraBuffer*                  Buffers::Camera = nullptr;
        cDirectLightBuffer*             Buffers::DirectLight = nullptr;
        cPointLightBuffer*              Buffers::PointLight = nullptr;
        cSpotLightBuffer*               Buffers::SpotLight = nullptr;
        cMaterialDataBuffer*            Buffers::Material = nullptr;
        cShadowPcfBuffer*               Buffers::ShadowPCF = nullptr;
        cSkeletonBuffer*                Buffers::Skeleton = nullptr;
        cSsaoBuffer*                    Buffers::SSAO = nullptr;
        cParticleBuffer*                Buffers::Particle = nullptr;
        cParticleEmitterBuffer*         Buffers::ParticleEmitter = nullptr;

        cTexture*            Textures::Canvas = nullptr;
        cTexture*            Textures::SharedDepth = nullptr;
        cTexture*            Textures::AlbedoAtlas = nullptr;
        cTexture*            Textures::NormalAtlas = nullptr;
        cTexture*            Textures::ParallaxAtlas = nullptr;
        cTexture*            Textures::MetalAtlas = nullptr;
        cTexture*            Textures::RoughnessAtlas = nullptr;
        cTexture*            Textures::AOAtlas = nullptr;
        cTexture*            Textures::EmissionAtlas = nullptr;
        sAtlas*              Textures::WidgetAtlas = nullptr;

        sSampler*            Samplers::Material = nullptr;
        sSampler*            Samplers::Shadow = nullptr;
        sSampler*            Samplers::Widget = nullptr;
        sSampler*            Samplers::Skybox = nullptr;
        sSampler*            Samplers::Font = nullptr;

    }

}